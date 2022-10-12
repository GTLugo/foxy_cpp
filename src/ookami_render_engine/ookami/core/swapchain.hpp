#pragma once

class GLFWwindow;

namespace vk {
  enum class Format;
  class Extent2D;

  namespace raii {
    class ImageView;
    class SwapchainKHR;
  }
}

namespace fx {
  namespace ookami {
    class Context;
  }

  class Swapchain {
  public:
    explicit Swapchain(const fx::shared<ookami::Context>& context);
    ~Swapchain();

    [[nodiscard]] auto format() const -> vk::Format;
    [[nodiscard]] auto extent() const -> vk::Extent2D;
    [[nodiscard]] auto image_views() const -> std::vector<vk::raii::ImageView>&;
  
    auto operator*() -> vk::raii::SwapchainKHR&;
    
  private:
    class Impl;
    unique<Impl> p_impl_;
  };
}