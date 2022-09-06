#include "renderer.hpp"

#include "foxy/api/vulkan/context.hpp"
#include "foxy/api/vulkan/swapchain.hpp"
#include "foxy/core/window.hpp"

namespace foxy {
  Renderer::Renderer(Window& window) {
    context_ = std::make_shared<vulkan::Context>(window.native());
    //swapchain_ = std::make_unique<Swapchain>(context_, window.bounds());
  }

  Renderer::~Renderer() = default;
}
