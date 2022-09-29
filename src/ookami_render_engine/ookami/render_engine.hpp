//
// Ookami Renderer by Gabriel Lugo
//

#pragma once

class GLFWwindow;

namespace fx {
  class RenderEngine {
  public:
    explicit RenderEngine(shared<GLFWwindow> window);
    ~RenderEngine();

  private:
    class Impl;
    unique<Impl> p_impl_;
  };
}