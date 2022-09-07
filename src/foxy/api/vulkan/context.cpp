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
        msg << "[GENR] ";
        break;
      case vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation:
        msg << "[VALD] ";
        break;
      case vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance:
        msg << "[PERF] ";
        break;
    }

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

  Context::Context(glfw::UniqueWindow& window)
    : context_{std::make_unique<vk::raii::Context>()},
      instance_{create_instance()},
      debug_messenger_{try_create_debug_messenger()},
      physical_device_{pick_physical_device()},
      logical_device_{create_logical_device()},
      surface_{create_surface(window)} {
    queue_mode_index_ = find_queue(vk::QueueFlagBits::eGraphics);
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

  auto Context::surface() -> Unique<Surface>& {
    return surface_;
  }

  auto Context::native() -> Unique<vk::raii::Context>& {
    return context_;
  }

  auto Context::create_instance() -> Unique<Instance> {
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
      return std::make_unique<vk::raii::Instance>(*context_, instance_create_info);
    } catch (const std::exception& e) {
      FOXY_FATAL << e.what();
      return nullptr;
    }
  }

  auto Context::pick_physical_device() -> Unique<PhysicalDevice> {
    vk::raii::PhysicalDevices physical_devices{*instance_};
    FOXY_ASSERT(!physical_devices.empty()) << "NO PHYSICAL VULKAN DEVICES FOUND";
    // this is a *move* (devices are not copyable)
    auto physical_device = std::make_unique<vk::raii::PhysicalDevice>(physical_devices[0]);
    auto device_properties = std::make_unique<vk::PhysicalDeviceProperties>(physical_device->getProperties());
    std::string device_name = device_properties->deviceName;
    auto api_version = std::make_unique<Version>(device_properties->apiVersion);
    auto driver_version = std::make_unique<Version>(device_properties->driverVersion);

    FOXY_INFO << "Vulkan Device: " << device_name << " | Device driver version: " << driver_version->to_string()
              << " | Vulkan API version: " << api_version->to_string();

    return physical_device;
  }

  auto Context::create_logical_device() -> Unique<LogicalDevice> {
    vk::DeviceCreateInfo device_create_info{};
    return std::make_unique<vk::raii::Device>(physical_device_->createDevice(device_create_info, nullptr));
  }

  auto Context::create_surface(glfw::UniqueWindow& window) -> Unique<Surface> {
    VkSurfaceKHR raw_surface;
    auto result = static_cast<vk::Result>(
        glfwCreateWindowSurface(static_cast<VkInstance>(**instance_), window.get(), nullptr, &raw_surface)
    );
    auto surface = std::make_unique<Surface>();

    surface->native = std::make_unique<vk::raii::SurfaceKHR>(*instance_, vk::createResultValueType(result, raw_surface));

    auto surface_formats = physical_device_->getSurfaceFormatsKHR(**surface_->native);
    if (surface_formats.size() == 1 && surface_formats[0].format == vk::Format::eUndefined) {
      surface->color_format = std::make_unique<vk::Format>(vk::Format::eB8G8R8A8Unorm);
    } else {
      surface->color_format = std::make_unique<vk::Format>(surface_formats[0].format);
    }
    surface->color_space = std::make_unique<vk::ColorSpaceKHR>(surface_formats[0].colorSpace);

    return surface;
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

      if (surface_ && !physical_device_->getSurfaceSupportKHR(i, **surface_->native)) {
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
    extension_data_.window_extensions = glfw::required_instance_extensions();
    for (const auto& extension: extension_data_.window_extensions) {
      if (required_instance_extensions.count(extension) == 0) {
        required_instance_extensions.insert(extension);
      }
    }
    // Vulkan
    auto extension_count = u32{ 0 };
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    extension_data_.instance_extensions = std::vector<VkExtensionProperties>{ extension_count };
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_data_.instance_extensions.data());
    for (const auto& extension: extension_data_.instance_extensions) {
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

  auto Context::try_create_debug_messenger() -> Unique<DebugMessenger> {
    if (!enable_validation_) {
      return nullptr;
    }

    auto callback_create_info = vk::DebugUtilsMessengerCreateInfoEXT{
      .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
      .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
      .pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)(vulkan_error_callback),
    };

    try {
      return std::make_unique<DebugMessenger>(*instance_, callback_create_info);
    } catch (const std::exception& e) {
      FOXY_FATAL << "Failed to set up debug messenger.";
      return nullptr;
    }
  }
}