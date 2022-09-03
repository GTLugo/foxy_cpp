#include "renderer.hpp"

#include "foxy/api/vulkan/context.hpp"
#include "foxy/api/vulkan/swapchain.hpp"

namespace foxy {
  Renderer::Renderer() {
    context_ = std::make_shared<vulkan::Context>();
    swapchain_ = std::make_unique<Swapchain>(context_);
  }

  Renderer::~Renderer() = default;
}
