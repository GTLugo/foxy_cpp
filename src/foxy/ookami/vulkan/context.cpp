#include "context.hpp"

#include "foxy/ookami/vulkan/vulkan.hpp"
#include "foxy/core/window/glfw/context.hpp"
#include "version.hpp"

namespace foxy::vulkan {
  inline static VKAPI_ATTR auto vulkan_error_callback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                                      vk::DebugUtilsMessageTypeFlagBitsEXT type,
                                                      const vk::DebugUtilsMessengerCallbackDataEXT* callback_data,
                                                      void*) -> vk::Bool32 {
    std::stringstream msg{};
//    switch (type) {
//      case vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral:
//        msg << "[GENR] ";
//        break;
//      case vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation:
//        msg << "[VALD] ";
//        break;
//      case vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance:
//        msg << "[PERF] ";
//        break;
//    }

    msg << callback_data->pMessage << " | code " << callback_data->messageIdNumber << ", " << callback_data->pMessageIdName;
    switch (severity) {
      case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
        FOXY_ERROR << msg.str();
        break;
      case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
        FOXY_WARN << msg.str();
        break;
      default:
        FOXY_INFO << msg.str();
        break;
    }

    return true;
  }

  class Context::Impl {
  public:
    explicit Impl(glfw::UniqueWindow& window, bool enable_validation = true)
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
      FOXY_TRACE << "Vulkan context ready.";
    }

    ~Impl() {

    }

    [[nodiscard]] auto window() -> glfw::UniqueWindow& {
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

    [[nodiscard]] auto query_swapchain_support() -> SwapchainSupportInfo {
      return query_swapchain_support(physical_device_);
    }

    [[nodiscard]] auto queue_families() -> const QueueFamilyIndices& {
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
      "VK_LAYER_KHRONOS_validation"
    };

    const bool enable_validation_;

    glfw::UniqueWindow& window_;

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
      auto layer_count = u32{ 0 };
      vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
      auto layer_properties = std::vector<VkLayerProperties>{ layer_count };
      vkEnumerateInstanceLayerProperties(&layer_count, layer_properties.data());

      std::stringstream lyrs;
      lyrs << '(' << layer_properties.size() << "):";
      for (const auto& layer: layer_properties) {
        lyrs << " | " << layer.layerName;
      }
      FOXY_DEBUG << "Available Vulkan Layers " << lyrs.str();

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

      auto callback_create_info = vk::DebugUtilsMessengerCreateInfoEXT{
        .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
        .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
        .pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)(vulkan_error_callback),
      };

      try {
        return vk::raii::DebugUtilsMessengerEXT{ instance_, callback_create_info };
      } catch (const std::exception& e) {
        FOXY_FATAL << "Failed to set up debug messenger.";
        return nullptr;
      }
    }

    [[nodiscard]] auto get_enabled_extensions() -> std::vector<const char*> {
      std::set<const char*> required_instance_extensions{};
      // GLFW
      extension_data_.window_extensions = glfw::required_instance_extensions();
      for (const auto& extension: extension_data_.window_extensions) {
        if (!required_instance_extensions.contains(extension)) {
          required_instance_extensions.insert(extension);
        }
      }
      // Vulkan
      auto extension_count = u32{ 0 };
      vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
      extension_data_.instance_extensions = std::vector<VkExtensionProperties>{ extension_count };
      vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_data_.instance_extensions.data());
      for (const auto& extension: extension_data_.instance_extensions) {
        if (!required_instance_extensions.contains(extension.extensionName)) {
          required_instance_extensions.insert(extension.extensionName);
        }
      }

      if (enable_validation_ && !required_instance_extensions.contains(VK_EXT_DEBUG_REPORT_EXTENSION_NAME)) {
        required_instance_extensions.insert(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
      }

      std::stringstream exts;
      exts << '(' << required_instance_extensions.size() << "):";
      for (const auto& extension: required_instance_extensions) {
        exts << " | " << extension;
      }
      FOXY_DEBUG << "Enabled Vulkan Extensions " << exts.str();

      return {required_instance_extensions.begin(), required_instance_extensions.end()};
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
        FOXY_FATAL << "Validation layers requested, but none are available.";
      }
      extension_data_.enabled_extensions = get_enabled_extensions();
      if (!extension_data_.enabled_extensions.empty()) {
        instance_create_info.enabledExtensionCount = static_cast<u32>(extension_data_.enabled_extensions.size());
        instance_create_info.ppEnabledExtensionNames = extension_data_.enabled_extensions.data();
      }

      auto debug_create_info = vk::DebugUtilsMessengerCreateInfoEXT{
        .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
        .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
        .pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)(vulkan_error_callback),
      };
      if (enable_validation_) {
        instance_create_info.enabledLayerCount = static_cast<u32>(validation_layer_names_.size());
        instance_create_info.ppEnabledLayerNames = validation_layer_names_.data();
        instance_create_info.pNext = reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debug_create_info);
      } else {
        instance_create_info.enabledLayerCount = 0;
      }

      try {
        return { context_, instance_create_info };
      } catch (const std::exception& e) {
        FOXY_FATAL << e.what();
        return nullptr;
      }
    }

    [[nodiscard]] auto find_queue_families(const PhysicalDevice& physical_device) -> QueueFamilyIndices {
      auto queue_family_indices = QueueFamilyIndices{};
      auto queue_family_properties = physical_device.getQueueFamilyProperties();

      u32 i{ 0 };
      for (const auto& queue_family: queue_family_properties) {
        if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics) {
          queue_family_indices.graphics = i;
        }

        vk::Bool32 surface_support = physical_device.getSurfaceSupportKHR(i, *surface_);
        if (surface_support) {
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
        [&](vk::ExtensionProperties& properties) {
          return properties.extensionName;
        });

      // make sure none of the required extensions are missing from the available extensions
      return !std::ranges::any_of(required_instance_extensions, [&](const std::string& extension) {
        return !available_extensions.contains(extension);
      });
    }

    [[nodiscard]] auto query_swapchain_support(const PhysicalDevice& device) -> SwapchainSupportInfo {
      SwapchainSupportInfo info{
        .capabilities = device.getSurfaceCapabilitiesKHR(*surface_),
        .formats = device.getSurfaceFormatsKHR(*surface_),
        .present_modes = device.getSurfacePresentModesKHR(*surface_),
      };

      return info;
    }

    [[nodiscard]] auto device_suitable(const PhysicalDevice& physical_device) -> bool {
      auto queue_family_indices{ find_queue_families(physical_device) };

      bool valid_swapchain{ false };
      if (check_device_extension_support(physical_device)) {
        auto swapchain_support_info{ query_swapchain_support(physical_device) };
        valid_swapchain = !swapchain_support_info.formats.empty() && !swapchain_support_info.present_modes.empty();
      }

      return queue_family_indices.complete() && valid_swapchain;
    }

    [[nodiscard]] auto pick_physical_device() -> PhysicalDevice {
      // TODO: Rank suitability and pick highest scoring device
      vk::raii::PhysicalDevices physical_devices{ instance_ };
      FOXY_ASSERT(!physical_devices.empty()) << "No physical Vulkan devices found.";
      vk::raii::PhysicalDevice physical_device{ nullptr };
      for (const auto& device: physical_devices) {
        if (device_suitable(device)) {
          physical_device = vk::raii::PhysicalDevice{ device }; // this is a *move* (devices are not copyable)
          break;
        }
      }
      FOXY_ASSERT(!physical_devices.empty()) << "No *viable* physical Vulkan devices found.";

      vk::PhysicalDeviceProperties device_properties{ physical_device.getProperties() };
      std::string device_name{ static_cast<const char*>(device_properties.deviceName) };
      Version api_version{ device_properties.apiVersion };
      Version driver_version{ device_properties.driverVersion };

      FOXY_INFO << "Vulkan Device: " << device_name << " | Device driver version: " << driver_version.to_string()
                << " | Vulkan API version: " << api_version.to_string();

      return physical_device;
    }

    [[nodiscard]] auto create_logical_device() -> LogicalDevice {
      std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
      std::set<u32> queue_families{
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

      vk::PhysicalDeviceFeatures device_features{};

      vk::DeviceCreateInfo device_create_info{
        .queueCreateInfoCount = static_cast<u32>(queue_create_infos.size()),
        .pQueueCreateInfos = queue_create_infos.data(),
        .enabledExtensionCount = static_cast<u32>(extension_data_.device_extensions.size()),
        .ppEnabledExtensionNames = extension_data_.device_extensions.data(),
      };

      return { physical_device_.createDevice(device_create_info, nullptr) };
    }

    [[nodiscard]] auto create_surface(glfw::UniqueWindow& window) -> Surface {
      VkSurfaceKHR raw_surface;

      auto result = static_cast<vk::Result>(
        glfwCreateWindowSurface(*instance_, window.get(), nullptr, &raw_surface)
      );

      return vk::raii::SurfaceKHR{ instance_, vk::createResultValueType(result, raw_surface) };
    }
  };

  //
  //  Context
  //

  Context::Context(glfw::UniqueWindow& window, bool enable_validation)
    : pImpl_{std::make_unique<Impl>(window, enable_validation)} {}

  Context::~Context() = default;

  auto Context::operator*() -> VulkanContext& {
    return pImpl_->native();
  }

  auto Context::window() -> glfw::UniqueWindow& {
    return pImpl_->window();
  }

  auto Context::native() -> vk::raii::Context& {
    return pImpl_->native();
  }

  auto Context::instance() -> vk::raii::Instance& {
    return pImpl_->instance();
  }

  auto Context::surface() -> Surface& {
    return pImpl_->surface();
  }

  auto Context::query_swapchain_support() -> SwapchainSupportInfo {
    return pImpl_->query_swapchain_support();
  }

  auto Context::queue_families() -> const QueueFamilyIndices& {
    return pImpl_->queue_families();
  }

  auto Context::physical_device() -> vk::raii::PhysicalDevice& {
    return pImpl_->physical_device();
  }

  auto Context::logical_device() -> vk::raii::Device& {
    return pImpl_->logical_device();
  }
}