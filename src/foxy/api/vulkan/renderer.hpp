#pragma once

namespace foxy {
  namespace vulkan {
    class Context;
  }

  class Swapchain;
  class Window;

  class Renderer {
  public:
    Renderer(Window& window);

    ~Renderer();
  private:
    Shared<vulkan::Context> context_;
    Unique<Swapchain> swapchain_;
  };
}