#include "render_engine.hpp"

#include "ookami/core/context.hpp"
#include "ookami/core/shader.hpp"
#include "ookami/core/low_level_renderer.hpp"

#include "vulkan/static.hpp"
#include <inferno/window.hpp>

namespace fx {
  class RenderEngine::Impl: types::SingleInstance<RenderEngine> {
  public:
    explicit Impl(const shared<Window>& window):
      context_{ std::make_shared<ookami::Context>(**window) }
    {
      std::shared_ptr fixed_value_shader{
        context_->create_shader(
          ShaderCreateInfo{
            .vertex = true,
            .fragment = true,
            .shader_directory = "res/foxy/shaders/fixed_value"
          }
        )
      };
      
      renderer_ = std::make_unique<LowLevelRenderer>(window, context_, fixed_value_shader, 2);
      
      Log::trace("Ookami Render Engine ready.");
    }
    
    ~Impl()
    {
      Log::trace("Destroying Ookami Render Engine...");
    }
  
    void submit()
    {
    
    }
  
    void draw_frame()
    {
      renderer_->draw();
    }
  
    void wait_idle()
    {
      context_->logical_device().waitIdle();
    }
  
  private:
    shared<ookami::Context> context_;
    unique<LowLevelRenderer> renderer_;
  };
  
  //
  //  Renderer
  //
  
  RenderEngine::RenderEngine(const shared<Window>& window):
    p_impl_{ std::make_unique<Impl>(window) } {}
  
  RenderEngine::~RenderEngine() = default;
  
  void RenderEngine::submit()
  {
    p_impl_->submit();
  }
  
  void RenderEngine::draw_frame()
  {
    p_impl_->draw_frame();
  }
  
  void RenderEngine::wait_idle()
  {
    p_impl_->wait_idle();
  }
}
