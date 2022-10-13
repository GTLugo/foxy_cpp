#include "context.hpp"

#include "shader.hpp"
#include <vulkan/static.hpp>
#include <GLFW/glfw3.h>

namespace fx::ookami {
  static auto vulkan_error_callback(
    const vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    vk::DebugUtilsMessageTypeFlagBitsEXT,
    const vk::DebugUtilsMessengerCallbackDataEXT* callback_data,
    void*
  ) -> vk::Bool32
  {
    std::stringstream msg{};
    msg << callback_data->pMessage << " | code " << callback_data->messageIdNumber << ", " << callback_data->pMessageIdName;
    switch (severity) {
      case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
        fx::Log::error(msg.str());
        break;
      case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
        fx::Log::trace(msg.str());
        break;
      case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
      case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
        break;
    }

    return true;
  }

  auto required_instance_extensions_strings() -> std::vector<std::string> {
    std::vector<std::string> result;
    auto count = fx::u32{ 0 };
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);
    for (auto i = fx::u32{ 0 }; i < count; ++i) {
      result.emplace_back(extensions[i]);
    }
    return result;
  }

  auto required_instance_extensions() -> std::vector<const char*> {
    auto count = fx::u32{ 0 };
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> result{extensions, extensions + count};
    return result;
  }

  class Context::Impl {
  public:
    explicit Impl(const fx::shared<GLFWwindow>& window, bool enable_validation = true)
      : enable_validation_{enable_validation},
        window_{window},
        context_{vk::raii::Context{}},
        extension_data_{ExtensionData{
          .device_extensions = std::vector<const char*>{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
          }
        }},
        instance_{create_instance()},
        debug_messenger_{try_create_debug_messenger()},
        surface_{create_surface(window)},
        physical_device_{pick_physical_device()},
        queue_family_indices_{find_queue_families(physical_device_)},
        logical_device_{create_logical_device()},
        graphics_queue_{logical_device_.getQueue(queue_family_indices_.graphics.value(), 0)},
        present_queue_{logical_device_.getQueue(queue_family_indices_.present.value(), 0)} {
      fx::Log::trace("Vulkan context ready.");
    }

    ~Impl() {
      fx::Log::trace("Destroying Vulkan context...");
    }
    
    void wait_for_fence(const vk::raii::Fence& fence)
    {
      if (auto result{ logical_device_.waitForFences(*fence, true, std::numeric_limits<u64>::max()) };
          result != vk::Result::eSuccess) {
        Log::error(to_string(result));
      }
    }
  
    void reset_fence(const vk::raii::Fence& fence)
    {
      logical_device_.resetFences(*fence);
    }

    [[nodiscard]] auto window() -> fx::shared<GLFWwindow> {
      return window_;
    }

    [[nodiscard]] auto native() -> vk::raii::Context& {
      return context_;
    }

    [[nodiscard]] auto instance() -> vk::raii::Instance& {
      return instance_;
    }

    [[nodiscard]] auto surface() -> vk::raii::SurfaceKHR& {
      return surface_;
    }
  
    [[nodiscard]] auto graphics_queue() -> vk::raii::Queue& {
      return graphics_queue_;
    }
  
    [[nodiscard]] auto present_queue() -> vk::raii::Queue& {
      return present_queue_;
    }

    [[nodiscard]] auto query_swapchain_support() const -> SwapchainSupportInfo {
      return query_swapchain_support(physical_device_);
    }

    [[nodiscard]] auto queue_families() const -> const QueueFamilyIndices& {
      return queue_family_indices_;
    }

    [[nodiscard]] auto physical_device() -> vk::raii::PhysicalDevice& {
      return physical_device_;
    }

    [[nodiscard]] auto logical_device() -> vk::raii::Device& {
      return logical_device_;
    }

  private:
    static inline const std::vector<const char*> validation_layer_names_ = {
      "VK_LAYER_KHRONOS_validation",
      "VK_LAYER_LUNARG_monitor"
    };

    bool enable_validation_;

    fx::shared<GLFWwindow> window_;

    vk::raii::Context context_;
    ExtensionData extension_data_;
    vk::raii::Instance instance_;
    vk::raii::DebugUtilsMessengerEXT debug_messenger_;

    vk::raii::SurfaceKHR surface_;

    vk::raii::PhysicalDevice physical_device_;
    QueueFamilyIndices queue_family_indices_;
    vk::raii::Device logical_device_;

    vk::raii::Queue graphics_queue_;
    vk::raii::Queue present_queue_;


    [[nodiscard]] static auto check_validation_layer_support() -> bool {
      auto layer_count = fx::u32{ 0 };
      vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
      auto layer_properties = std::vector<VkLayerProperties>{ layer_count };
      vkEnumerateInstanceLayerProperties(&layer_count, layer_properties.data());

      std::stringstream lyrs;
      lyrs << '(' << layer_properties.size() << "):";
      for (const auto& layer: layer_properties) {
        lyrs << " | " << layer.layerName;
      }
      fx::Log::debug("Available Vulkan Layers {}", lyrs.str());

      for (const auto& layer_name: validation_layer_names_) {
        auto layer_found{ false };
        for (const auto& layer: layer_properties) {
          if (std::strcmp(layer_name, layer.layerName) == 0) {
            layer_found = true;
            break;
          }
        }

        if (!layer_found) {
          return false;
        }
      }

      return true;
    }

    [[nodiscard]] auto try_create_debug_messenger() -> vk::raii::DebugUtilsMessengerEXT {
      if (!enable_validation_) {
        return nullptr;
      }

      const auto callback_create_info = vk::DebugUtilsMessengerCreateInfoEXT{
        .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
        .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
        .pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(vulkan_error_callback),  // NOLINT(clang-diagnostic-cast-function-type)
      };

      try {
        return vk::raii::DebugUtilsMessengerEXT{ instance_, callback_create_info };
      } catch (const std::exception& e) {
        fx::Log::fatal("Failed to set up debug messenger: {}", e.what());
        return nullptr;
      }
    }

    [[nodiscard]] auto get_enabled_extensions() -> std::vector<const char*> {
      std::set<const char*> required_extensions{};
      // GLFW
      extension_data_.window_extensions = required_instance_extensions();
      for (const auto& extension: extension_data_.window_extensions) {
        if (!required_extensions.contains(extension)) {
          required_extensions.insert(extension);
        }
      }
      // Vulkan
      auto extension_count = fx::u32{ 0 };
      vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
      extension_data_.instance_extensions = std::vector<VkExtensionProperties>{ extension_count };
      vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_data_.instance_extensions.data());
      for (const auto& [extensionName, specVersion]: extension_data_.instance_extensions) {
        if (!required_extensions.contains(extensionName)) {
          required_extensions.insert(extensionName);
        }
      }

      if (enable_validation_ && !required_extensions.contains(VK_EXT_DEBUG_REPORT_EXTENSION_NAME)) {
        required_extensions.insert(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
      }

      std::stringstream exts;
      exts << '(' << required_extensions.size() << "):";
      for (const auto& extension: required_extensions) {
        exts << " | " << extension;
      }
      fx::Log::debug("Enabled Vulkan Extensions {}", exts.str());

      return {required_extensions.begin(), required_extensions.end()};
    }

    [[nodiscard]] auto create_instance() -> Instance {
      vk::ApplicationInfo app_info{
        .pApplicationName = "FOXY APP",
        .pEngineName = "FOXY FRAMEWORK",
        .apiVersion = VK_API_VERSION_1_3
      };

      vk::InstanceCreateInfo instance_create_info{
        .pApplicationInfo = &app_info
      };

      if (enable_validation_ && !check_validation_layer_support()) {
        fx::Log::error("Validation layers requested, but none are available. Disabling validation.");
        enable_validation_ = false;
      }
      extension_data_.enabled_extensions = get_enabled_extensions();
      if (!extension_data_.enabled_extensions.empty()) {
        instance_create_info.enabledExtensionCount = static_cast<fx::u32>(extension_data_.enabled_extensions.size());
        instance_create_info.ppEnabledExtensionNames = extension_data_.enabled_extensions.data();
      }

      auto debug_create_info = vk::DebugUtilsMessengerCreateInfoEXT{
        .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
        .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
        .pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(vulkan_error_callback),  // NOLINT(clang-diagnostic-cast-function-type)
      };
      if (enable_validation_) {
        instance_create_info.enabledLayerCount = static_cast<fx::u32>(validation_layer_names_.size());
        instance_create_info.ppEnabledLayerNames = validation_layer_names_.data();
        instance_create_info.pNext = reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debug_create_info);
      } else {
        instance_create_info.enabledLayerCount = 0;
      }

      try {
        return { context_, instance_create_info };
      } catch (const std::exception& e) {
        fx::Log::fatal(e.what());
        return nullptr;
      }
    }

    [[nodiscard]] auto find_queue_families(const PhysicalDevice& physical_device) const -> QueueFamilyIndices {
      auto queue_family_indices = QueueFamilyIndices{};
      const auto queue_family_properties = physical_device.getQueueFamilyProperties();

      fx::u32 i{ 0 };
      for (const auto& queue_family: queue_family_properties) {
        if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics) {
          queue_family_indices.graphics = i;
        }

        if (physical_device.getSurfaceSupportKHR(i, *surface_)) {
          queue_family_indices.present = i;
        }

        if (queue_family_indices.complete()) {
          break;
        }

        ++i;
      }

      return queue_family_indices;
    }

    [[nodiscard]] auto check_device_extension_support(const PhysicalDevice& device) -> bool {
      std::set<std::string> required_instance_extensions{
        extension_data_.device_extensions.begin(),
        extension_data_.device_extensions.end()
      };

      // transform vector<Properties> to set<string>
      std::set<std::string> available_extensions{};
      std::ranges::transform(
        device.enumerateDeviceExtensionProperties(),
        std::inserter(available_extensions, available_extensions.end()),
        [&](const vk::ExtensionProperties& properties) {
          return properties.extensionName;
        });

      // make sure none of the required extensions are missing from the available extensions
      return !std::ranges::any_of(required_instance_extensions, [&](const std::string& extension) {
        return !available_extensions.contains(extension);
      });
    }

    [[nodiscard]] auto query_swapchain_support(const PhysicalDevice& device) const -> SwapchainSupportInfo {
      SwapchainSupportInfo info{
        .capabilities = device.getSurfaceCapabilitiesKHR(*surface_),
        .formats = device.getSurfaceFormatsKHR(*surface_),
        .present_modes = device.getSurfacePresentModesKHR(*surface_),
      };

      return info;
    }

    [[nodiscard]] auto device_suitable(const PhysicalDevice& physical_device) -> bool {
      const auto queue_family_indices{ find_queue_families(physical_device) };

      bool valid_swapchain{ false };
      if (check_device_extension_support(physical_device)) {
        const auto [capabilities, formats, present_modes]{ query_swapchain_support(physical_device) };
        valid_swapchain = !formats.empty() && !present_modes.empty();
      }

      return queue_family_indices.complete() && valid_swapchain;
    }

    [[nodiscard]] auto pick_physical_device() -> PhysicalDevice {
      // TODO: Rank suitability and pick highest scoring device
      const vk::raii::PhysicalDevices physical_devices{ instance_ };
      if (physical_devices.empty()) {
        fx::Log::fatal("No physical Vulkan devices found.");
      }
      vk::raii::PhysicalDevice physical_device{ nullptr };
      for (const auto& device: physical_devices) {
        if (device_suitable(device)) {
          physical_device = vk::raii::PhysicalDevice{ device }; // this is a *move* (devices are not copyable)
          break;
        }
      }
      if (physical_devices.empty()) {
        fx::Log::fatal("No *viable* physical Vulkan devices found.");
      }

      vk::PhysicalDeviceProperties device_properties{ physical_device.getProperties() };
      std::string device_name{ static_cast<const char*>(device_properties.deviceName) };
      const Version api_version{ device_properties.apiVersion };
      const Version driver_version{ device_properties.driverVersion };

      fx::Log::debug("Vulkan Device: {} | Device driver version: {} | Vulkan API version: {}", 
        device_name, driver_version.to_string(), api_version.to_string());

      return physical_device;
    }

    [[nodiscard]] auto create_logical_device() -> LogicalDevice {
      std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
      const std::set queue_families{
        queue_family_indices_.graphics.value(),
        queue_family_indices_.present.value(),
      };
      float queue_priority = 1.f;
      for (auto family: queue_families) {
        queue_create_infos.push_back({
          .queueFamilyIndex = family,
          .queueCount = 1,
          .pQueuePriorities = &queue_priority,
        });
      }

      const vk::DeviceCreateInfo device_create_info{
        .queueCreateInfoCount = static_cast<fx::u32>(queue_create_infos.size()),
        .pQueueCreateInfos = queue_create_infos.data(),
        .enabledExtensionCount = static_cast<fx::u32>(extension_data_.device_extensions.size()),
        .ppEnabledExtensionNames = extension_data_.device_extensions.data(),
      };

      return { physical_device_.createDevice(device_create_info, nullptr) };
    }

    [[nodiscard]] auto create_surface(const fx::shared<GLFWwindow>& window) const -> Surface {
      VkSurfaceKHR raw_surface;

      const auto result = static_cast<vk::Result>(
        glfwCreateWindowSurface(*instance_, window.get(), nullptr, &raw_surface)
      );

      return vk::raii::SurfaceKHR{ instance_, vk::createResultValueType(result, raw_surface) };
    }
  };

  //
  //  Context
  //

  Context::Context(const fx::shared<GLFWwindow>& window, bool enable_validation):
    p_impl_{std::make_unique<Impl>(window, enable_validation)} {}

  Context::~Context() = default;
  
  void Context::wait_for_fence(const vk::raii::Fence& fence)
  {
    p_impl_->wait_for_fence(fence);
  }
  
  void Context::reset_fence(const vk::raii::Fence& fence)
  {
    p_impl_->reset_fence(fence);
  }
  
  auto Context::window() -> shared<GLFWwindow>
  {
    return p_impl_->window();
  }

  auto Context::native() -> vk::raii::Context& {
    return p_impl_->native();
  }

  auto Context::instance() -> vk::raii::Instance& {
    return p_impl_->instance();
  }

  auto Context::surface() -> Surface& {
    return p_impl_->surface();
  }
  
  auto Context::graphics_queue() -> vk::raii::Queue& {
    return p_impl_->graphics_queue();
  }
  
  auto Context::present_queue() -> vk::raii::Queue& {
    return p_impl_->present_queue();
  }

  auto Context::query_swapchain_support() const -> SwapchainSupportInfo {
    return p_impl_->query_swapchain_support();
  }

  auto Context::queue_families() const -> const QueueFamilyIndices& {
    return p_impl_->queue_families();
  }

  auto Context::physical_device() -> vk::raii::PhysicalDevice& {
    return p_impl_->physical_device();
  }

  auto Context::logical_device() -> vk::raii::Device& {
    return p_impl_->logical_device();
  }
  
  auto Context::create_shader(const ShaderCreateInfo& shader_create_info) -> unique<Shader>
  {
    return std::make_unique<Shader>(
      logical_device(),
      shader_create_info
    );
  }
  
  auto Context::operator*() -> VulkanContext& {
    return p_impl_->native();
  }
}