#pragma once

class GLFWwindow;

namespace foxy::vulkan {
  class SwapchainSupportInfo;
  class QueueFamilyIndices;
}

namespace vk::raii {
  class Context;
  class Instance;
  class PhysicalDevice;
  class Device;
  class SurfaceKHR;
  class PhysicalDevice;
}

namespace foxy::vulkan {
  class Context {
    using VulkanContext = vk::raii::Context;
    using Instance = vk::raii::Instance;
    using PhysicalDevice = vk::raii::PhysicalDevice;
    using LogicalDevice = vk::raii::Device;
    using Surface = vk::raii::SurfaceKHR;
    using UniqueWindow = Unique<GLFWwindow, void(*)(GLFWwindow*)>;

  public:
    explicit Context(UniqueWindow& window, bool enable_validation = true);
    ~Context();

    auto operator*() -> VulkanContext&;

    [[nodiscard]] auto window() -> UniqueWindow&;
    [[nodiscard]] auto native() -> VulkanContext&;
    [[nodiscard]] auto instance() -> Instance&;
    [[nodiscard]] auto surface() -> Surface&;
    [[nodiscard]] auto query_swapchain_support() -> SwapchainSupportInfo;
    [[nodiscard]] auto queue_families() -> const QueueFamilyIndices&;
    [[nodiscard]] auto physical_device() -> PhysicalDevice&;
    [[nodiscard]] auto logical_device() -> LogicalDevice&;

  private:
    class Impl;
    Unique<Impl> pImpl_;
  };
}