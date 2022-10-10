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
  class LowLevelRenderer::Impl: types::SingleInstance<LowLevelRenderer> {
  public:
    explicit Impl(const shared<GLFWwindow>& window):
      context_{
      #ifdef FOXY_DEBUG_MODE
        std::make_shared<ookami::Context>(window)
      #else
        std::make_shared<ookami::Context>(window, false)
      #endif
      },
      image_available_{ context_->logical_device().createSemaphore({}) },
      render_complete_{ context_->logical_device().createSemaphore({}) },
      image_in_flight_{ context_->logical_device().createFence({}) }
    {
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
    
    ~Impl() = default;
    
    void draw()
    {
    
    }
  
    void clear()
    {
    
    }
  
  private:
    shared <ookami::Context> context_;
    shared <Swapchain> swapchain_;
    shared <Pipeline> pipeline_;
    
    vk::raii::Semaphore image_available_;
    vk::raii::Semaphore render_complete_;
    vk::raii::Fence image_in_flight_;
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
  
  void LowLevelRenderer::clear()
  {
    p_impl_->clear();
  }
} // fx