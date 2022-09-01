#pragma once

namespace foxy {
  class Context;
  class Swapchain;

  class Renderer {
  public:
    Renderer();

    ~Renderer();
  private:
    Shared<Context> context_;
    Unique<Swapchain> swapchain_;
  };
}