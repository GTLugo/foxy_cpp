module;

#include "foxy/internal/foxy_includes.hpp"
#include "foxy/internal/vulkan.hpp"

export module foxy_vulkan_renderer;

import foxy_util;
import foxy_vulkan_context;
import foxy_vulkan_swapchain;

namespace foxy {
  export class Renderer {
  public:
    Renderer()
      : context_{},
        swapchain_{context_} {
      
    }

    ~Renderer() {
      
    }
  private:
    Context context_;
    Swapchain swapchain_;
  };
}