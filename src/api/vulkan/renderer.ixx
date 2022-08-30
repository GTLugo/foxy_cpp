module;

#include "foxy/internal/foxy_includes.hpp"
#include "foxy/internal/vulkan.hpp"

#ifdef __INTELLISENSE__
#include "util/util.ixx"
#include "api/vulkan/context.ixx"
#include "api/vulkan/swapchain.ixx"
#endif

export module foxy_vulkan_renderer;

#ifndef __INTELLISENSE__
export import foxy_util;
export import foxy_vulkan_context;
export import foxy_vulkan_swapchain;
#endif

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