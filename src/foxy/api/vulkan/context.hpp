#pragma once

#include "foxy/api/glfw/unique_window.hpp"
#include "foxy/api/vulkan/vulkan.hpp"

namespace foxy {
  class Version;
}

namespace foxy::vulkan {
  class Context {
    using VulkanContext = vk::raii::Context;
    using Instance = vk::raii::Instance;
    using PhysicalDevice = vk::raii::PhysicalDevice;
    using LogicalDevice = vk::raii::Device;
    using Surface = vk::raii::SurfaceKHR;
    using Format = vk::Format;
    using ColorSpace = vk::ColorSpaceKHR;

    using DeviceProperties = vk::PhysicalDeviceProperties;
    using DeviceFeatures = vk::PhysicalDeviceFeatures;
    using DeviceMemoryProperties = vk::PhysicalDeviceMemoryProperties;
    using QueueFlags = vk::Flags<vk::QueueFlagBits>;

  public:
    Context(glfw::UniqueWindow& window);
    ~Context();

    FN instance() -> Unique<Instance>&;
    FN physical_device() -> Unique<PhysicalDevice>&;
    FN logical_device() -> Unique<LogicalDevice>&;
    FN surface() -> Unique<Surface>&;
    FN native() -> Unique<VulkanContext>&;

    FN best_queue(const QueueFlags& flags) const -> u32;

  private:
    Unique<VulkanContext> context_;
    Unique<Instance> instance_;
    bool enable_validation_{
    #ifdef FOXY_ENABLE_VALIDATION_LAYERS
      true
    #else
      false
    #endif
    };
    static inline std::list<std::string> validation_layer_names_ = {
      // This is a meta layer that enables all of the standard
      // validation layers in the correct order :
      // threading, parameter_validation, device_limits, object_tracker, image, core_validation, swapchain, and unique_objects
      "VK_LAYER_LUNARG_assistant_layer", "VK_LAYER_LUNARG_standard_validation"
    };

    Unique<PhysicalDevice> physical_device_;
    Unique<LogicalDevice> logical_device_;
    std::vector<vk::QueueFamilyProperties> queue_family_properties_;

    Unique<Surface> surface_;
    Unique<Format> color_format_;
    Unique<ColorSpace> color_space_;
    u32 queue_mode_index_{ UINT32_MAX };

    auto create_instance() -> Unique<Instance>;
    auto find_devices() -> std::pair<Unique<PhysicalDevice>, Unique<LogicalDevice>>;
    auto create_surface(glfw::UniqueWindow& window) -> Unique<Surface>;

    static FN get_available_layers() -> std::set<std::string>;
    static FN filter_layers(const std::list<std::string>& desiredLayers) -> std::vector<const char*>;
  };
}