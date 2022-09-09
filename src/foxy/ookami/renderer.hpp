//
// Ookami Renderer by Gabriel Lugo
//

#pragma once

class GLFWwindow;

namespace foxy::vulkan {
  class Context;
  class Swapchain;
  class Shader;
  class Pipeline;
}

namespace foxy::ookami {
  class Renderer {
  public:
    Renderer(Unique<GLFWwindow, void(*)(GLFWwindow*)>& window);

    ~Renderer();
  private:
    Shared<vulkan::Context> context_;
    Unique<vulkan::Swapchain> swapchain_;
    Unique<vulkan::Pipeline> pipeline_;
  };
}