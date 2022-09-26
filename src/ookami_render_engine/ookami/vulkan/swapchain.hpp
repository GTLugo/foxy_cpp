#pragma once

class GLFWwindow;

namespace vk {
  enum class Format;
  class Extent2D;
}

namespace ookami {
  class Context;
}

namespace ookami {
  class Swapchain {
  public:
    explicit Swapchain(const koyote::shared<GLFWwindow>& window, const koyote::shared<Context>& context);
    ~Swapchain();

    [[nodiscard]] auto format() -> vk::Format;
    [[nodiscard]] auto extent() -> vk::Extent2D;
  private:
    class Impl;
    koyote::unique<Impl> p_impl_;
  };
}