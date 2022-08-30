module;

#include "foxy/internal/foxy_includes.hpp"
#include "foxy/internal/vulkan.hpp"

#ifdef __INTELLISENSE__
#include "util/util.ixx"
#include "api/vulkan/context.ixx"
#endif

export module foxy_vulkan_swapchain;

#ifndef __INTELLISENSE__
export import foxy_util;
export import foxy_vulkan_context;
#endif

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
  };
}