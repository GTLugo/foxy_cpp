#pragma once

class GLFWwindow;

namespace ookami {
  class Context;
}

namespace ookami {
  class Swapchain {
  public:
    explicit Swapchain(kyt::shared<GLFWwindow> window, kyt::shared<Context> context);
    ~Swapchain();
  private:
    class Impl;
    kyt::unique<Impl> pImpl_;
  };
}