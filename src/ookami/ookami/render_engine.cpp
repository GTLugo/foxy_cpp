#include "render_engine.hpp"

#include "ookami/vulkan/context.hpp"
#include "ookami/vulkan/swapchain.hpp"
#include "ookami/vulkan/pipeline.hpp"
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
      context_ = std::make_shared<vulkan::Context>(window, false);
      #endif

      swapchain_ = std::make_unique<Swapchain>(window, context_);
      pipeline_ = std::make_unique<Pipeline>(context_);

      koyote::Log::trace("Ookami Render Engine ready.");
    }

    ~Impl() {
      instantiated_ = false;
      koyote::Log::trace("Stopping Ookami Engine...");
    }
  private:
    static inline bool instantiated_{ false };

    koyote::shared<Context> context_;
    koyote::unique<Swapchain> swapchain_;
    koyote::unique<Pipeline> pipeline_;
  };

  //
  //  Renderer
  //

  RenderEngine::RenderEngine(koyote::shared<GLFWwindow> window)
    : pImpl_{std::make_unique<Impl>(window)} {}

  RenderEngine::~RenderEngine() = default;
}
