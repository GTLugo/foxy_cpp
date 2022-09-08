#include "renderer.hpp"

#include "foxy/core/window.hpp"
#include "foxy/ookami/vulkan/context.hpp"
#include "foxy/ookami/vulkan/swapchain.hpp"
#include "foxy/ookami/vulkan/pipeline.hpp"

namespace foxy::ookami {
  Renderer::Renderer(Window& window) {
    #ifdef FOXY_DEBUG_MODE
    context_ = std::make_shared<vulkan::Context>(*window);
    #else
    context_ = std::make_shared<vulkan::Context>(*window, false);
    #endif

    swapchain_ = std::make_unique<vulkan::Swapchain>(context_);
    pipeline_ = std::make_unique<vulkan::Pipeline>(context_);
  }

  Renderer::~Renderer() = default;
}
