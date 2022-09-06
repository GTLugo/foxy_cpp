#pragma once

#include "foxy/api/vulkan/context.hpp"

namespace foxy::vulkan {
  class Context;
}

namespace foxy {
  class Swapchain {
    using NativeSwapchain = vk::raii::SwapchainKHR;
    using Extent = vk::Extent2D;
  public:
    struct Image {
      Unique<vk::raii::Image> image;
      Unique<vk::raii::ImageView> view;
      Unique<vk::raii::Fence> fence;
    };

    Swapchain(Shared<vulkan::Context> context, Rect window_bounds);

    ~Swapchain();
  private:
    Shared<vulkan::Context> context_;
    Shared<NativeSwapchain> swapchain_;
    Unique<Extent> extent_;
    u32 image_count_{ 0 };
    u32 current_image_{ 0 };
    std::vector<Image> images_;
    Unique<vk::PresentInfoKHR> present_info_;
    Unique<vk::RenderPass> render_pass_;
  };
}