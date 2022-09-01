module;

#include "foxy/internal/foxy_includes.hpp"
#include "foxy/internal/vulkan.hpp"

export module foxy_vulkan_swapchain;

import foxy_util;
import foxy_vulkan_context;

namespace foxy {
  export class Swapchain {
  public:
    Swapchain(Context& context)
      : context_{context} {
        
    }

    ~Swapchain() {

    }
  private:
    Context& context_;
    unique<vk::raii::SurfaceKHR> surface_;
    unique<vk::raii::SwapchainKHR> swapchain_;
  };
}