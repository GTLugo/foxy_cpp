#include "render_engine.hpp"

#include "ookami/core/low_level_renderer.hpp"
#include <GLFW/glfw3.h>

namespace fx {
  class RenderEngine::Impl: types::SingleInstance<RenderEngine> {
  public:
    explicit Impl(const shared<GLFWwindow>& window)
    {
      Log::trace("Starting Ookami Render Engine...");
      
      renderer_ = std::make_unique<LowLevelRenderer>(window);
      
      Log::trace("Ookami Render Engine ready.");
    }
    
    ~Impl()
    {
      Log::trace("Stopping Ookami Engine...");
    }
  
    void submit()
    {
    
    }
  
    void draw_frame()
    {
      renderer_->draw();
    }
  
  private:
    unique<LowLevelRenderer> renderer_;
  };
  
  //
  //  Renderer
  //
  
  RenderEngine::RenderEngine(const shared<GLFWwindow>& window)
    : p_impl_{ std::make_unique<Impl>(window) }
  {}
  
  RenderEngine::~RenderEngine() = default;
  
  void RenderEngine::draw_frame()
  {
    p_impl_->draw_frame();
  }
  
  void RenderEngine::submit()
  {
    p_impl_->submit();
  }
}
