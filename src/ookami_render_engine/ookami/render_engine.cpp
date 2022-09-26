#include "render_engine.hpp"

#include "ookami/vulkan/context.hpp"
#include "ookami/vulkan/swapchain.hpp"
#include "ookami/vulkan/pipeline.hpp"
#include "ookami/vulkan/shader.hpp"
#include <GLFW/glfw3.h>

namespace fx {
  class RenderEngine::Impl {
  public:
    explicit Impl(fx::shared<GLFWwindow> window) {
      if (instantiated_) {
        fx::Log::fatal("Attempted second instantiation of fx::RenderEngine");
      }
      instantiated_ = true;
      fx::Log::trace("Starting Ookami Render Engine...");

      #ifdef FOXY_DEBUG_MODE
      context_ = std::make_shared<ookami::Context>(window);
      #else
      context_ = std::make_shared<ookami::Context>(window, false);
      #endif

      swap_chain_ = std::make_shared<Swapchain>(window, context_);

      fx::Log::info("Please wait while shaders load...");
      const auto sw{ fx::Stopwatch() };

      auto simple_shader = std::make_shared<Shader>(
        context_->logical_device(),
        Shader::CreateInfo{
          .vertex = true,
          .fragment = true,
          .shader_directory = "res/foxy/shaders/simple"
        }
      );

      auto fixed_shader = std::make_shared<Shader>(
        context_->logical_device(),
        Shader::CreateInfo{
          .vertex = true,
          .fragment = true,
          .shader_directory = "res/foxy/shaders/fixed"
        }
      );

      fx::Log::info("Shader loading complete! ({} s)", sw.get_time_elapsed<fx::secs>());
      pipeline_ = std::make_unique<Pipeline>(context_, swap_chain_, fixed_shader);

      fx::Log::trace("Ookami Render Engine ready.");
    }

    ~Impl() {
      instantiated_ = false;
      fx::Log::trace("Stopping Ookami Engine...");
    }
  private:
    static inline bool instantiated_{ false };

    fx::shared<ookami::Context> context_;
    fx::shared<Swapchain> swap_chain_;
    fx::unique<Pipeline> pipeline_;
  };

  //
  //  Renderer
  //

  RenderEngine::RenderEngine(fx::shared<GLFWwindow> window)
    : pImpl_{std::make_unique<Impl>(window)} {}

  RenderEngine::~RenderEngine() = default;
}
