#pragma once

#include "foxy/api/vulkan/context.hpp"
#include "foxy/api/glfw/unique_window.hpp"

namespace foxy::vulkan {
  class Context;

  class Swapchain {
    using NativeSwapchain = vk::raii::SwapchainKHR;
    using Extent = vk::Extent2D;
  public:
    struct Image {
      Unique<vk::raii::Image> image;
      Unique<vk::raii::ImageView> view;
      Unique<vk::raii::Fence> fence;
    };

    Swapchain(Shared<vulkan::Context> context);

    ~Swapchain();
  private:
    Shared<vulkan::Context> context_;
    Shared<NativeSwapchain> swapchain_;

    vk::SurfaceFormatKHR surface_format_;
    vk::PresentModeKHR present_mode_;
    vk::Extent2D extent_;

    u32 image_count_{ 0 };

    [[nodiscard]] auto pick_swap_surface_format(const std::vector<vk::SurfaceFormatKHR>& formats) -> vk::SurfaceFormatKHR;
    [[nodiscard]] auto pick_swap_present_mode(const std::vector<vk::PresentModeKHR>& modes) -> vk::PresentModeKHR;
    [[nodiscard]] auto pick_swap_extent(const vk::SurfaceCapabilitiesKHR& capabilities) -> vk::Extent2D;
  };
}