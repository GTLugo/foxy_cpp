//
// Ookami Renderer by Gabriel Lugo
//

#pragma once

namespace fx {
  class Window;
  
  class RenderEngine {
  public:
    explicit RenderEngine(const shared<Window>& window);
    ~RenderEngine();
    
    void submit();
    void draw_frame();
    void wait_idle();

  private:
    class Impl;
    unique<Impl> p_impl_;
  };
}