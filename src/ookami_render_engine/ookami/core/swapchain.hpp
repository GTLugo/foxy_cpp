#pragma once

class GLFWwindow;

namespace vk {
  enum class Format;
  class Extent2D;

  namespace raii {
    class ImageView;
    class SwapchainKHR;
    class RenderPass;
    class Framebuffer;
    class Semaphore;
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
  
    [[nodiscard]] auto dirty() const -> bool;
    void rebuild();
    [[nodiscard]] auto acquire_next_image(const vk::raii::Semaphore& semaphore) -> std::optional<u32>;
  
    [[nodiscard]] auto context() const -> const shared<ookami::Context>&;
    [[nodiscard]] auto format() const -> vk::Format;
    [[nodiscard]] auto extent() const -> vk::Extent2D;
    [[nodiscard]] auto image_views() const -> std::vector<vk::raii::ImageView>&;
    [[nodiscard]] auto render_pass() const -> const shared<vk::raii::RenderPass>&;
    [[nodiscard]] auto framebuffers() -> std::vector<vk::raii::Framebuffer>&;
  
    auto operator*() -> vk::raii::SwapchainKHR&;
    
  private:
    class Impl;
    unique<Impl> p_impl_;
  };
}