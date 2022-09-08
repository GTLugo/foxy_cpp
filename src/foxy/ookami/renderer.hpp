//
// Ookami Renderer by Gabriel Lugo
//

#pragma once

namespace foxy {
  namespace vulkan {
    class Context;
    class Swapchain;
  }

  class Window;
}

namespace foxy::ookami {
  class Renderer {
  public:
    Renderer(Window& window);

    ~Renderer();
  private:
    Shared<vulkan::Context> context_;
    Unique<vulkan::Swapchain> swapchain_;
  };
}