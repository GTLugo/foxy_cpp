//
// Ookami Renderer by Gabriel Lugo
//

#pragma once

class GLFWwindow;

namespace fx {
  class RenderEngine {
  public:
    explicit RenderEngine(fx::shared<GLFWwindow> window);
    ~RenderEngine();

  private:
    PIMPL(Impl) pImpl_;
  };
}