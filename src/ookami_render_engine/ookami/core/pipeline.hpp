//
// Created by galex on 9/8/2022.
//

#pragma once

namespace vk {
  class Viewport;
  class Rect2D;
  
  namespace raii {
    class RenderPass;
    class Framebuffer;
    class Pipeline;
  }
}

namespace fx {
  class Shader;
  class Swapchain;

  namespace ookami {
    class Context;
  }

  class Pipeline {
  public:
    explicit Pipeline(shared<ookami::Context> context, shared<Swapchain> swap_chain, shared<Shader> shader);
    ~Pipeline();
  
    [[nodiscard]] auto render_pass() const -> const shared<vk::raii::RenderPass>&;
    [[nodiscard]] auto framebuffers() -> std::vector<vk::raii::Framebuffer>&;
    [[nodiscard]] auto viewport() const -> const vk::Viewport&;
    [[nodiscard]] auto scissor() const -> const vk::Rect2D&;
  
    auto operator*() -> unique<vk::raii::Pipeline>&;
    
  private:
    class Impl;
    unique<Impl> p_impl_;
  };
}  // foxy // vulkan
