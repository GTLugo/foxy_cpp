//
// Ookami Renderer by Gabriel Lugo
//

#pragma once

class GLFWwindow;

namespace fx {
  class RenderEngine {
  public:
    explicit RenderEngine(const shared<GLFWwindow>& window);
    ~RenderEngine();
    
    void submit();
    void draw_frame();

  private:
    class Impl;
    unique<Impl> p_impl_;
  };
}