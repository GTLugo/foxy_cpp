#include "renderer.hpp"

#include "foxy/core/window/glfw/glfw_context.hpp"
#include "foxy/ookami/vulkan/vulkan_context.hpp"
#include "foxy/ookami/vulkan/swapchain.hpp"
#include "foxy/ookami/vulkan/pipeline.hpp"

namespace foxy::ookami {
  class Renderer::Impl {
  public:
    explicit Impl(Shared<GLFWwindow> window) {
      FOXY_ASSERT(!instantiated_) << "Attempted second instantiation of foxy::ookami::Renderer";
      instantiated_ = true;
      FOXY_TRACE << "Starting Ookami Renderer...";

      #ifdef FOXY_DEBUG_MODE
      context_ = std::make_shared<vulkan::Context>(window);
      #else
      context_ = std::make_shared<vulkan::Context>(window, false);
      #endif

      swapchain_ = std::make_unique<vulkan::Swapchain>(window, context_);
      pipeline_ = std::make_unique<vulkan::Pipeline>(context_);

      FOXY_TRACE << "Ookami Renderer ready.";
    }

    ~Impl() {
      instantiated_ = false;
      FOXY_TRACE << "Stopping Ookami Renderer...";
    }
  private:
    static inline bool instantiated_{ false };

    Shared<vulkan::Context> context_;
    Unique<vulkan::Swapchain> swapchain_;
    Unique<vulkan::Pipeline> pipeline_;
  };

  //
  //  Renderer
  //

  Renderer::Renderer(Shared<GLFWwindow> window)
    : pImpl_{std::make_unique<Impl>(window)} {}

  Renderer::~Renderer() = default;
}
