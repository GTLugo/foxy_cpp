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
        create_shader(
          ShaderCreateInfo{
            .path = "res/foxy/shaders/fixed_value.hlsl",
            .vertex = true,
            .fragment = true,
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

    [[nodiscard]] auto create_shader(const ShaderCreateInfo& shader_create_info) const -> unique<Shader>
    {
      return context_->create_shader(shader_create_info);
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

  auto RenderEngine::create_shader(const ShaderCreateInfo& shader_create_info) const -> unique<Shader>
  {
    return p_impl_->create_shader(shader_create_info);
  }
}
