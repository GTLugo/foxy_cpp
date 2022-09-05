#include "swapchain.hpp"

#include "foxy/api/vulkan/context.hpp"
#include "foxy/api/vulkan/vulkan.hpp"

namespace foxy {
  Swapchain::Swapchain(Shared<vulkan::Context> context)
      : context_{std::move(context)} {
    auto old_swapchain = swapchain_;

    {
      vk::SwapchainCreateInfoKHR swapchain_create_info{

      };
      //swapchain_ = std::make_unique<vk::raii::SwapchainKHR>(*context_->logical_device(), swapchain_create_info);
      swapchain_ = std::make_unique<vk::raii::SwapchainKHR>(nullptr);
    }
  }

  Swapchain::~Swapchain() {

  }
}