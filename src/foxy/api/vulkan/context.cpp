#include "context.hpp"

#include "foxy/api/vulkan/version.hpp"
#define FOXY_GLFW_INCLUDE_VULKAN
#include "foxy/api/glfw/glfw.hpp"

namespace foxy::vulkan {
  inline static VKAPI_ATTR auto vulkan_error_callback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                                      vk::DebugUtilsMessageTypeFlagBitsEXT type,
                                                      const vk::DebugUtilsMessengerCallbackDataEXT* callback_data,
                                                      void*) -> vk::Bool32 {
    std::stringstream msg{};
    switch (type) {
      case vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral:
        msg << "GENERAL";
        break;
      case vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation:
        msg << "VALIDATION";
        break;
      case vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance:
        msg << "PERFORMANCE";
        break;
    }

    msg << " | code " << callback_data->messageIdNumber << ", " << callback_data->pMessageIdName << ": " << callback_data->pMessage;
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

  Context::Context(glfw::UniqueWindow& window)
    : context_{std::make_unique<vk::raii::Context>()},
      instance_{nullptr} {
    create_instance();
    setup_debug_messenger();
    find_devices();
    create_surface(window);
  }

  Context::~Context() = default;

  auto Context::instance() -> Unique<vk::raii::Instance>& {
    return instance_;
  }

  auto Context::physical_device() -> Unique <vk::raii::PhysicalDevice>& {
    return physical_device_;
  }

  auto Context::logical_device() -> Unique<vk::raii::Device>& {
    return logical_device_;
  }

  auto Context::surface() -> Unique <vk::raii::SurfaceKHR>& {
    return surface_;
  }

  auto Context::native() -> Unique<vk::raii::Context>& {
    return context_;
  }

  void Context::create_instance() {
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
    enabled_extensions_ = get_enabled_extensions();
    if (!enabled_extensions_.empty()) {
      instance_create_info.enabledExtensionCount = static_cast<u32>(enabled_extensions_.size());
      instance_create_info.ppEnabledExtensionNames = enabled_extensions_.data();
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
      instance_ = std::make_unique<vk::raii::Instance>(*context_, instance_create_info);
    } catch (const std::exception& e) {
      FOXY_FATAL << e.what();
    }
  }

  void Context::find_devices() {
    vk::raii::PhysicalDevices physical_devices_{*instance_};
    FOXY_ASSERT(!physical_devices_.empty()) << "NO PHYSICAL VULKAN DEVICES FOUND";
    // this is a *move* (devices are not copyable)
    physical_device_ = std::make_unique<vk::raii::PhysicalDevice>(physical_devices_[0]);
    auto device_properties = std::make_unique<vk::PhysicalDeviceProperties>(physical_device_->getProperties());
    std::string device_name = device_properties->deviceName;
    auto api_version = std::make_unique<Version>(device_properties->apiVersion);
    auto driver_version = std::make_unique<Version>(device_properties->driverVersion);

    //vulkan::DeviceCreateInfo device_create_info2{};

    vk::DeviceCreateInfo device_create_info{};
    logical_device_ = std::make_unique<vk::raii::Device>(physical_device_->createDevice(device_create_info, nullptr));

    FOXY_INFO << "Vulkan Device: " << device_name << " | Device driver version: " << driver_version->to_string()
        << " | Vulkan API version: " << api_version->to_string();
  }

  void Context::create_surface(glfw::UniqueWindow& window) {
    VkSurfaceKHR raw_surface;
    auto result = static_cast<vk::Result>(
        glfwCreateWindowSurface(static_cast<VkInstance>(**instance_), window.get(), nullptr, &raw_surface)
    );
    surface_ = std::make_unique<vk::raii::SurfaceKHR>(*instance_, vk::createResultValueType(result, raw_surface));

    auto surface_formats = physical_device_->getSurfaceFormatsKHR(**surface_);
    if (surface_formats.size() == 1 && surface_formats[0].format == vk::Format::eUndefined) {
      color_format_ = std::make_unique<vk::Format>(vk::Format::eB8G8R8A8Unorm);
    } else {
      color_format_ = std::make_unique<vk::Format>(surface_formats[0].format);
    }
    color_space_ = std::make_unique<vk::ColorSpaceKHR>(surface_formats[0].colorSpace);
    queue_mode_index_ = find_queue(vk::QueueFlagBits::eGraphics);
  }

  auto Context::find_queue(const vk::QueueFlags& flags) const -> u32 {
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

  auto Context::get_enabled_extensions() -> std::vector<const char*> {
    std::set<const char*> required_instance_extensions{};
    // GLFW
    glfw_extensions_ = glfw::required_instance_extensions();
    for (const auto& extension: glfw_extensions_) {
      if (required_instance_extensions.count(extension) == 0) {
        required_instance_extensions.insert(extension);
      }
    }
    // Vulkan
    auto extension_count = u32{ 0 };
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    instance_extensions_ = std::vector<VkExtensionProperties>{ extension_count };
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, instance_extensions_.data());
    for (const auto& extension: instance_extensions_) {
      if (required_instance_extensions.count(extension.extensionName) == 0) {
        required_instance_extensions.insert(extension.extensionName);
      }
    }

    if (enable_validation_ && required_instance_extensions.count(VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0) {
      required_instance_extensions.insert(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

    FOXY_DEBUG << "Vulkan Extension Count: " << required_instance_extensions.size();
    std::stringstream exts;
    for (const auto& extension: required_instance_extensions) {
      exts << extension << " | ";
    }
    FOXY_DEBUG << "Vulkan Extensions: " << exts.str();

    return {required_instance_extensions.begin(), required_instance_extensions.end()};
  }

  auto Context::check_validation_layer_support() -> bool {
    auto layer_count = u32{ 0 };
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    auto layer_properties = std::vector<VkLayerProperties>{ layer_count };
    vkEnumerateInstanceLayerProperties(&layer_count, layer_properties.data());

    FOXY_DEBUG << "Vulkan Layer Count: " << layer_properties.size();
    std::stringstream lyrs;
    for (const auto& layer: layer_properties) {
      lyrs << layer.layerName << " | ";
    }
    FOXY_DEBUG << "Vulkan Layers: " << lyrs.str();

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

  void Context::setup_debug_messenger() {
    if (!enable_validation_) {
      return;
    }

    auto callback_create_info = vk::DebugUtilsMessengerCreateInfoEXT{
      .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
      .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
      .pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)(vulkan_error_callback),
    };

    try {
      debug_messenger_ = std::make_unique<vk::raii::DebugUtilsMessengerEXT>(*instance_, callback_create_info);
    } catch (const std::exception& e) {
      FOXY_FATAL << "Failed to set up debug messenger.";
    }
  }
}