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
    //using Surface = vk::raii::SurfaceKHR;

    using DeviceProperties = vk::PhysicalDeviceProperties;
    using DeviceFeatures = vk::PhysicalDeviceFeatures;
    using DeviceMemoryProperties = vk::PhysicalDeviceMemoryProperties;
    using QueueFlags = vk::Flags<vk::QueueFlagBits>;
    using DevicePair = std::pair<Unique<PhysicalDevice>, Unique<LogicalDevice>>;
    using DebugMessenger = vk::raii::DebugUtilsMessengerEXT;

  public:
    Context(glfw::UniqueWindow& window);
    ~Context();

    [[nodiscard]] auto instance() -> Unique<Instance>&;
    [[nodiscard]] auto physical_device() -> Unique<PhysicalDevice>&;
    [[nodiscard]] auto logical_device() -> Unique<LogicalDevice>&;
    [[nodiscard]] auto surface() -> Unique<Surface>&;
    [[nodiscard]] auto native() -> Unique<VulkanContext>&;

    [[nodiscard]] auto find_queue(const QueueFlags& flags) const -> u32;

  private:
    static inline const bool enable_validation_{
        #ifdef NDEBUG
        false
        #else
        true
        #endif
    };
    static inline const std::vector<const char*> validation_layer_names_ = {
        "VK_LAYER_KHRONOS_validation"
    };

    Unique<VulkanContext> context_;
    ExtensionData extension_data_{};
    Unique<Instance> instance_;
    Unique<DebugMessenger> debug_messenger_;

    Unique<PhysicalDevice> physical_device_;
    Unique<LogicalDevice> logical_device_;
    std::vector<vk::QueueFamilyProperties> queue_family_properties_;

    Unique<Surface> surface_;
    u32 queue_mode_index_{ UINT32_MAX };

    [[nodiscard]] auto create_instance() -> Unique<Instance>;
    [[nodiscard]] auto pick_physical_device() -> Unique<PhysicalDevice>;
    [[nodiscard]] auto create_logical_device() -> Unique<LogicalDevice>;
    [[nodiscard]] auto create_surface(glfw::UniqueWindow& window) -> Unique<Surface>;
    [[nodiscard]] auto try_create_debug_messenger() -> Unique<DebugMessenger>;

    auto get_enabled_extensions() -> std::vector<const char*>;
    auto check_validation_layer_support() -> bool;
  };
}