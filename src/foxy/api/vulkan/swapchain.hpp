#pragma once

namespace vk::raii {
  class SwapchainKHR;
}

namespace foxy::vulkan {
  class Context;
}

namespace foxy {
  class Swapchain {
    using NativeSwapchain = vk::raii::SwapchainKHR;
  public:
    explicit Swapchain(Shared<vulkan::Context> context);

    ~Swapchain();
  private:
    Shared<vulkan::Context> context_;
    Shared<NativeSwapchain> swapchain_;
  };
}