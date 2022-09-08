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

    using DeviceProperties = vk::PhysicalDeviceProperties;
    using DeviceFeatures = vk::PhysicalDeviceFeatures;
    using DeviceMemoryProperties = vk::PhysicalDeviceMemoryProperties;
    using QueueFlags = vk::Flags<vk::QueueFlagBits>;
    using DevicePair = std::pair<Unique<PhysicalDevice>, Unique<LogicalDevice>>;
    using DebugMessenger = vk::raii::DebugUtilsMessengerEXT;

  public:
    Context(glfw::UniqueWindow& window, bool enable_validation = true);
    ~Context();

    auto operator*() -> Unique<VulkanContext>&;

    [[nodiscard]] auto window() -> glfw::UniqueWindow&;
    [[nodiscard]] auto native() -> Unique<VulkanContext>&;
    [[nodiscard]] auto instance() -> Unique<Instance>&;
    [[nodiscard]] auto surface() -> Unique<Surface>&;
    [[nodiscard]] auto query_swapchain_support() -> SwapchainSupportInfo;
    [[nodiscard]] auto queue_families() -> const QueueFamilyIndices&;
    [[nodiscard]] auto physical_device() -> Unique<PhysicalDevice>&;
    [[nodiscard]] auto logical_device() -> Unique<LogicalDevice>&;

  private:
    static inline const std::vector<const char*> validation_layer_names_ = {
        "VK_LAYER_KHRONOS_validation"
    };

    const bool enable_validation_;
    glfw::UniqueWindow& window_;

    Unique<VulkanContext> context_;
    ExtensionData extension_data_{
      .device_extensions = std::vector<const char*>{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
      }
    };
    Unique<Instance> instance_;
    Unique<DebugMessenger> debug_messenger_;

    Unique<Surface> surface_;

    Unique<PhysicalDevice> physical_device_;
    QueueFamilyIndices queue_family_indices_;
    Unique<LogicalDevice> logical_device_;

    Unique<vk::raii::Queue> graphics_queue_;
    Unique<vk::raii::Queue> present_queue_;


    [[nodiscard]] static auto check_validation_layer_support() -> bool;

    [[nodiscard]] auto try_create_debug_messenger() -> Unique<DebugMessenger>;
    [[nodiscard]] auto get_enabled_extensions() -> std::vector<const char*>;
    [[nodiscard]] auto create_instance() -> Unique<Instance>;
    [[nodiscard]] auto find_queue_families(const PhysicalDevice& physical_device) -> QueueFamilyIndices;
    [[nodiscard]] auto check_device_extension_support(const PhysicalDevice& device) -> bool;
    [[nodiscard]] auto query_swapchain_support(const PhysicalDevice& device) -> SwapchainSupportInfo;
    [[nodiscard]] auto device_suitable(const PhysicalDevice& physical_device) -> bool;
    [[nodiscard]] auto pick_physical_device() -> Unique<PhysicalDevice>;
    [[nodiscard]] auto create_logical_device() -> Unique<LogicalDevice>;
    [[nodiscard]] auto create_surface(glfw::UniqueWindow& window) -> Unique<Surface>;
  };
}