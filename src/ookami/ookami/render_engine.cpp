#include "render_engine.hpp"

#include "ookami/vulkan/context.hpp"
#include "ookami/vulkan/swapchain.hpp"
#include "ookami/vulkan/pipeline.hpp"
#include <GLFW/glfw3.h>

namespace ookami {
  class RenderEngine::Impl {
  public:
    explicit Impl(kyt::shared<GLFWwindow> window) {
      DCHECK(!instantiated_) << "Attempted second instantiation of foxy::ookami::Renderer";
      instantiated_ = true;
      LOG(TRACE) << "Starting Ookami Renderer...";

      #ifdef FOXY_DEBUG_MODE
      context_ = std::make_shared<Context>(window);
      #else
      context_ = std::make_shared<vulkan::Context>(window, false);
      #endif

      swapchain_ = std::make_unique<Swapchain>(window, context_);
      pipeline_ = std::make_unique<Pipeline>(context_);

      LOG(TRACE) << "Ookami Renderer ready.";
    }

    ~Impl() {
      instantiated_ = false;
      LOG(TRACE) << "Stopping Ookami Renderer...";
    }
  private:
    static inline bool instantiated_{ false };

    kyt::shared<Context> context_;
    kyt::unique<Swapchain> swapchain_;
    kyt::unique<Pipeline> pipeline_;
  };

  //
  //  Renderer
  //

  RenderEngine::RenderEngine(kyt::shared<GLFWwindow> window)
    : pImpl_{std::make_unique<Impl>(window)} {}

  RenderEngine::~RenderEngine() = default;
}
