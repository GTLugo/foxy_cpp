//
// Ookami Renderer by Gabriel Lugo
//

#pragma once

namespace fx {
  class Window;
  class Shader;
  class ShaderCreateInfo;
  
  class RenderEngine {
  public:
    explicit RenderEngine(const shared<Window>& window);
    ~RenderEngine();
    
    void submit();
    void draw_frame();
    void wait_idle();

    [[nodiscard]] auto create_shader(const ShaderCreateInfo& shader_create_info) const -> unique<Shader>;

  private:
    class Impl;
    unique<Impl> p_impl_;
  };
}