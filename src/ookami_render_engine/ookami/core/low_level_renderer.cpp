//
// Created by galex on 10/2/2022.
//

#include "low_level_renderer.hpp"

#include "context.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "shader.hpp"
#include "vulkan.hpp"
#include <GLFW/glfw3.h>

namespace fx {
  class LowLevelRenderer::Impl {
  public:
    explicit Impl(const shared<GLFWwindow>& window)
    {
      if (instantiated_) {
        Log::fatal("Attempted second instantiation of LowLevelRenderer");
      }
      instantiated_ = true;
      Log::trace("Creating Low Level Renderer...");
      
      #ifdef FOXY_DEBUG_MODE
      context_ = std::make_shared<ookami::Context>(window);
      #else
      context_ = std::make_shared<ookami::Context>(window, false);
      #endif
      
      swapchain_ = std::make_shared<Swapchain>(window, context_);
      
      Log::info("Please wait while shaders load...");
      const auto sw{ Stopwatch() };
      
      auto simple_shader = std::make_shared<Shader>(
        context_->logical_device(),
        Shader::CreateInfo{
          .vertex = true,
          .fragment = true,
          .shader_directory = "res/foxy/shaders/simple"
        }
      );
      
      auto fixed_value_shader = std::make_shared<Shader>(
        context_->logical_device(),
        Shader::CreateInfo{
          .vertex = true,
          .fragment = true,
          .shader_directory = "res/foxy/shaders/fixed_value"
        }
      );
      
      Log::info("Shader loading complete! ({} s)", sw.get_time_elapsed<secs>());
      pipeline_ = std::make_shared<Pipeline>(context_, swapchain_, fixed_value_shader);
      
      Log::trace("Low Level Renderer ready.");
    }
    
    ~Impl()
    {
      instantiated_ = false;
    }
    
    void draw() {
    
    }
  
  private:
    static inline bool instantiated_{ false };
    
    shared <ookami::Context> context_;
    shared <Swapchain> swapchain_;
    shared <Pipeline> pipeline_;
  };
  
  //
  //  Renderer
  //
  
  LowLevelRenderer::LowLevelRenderer(const shared<GLFWwindow>& window):
    p_impl_{ std::make_unique<Impl>(window) } {}
  
  LowLevelRenderer::~LowLevelRenderer() = default;
  
  void LowLevelRenderer::draw()
  {
    p_impl_->draw();
  }
} // fx