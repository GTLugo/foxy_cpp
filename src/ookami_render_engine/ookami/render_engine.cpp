#include "render_engine.hpp"

#include "ookami/core/low_level_renderer.hpp"
#include <GLFW/glfw3.h>

namespace fx {
  class RenderEngine::Impl {
  public:
    explicit Impl(const shared<GLFWwindow>& window)
    {
      if (instantiated_) {
        Log::fatal("Attempted second instantiation of RenderEngine");
      }
      instantiated_ = true;
      Log::trace("Starting Ookami Render Engine...");
      
      renderer_ = std::make_unique<LowLevelRenderer>(window);
      
      Log::trace("Ookami Render Engine ready.");
    }
    
    ~Impl()
    {
      instantiated_ = false;
      Log::trace("Stopping Ookami Engine...");
    }
  
    void submit()
    {
    
    }
  
    void draw_frame()
    {
    
    }
  
  private:
    static inline bool instantiated_{ false };
  
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
