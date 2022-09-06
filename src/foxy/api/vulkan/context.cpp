#include "context.hpp"

#include "foxy/api/vulkan/version.hpp"
#define FOXY_GLFW_INCLUDE_VULKAN
#include "foxy/api/glfw/glfw.hpp"

namespace foxy::vulkan {
  inline static FN vulkan_error_callback(VkDebugReportFlagsEXT flags,
                                         VkDebugReportObjectTypeEXT objType,
                                         uint64_t srcObject,
                                         size_t location,
                                         int32_t msgCode,
                                         const char* pLayerPrefix,
                                         const char* pMsg,
                                         void* pUserData) -> VkBool32 {
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
      FOXY_ERROR << pLayerPrefix << " | code " << msgCode << ": " << pMsg;
    } else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
      FOXY_WARN << pLayerPrefix << " | code " << msgCode << ": " << pMsg;
    } else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
      FOXY_WARN << pLayerPrefix << " | code " << msgCode << ": " << pMsg;
    } else {
      return false;
    }
    return true;
  }

  Context::Context(glfw::UniqueWindow& window)
    : physical_device_{},
      logical_device_{} {
    context_ = std::make_unique<vk::raii::Context>();
    instance_ = create_instance();
    std::tie(physical_device_, logical_device_) = find_devices();
    surface_ = create_surface(window);
  }

  Context::~Context() = default;

  FN Context::instance() -> Unique<vk::raii::Instance>& {
    return instance_;
  }

  FN Context::physical_device() -> Unique <vk::raii::PhysicalDevice>& {
    return physical_device_;
  }

  FN Context::logical_device() -> Unique<vk::raii::Device>& {
    return logical_device_;
  }

  FN Context::surface() -> Unique <vk::raii::SurfaceKHR>& {
    return surface_;
  }

  FN Context::native() -> Unique<vk::raii::Context>& {
    return context_;
  }

  auto Context::create_instance() -> Unique<vk::raii::Instance> {
    vk::ApplicationInfo app_info{
        .pApplicationName = "FOXY APP",
        .pEngineName = "FOXY FRAMEWORK",
        .apiVersion = VK_API_VERSION_1_3
    };

    // Extensions
    std::set<std::string> required_extensions{};
    auto glfw_extensions = glfw::required_instance_extensions();
    required_extensions.insert(glfw_extensions.begin(), glfw_extensions.end());
    if (enable_validation_) {
      required_extensions.insert(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }
    std::set<std::string> instance_extensions{};
    std::list<std::function<std::set<std::string>()>> extension_pickers{};
    instance_extensions.insert(required_extensions.begin(), required_extensions.end());
    for (const auto& picker : extension_pickers) {
      auto extensions = picker();
      instance_extensions.insert(extensions.begin(), extensions.end());
    }
    std::vector<const char*> enabled_extensions;
    for (const auto& extension : instance_extensions) {
      enabled_extensions.push_back(extension.c_str());
    }

    vk::InstanceCreateInfo instance_create_info{
        .pApplicationInfo = &app_info
    };
    if (!enabled_extensions.empty()) {
      instance_create_info.enabledExtensionCount = static_cast<u32>(enabled_extensions.size());
      instance_create_info.ppEnabledExtensionNames = enabled_extensions.data();
    }

    std::vector<const char*> layers;
    if (enable_validation_) {
      layers = filter_layers(validation_layer_names_);
      instance_create_info.enabledLayerCount = static_cast<u32>(layers.size());
      instance_create_info.ppEnabledLayerNames = layers.data();
    }

    auto instance = std::make_unique<vk::raii::Instance>(*context_, instance_create_info);

    if (enable_validation_) {
      auto report_callback_create_info = vk::DebugReportCallbackCreateInfoEXT{
          .flags = vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning,
          .pfnCallback = (PFN_vkDebugReportCallbackEXT)(vulkan_error_callback),
      };
    }

    return std::move(instance);
  }

  FN Context::find_devices() -> std::pair<Unique<PhysicalDevice>, Unique<LogicalDevice>> {
    vk::raii::PhysicalDevices physical_devices_{*instance_};
    FOXY_ASSERT(!physical_devices_.empty()) << "NO PHYSICAL VULKAN DEVICES FOUND";
    // this is a *move* (devices are not copyable)
    auto physical_device = std::make_unique<vk::raii::PhysicalDevice>(physical_devices_[0]);
    auto device_properties = std::make_unique<vk::PhysicalDeviceProperties>(physical_device->getProperties());
    std::string device_name = device_properties->deviceName;
    auto api_version = std::make_unique<Version>(device_properties->apiVersion);
    auto driver_version = std::make_unique<Version>(device_properties->driverVersion);

    vk::DeviceCreateInfo device_create_info{};
    auto logical_device = std::make_unique<vk::raii::Device>(physical_device->createDevice(device_create_info, nullptr));

    FOXY_INFO << "Vulkan Device: " << device_name << " | Device driver version: " << driver_version->to_string()
        << " | Vulkan API version: " << api_version->to_string();

    return std::make_pair(std::move(physical_device), std::move(logical_device));
  }

  FN Context::create_surface(glfw::UniqueWindow& window) -> Unique<vk::raii::SurfaceKHR> {
    VkSurfaceKHR raw_surface;
    auto result = static_cast<vk::Result>(
        glfwCreateWindowSurface(static_cast<VkInstance>(**instance_), window.get(), nullptr, &raw_surface)
    );
    auto vk_surface = vk::createResultValueType(result, raw_surface);
    auto unique_surface = std::make_unique<vk::raii::SurfaceKHR>(*instance_, vk_surface);

    auto surface_formats = physical_device_->getSurfaceFormatsKHR(**unique_surface);
    if (surface_formats.size() == 1 && surface_formats[0].format == vk::Format::eUndefined) {
      color_format_ = std::make_unique<vk::Format>(vk::Format::eB8G8R8A8Unorm);
    } else {
      color_format_ = std::make_unique<vk::Format>(surface_formats[0].format);
    }
    color_space_ = std::make_unique<vk::ColorSpaceKHR>(surface_formats[0].colorSpace);
    queue_mode_index_ = best_queue(vk::QueueFlagBits::eGraphics);

    return std::move(unique_surface);
  }

  FN Context::best_queue(const vk::QueueFlags& flags) const -> u32 {
    auto best_match = u32{ VK_QUEUE_FAMILY_IGNORED };
    auto best_match_extra_flags = vk::QueueFlags{ VK_QUEUE_FLAG_BITS_MAX_ENUM };
    auto queue_count = size_t{ queue_family_properties_.size() };
    for (u32 i{ 0 }; i < queue_count; ++i) {
      auto current_flags = queue_family_properties_[i].queueFlags;

      if (!(current_flags & flags)) {
        // Doesn't contain the required flags, skip it
        continue;
      }

      if (surface_ && !physical_device_->getSurfaceSupportKHR(i, **surface_)) {
        // Doesn't support the current surface, skip it
        continue;
      }
      auto current_extra_flags = (current_flags & ~flags);

      if (vk::QueueFlags{ 0 } == current_extra_flags) {
        // exact match; no extra flags
        return i;
      }

      if (best_match == VK_QUEUE_FAMILY_IGNORED || current_extra_flags < best_match_extra_flags) {
        best_match = i;
        best_match_extra_flags = current_extra_flags;
      }
    }
    return best_match;
  }

  auto Context::get_available_layers() -> std::set<std::string> {
    std::set<std::string> result;
    auto layers = vk::enumerateInstanceLayerProperties();
    for (auto layer : layers) {
      result.insert(layer.layerName);
    }
    return result;
  }

  auto Context::filter_layers(const std::list<std::string>& desiredLayers) -> std::vector<const char*> {
    static std::set<std::string> validLayerNames = get_available_layers();
    std::vector<const char*> result;
    for (const auto& string : desiredLayers) {
      if (validLayerNames.count(string) != 0) {
        result.push_back(string.c_str());
      }
    }
    return result;
  }
}