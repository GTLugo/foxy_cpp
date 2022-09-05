#include "renderer.hpp"

#include "foxy/api/vulkan/context.hpp"
#include "foxy/api/vulkan/swapchain.hpp"
#include "foxy/api/glfw/glfw.hpp"

namespace foxy {
  Renderer::Renderer(glfw::UniqueWindow& window) {
    context_ = std::make_shared<vulkan::Context>(window);
    swapchain_ = std::make_unique<Swapchain>(context_);
  }

  Renderer::~Renderer() = default;
}
