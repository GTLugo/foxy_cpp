#pragma once

#include "foxy/api/glfw/unique_window.hpp"

namespace foxy {
  namespace vulkan {
    class Context;
  }
  class Swapchain;

  class Renderer {
  public:
    Renderer(glfw::UniqueWindow& window);

    ~Renderer();
  private:
    Shared<vulkan::Context> context_;
    Unique<Swapchain> swapchain_;
  };
}