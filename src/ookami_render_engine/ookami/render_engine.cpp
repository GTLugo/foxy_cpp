#include "render_engine.hpp"

#include "core/context.hpp"
#include "core/swapchain.hpp"
#include "core/pipeline.hpp"
#include "core/shader.hpp"
#include <GLFW/glfw3.h>

namespace fx {
  class RenderEngine::Impl {
  public:
    explicit Impl(shared<GLFWwindow> window)
    {
      if (instantiated_) {
        Log::fatal("Attempted second instantiation of RenderEngine");
      }
      instantiated_ = true;
      Log::trace("Starting Ookami Render Engine...");

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

      auto fixed_shader = std::make_shared<Shader>(
        context_->logical_device(),
        Shader::CreateInfo{
          .vertex = true,
          .fragment = true,
          .shader_directory = "res/foxy/shaders/fixed"
        }
      );

      Log::info("Shader loading complete! ({} s)", sw.get_time_elapsed<secs>());
      pipeline_ = std::make_shared<Pipeline>(context_, swapchain_, fixed_shader);

      Log::trace("Ookami Render Engine ready.");
    }

    ~Impl()
    {
      instantiated_ = false;
      Log::trace("Stopping Ookami Engine...");
    }
  private:
    static inline bool instantiated_{ false };

    shared<ookami::Context> context_;
    shared<Swapchain> swapchain_;
    shared<Pipeline> pipeline_;
  };

  //
  //  Renderer
  //

  RenderEngine::RenderEngine(shared<GLFWwindow> window)
    : p_impl_{std::make_unique<Impl>(window)} {}

  RenderEngine::~RenderEngine() = default;
}
