#pragma once

class GLFWwindow;

namespace vk {
  enum class Format;
  class Extent2D;
}

namespace fx::ookami {
  class Context;
}

namespace fx {
  class Swapchain {
  public:
    explicit Swapchain(const fx::shared<GLFWwindow>& window, const fx::shared<ookami::Context>& context);
    ~Swapchain();

    [[nodiscard]] auto format() const -> vk::Format;
    [[nodiscard]] auto extent() const -> vk::Extent2D;
  private:
    class Impl;
    fx::unique<Impl> p_impl_;
  };
}