#include "swapchain.hpp"

namespace foxy {
  Swapchain::Swapchain(Shared<vulkan::Context> context, Rect window_bounds)
      : context_{std::move(context)} {
    auto old_swapchain = swapchain_;

    {
      auto capabilities = context_->physical_device()->getSurfaceCapabilitiesKHR(**context_->surface());
      auto present_modes = context_->physical_device()->getSurfacePresentModesKHR(**context_->surface());

      // width and height are either both -1, or both not -1.
      if (capabilities.currentExtent.width == -1) {
        extent_->width = static_cast<u32>(window_bounds.width());
        extent_->height = static_cast<u32>(window_bounds.height());
      } else {
        // If the surface size is defined, the swap chain size must match
        extent_ = std::make_unique<vk::Extent2D>(capabilities.currentExtent);
      }

      if (!render_pass_) {

      }

      vk::SwapchainCreateInfoKHR swapchain_create_info{

      };
      //swapchain_ = std::make_unique<vk::raii::SwapchainKHR>(*context_->logical_device(), swapchain_create_info);
      swapchain_ = std::make_unique<vk::raii::SwapchainKHR>(nullptr);
    }
  }

  Swapchain::~Swapchain() {

  }
}