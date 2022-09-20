#pragma once

class GLFWwindow;

namespace ookami {
  class SwapchainSupportInfo;
  class QueueFamilyIndices;

  auto required_instance_extensions_strings() -> std::vector<std::string>;
  auto required_instance_extensions() -> std::vector<const char*>;
}

namespace vk::raii {
  class Context;
  class Instance;
  class PhysicalDevice;
  class Device;
  class SurfaceKHR;
  class PhysicalDevice;
}



namespace ookami {
  class Context {
    using VulkanContext = vk::raii::Context;
    using Instance = vk::raii::Instance;
    using PhysicalDevice = vk::raii::PhysicalDevice;
    using LogicalDevice = vk::raii::Device;
    using Surface = vk::raii::SurfaceKHR;

  public:
    explicit Context(koyote::shared<GLFWwindow> window, bool enable_validation = true);
    ~Context();

    auto operator*() -> VulkanContext&;
    
    [[nodiscard]] auto native() -> VulkanContext&;
    [[nodiscard]] auto instance() -> Instance&;
    [[nodiscard]] auto surface() -> Surface&;
    [[nodiscard]] auto query_swapchain_support() -> SwapchainSupportInfo;
    [[nodiscard]] auto queue_families() -> const QueueFamilyIndices&;
    [[nodiscard]] auto physical_device() -> PhysicalDevice&;
    [[nodiscard]] auto logical_device() -> LogicalDevice&;

  private:
    PIMPL(Impl) pImpl_;
  };
}