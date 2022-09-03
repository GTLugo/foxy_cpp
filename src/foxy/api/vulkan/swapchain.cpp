#include "swapchain.hpp"

#include "foxy/api/vulkan/context.hpp"
#include "foxy/api/vulkan/vulkan.hpp"

namespace foxy {
  Swapchain::Swapchain(Shared<vulkan::Context> context)
      : context_{std::move(context)} {
    auto old_swapchain = swapchain_;

    {
      vk::DisplaySurfaceCreateInfoKHR display_surface_create_info{

      };
      //surface_ = std::make_unique<vk::raii::SurfaceKHR>(*context_->instance(), display_surface_create_info);
      surface_ = std::make_unique<vk::raii::SurfaceKHR>(nullptr);
    }

    {
      vk::SwapchainCreateInfoKHR swapchain_create_info{

      };
      //swapchain_ = std::make_unique<vk::raii::SwapchainKHR>(*context_->device(), swapchain_create_info);
      swapchain_ = std::make_unique<vk::raii::SwapchainKHR>(nullptr);
    }
  }

  Swapchain::~Swapchain() {

  }
}