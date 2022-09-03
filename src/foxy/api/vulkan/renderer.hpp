#pragma once

namespace foxy {
  namespace vulkan {
    class Context;
  }
  class Swapchain;

  class Renderer {
  public:
    Renderer();

    ~Renderer();
  private:
    Shared<vulkan::Context> context_;
    Unique<Swapchain> swapchain_;
  };
}