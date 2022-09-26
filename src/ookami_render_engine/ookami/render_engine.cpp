#include "render_engine.hpp"

#include "ookami/vulkan/context.hpp"
#include "ookami/vulkan/swapchain.hpp"
#include "ookami/vulkan/pipeline.hpp"
#include "ookami/vulkan/shader.hpp"
#include <GLFW/glfw3.h>

namespace ookami {
  class RenderEngine::Impl {
  public:
    explicit Impl(koyote::shared<GLFWwindow> window) {
      if (instantiated_) {
        koyote::Log::fatal("Attempted second instantiation of ookami::RenderEngine");
      }
      instantiated_ = true;
      koyote::Log::trace("Starting Ookami Render Engine...");

      #ifdef FOXY_DEBUG_MODE
      context_ = std::make_shared<Context>(window);
      #else
      context_ = std::make_shared<Context>(window, false);
      #endif

      swap_chain_ = std::make_shared<Swapchain>(window, context_);

      koyote::Log::info("Please wait while shaders load...");
      const auto sw{ koyote::Stopwatch() };

      auto simple_shader = std::make_shared<Shader>(
        context_->logical_device(),
        Shader::CreateInfo{
          .vertex = true,
          .fragment = true,
          .shader_directory = "res/foxy/shaders/simple"
        }
      );

      koyote::Log::info("Shader loading complete! ({} s)", sw.get_time_elapsed<koyote::secs>());
      pipeline_ = std::make_unique<Pipeline>(context_, swap_chain_, simple_shader);

      koyote::Log::trace("Ookami Render Engine ready.");
    }

    ~Impl() {
      instantiated_ = false;
      koyote::Log::trace("Stopping Ookami Engine...");
    }
  private:
    static inline bool instantiated_{ false };

    koyote::shared<Context> context_;
    koyote::shared<Swapchain> swap_chain_;
    koyote::unique<Pipeline> pipeline_;
  };

  //
  //  Renderer
  //

  RenderEngine::RenderEngine(koyote::shared<GLFWwindow> window)
    : pImpl_{std::make_unique<Impl>(window)} {}

  RenderEngine::~RenderEngine() = default;
}
