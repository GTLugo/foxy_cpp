#pragma once

class GLFWwindow;

namespace ookami {
  class Context;
}

namespace ookami {
  class Swapchain {
  public:
    explicit Swapchain(koyote::shared<GLFWwindow> window, koyote::shared<Context> context);
    ~Swapchain();
  private:
    class Impl;
    koyote::unique<Impl> pImpl_;
  };
}