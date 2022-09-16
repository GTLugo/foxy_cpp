//
// Ookami Renderer by Gabriel Lugo
//

#pragma once

class GLFWwindow;

namespace ookami {
  class RenderEngine {
  public:
    explicit RenderEngine(kyt::shared<GLFWwindow> window);
    ~RenderEngine();

  private:
    PIMPL(Impl) pImpl_;
  };
}