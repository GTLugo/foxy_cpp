#pragma once

#include "foxy/api/vulkan/context.hpp"

namespace foxy {
  class Swapchain {
  public:
    explicit Swapchain(Shared<Context> context)
      : context_{std::move(context)} {}

    ~Swapchain() = default;
  private:
    Shared<Context> context_;
    Unique<vk::raii::SurfaceKHR> surface_;
    Unique<vk::raii::SwapchainKHR> swapchain_;
  };
}