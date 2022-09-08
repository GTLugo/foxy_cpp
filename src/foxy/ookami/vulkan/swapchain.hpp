#pragma once

#include "context.hpp"
#include "foxy/core/window/glfw/unique_window.hpp"

namespace foxy::vulkan {
  class Context;

  class Swapchain {
  public:
    Swapchain(Shared<vulkan::Context> context);

    ~Swapchain();
  private:
    Shared<vulkan::Context> context_;
    Shared<vk::raii::SwapchainKHR> swapchain_;

    vk::Format swapchain_image_format_;
    vk::Extent2D swapchain_extent_;
    std::vector<VkImage> swap_images_;
    std::vector<vk::raii::ImageView> swap_image_views_;

    [[nodiscard]] auto pick_swap_surface_format(const std::vector<vk::SurfaceFormatKHR>& formats) -> vk::SurfaceFormatKHR;
    [[nodiscard]] auto pick_swap_present_mode(const std::vector<vk::PresentModeKHR>& modes) -> vk::PresentModeKHR;
    [[nodiscard]] auto pick_swap_extent(const vk::SurfaceCapabilitiesKHR& capabilities) -> vk::Extent2D;
    [[nodiscard]] auto create_swapchain() -> Shared<vk::raii::SwapchainKHR>;
    [[nodiscard]] auto create_image_views() -> std::vector<vk::raii::ImageView>;
  };
}