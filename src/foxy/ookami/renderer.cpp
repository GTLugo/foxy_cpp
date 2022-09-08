#include "renderer.hpp"

#include "foxy/api/vulkan/context.hpp"
#include "foxy/api/vulkan/swapchain.hpp"
#include "foxy/core/window.hpp"

namespace foxy::ookami {
  Renderer::Renderer(Window& window) {
    #ifdef FOXY_DEBUG_MODE
    context_ = std::make_shared<vulkan::Context>(*window);
    #else
    context_ = std::make_shared<vulkan::Context>(*window, false);
    #endif

    swapchain_ = std::make_unique<vulkan::Swapchain>(context_);
  }

  Renderer::~Renderer() = default;
}
