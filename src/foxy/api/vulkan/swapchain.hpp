#pragma once

namespace vk::raii {
  class SurfaceKHR;
  class SwapchainKHR;
}

namespace foxy::vulkan {
  class Context;
}

namespace foxy {
  class Swapchain {
    using Surface = vk::raii::SurfaceKHR;
    using NativeSwapchain = vk::raii::SwapchainKHR;
  public:
    explicit Swapchain(Shared<vulkan::Context> context);

    ~Swapchain();
  private:
    Shared<vulkan::Context> context_;
    Unique<Surface> surface_;
    Shared<NativeSwapchain> swapchain_;
  };
}