//
// Ookami Renderer by Gabriel Lugo
//

#pragma once

class GLFWwindow;

namespace ookami {
  class RenderEngine {
  public:
    explicit RenderEngine(koyote::shared<GLFWwindow> window);
    ~RenderEngine();

  private:
    PIMPL(Impl) pImpl_;
  };
}