#pragma once

class GLFWwindow;

namespace ookami {
  class Context;
}

namespace ookami {
  class Swapchain {
  public:
    explicit Swapchain(const koyote::shared<GLFWwindow>& window, const koyote::shared<Context>& context);
    ~Swapchain();
  private:
    class Impl;
    koyote::unique<Impl> p_impl_;
  };
}