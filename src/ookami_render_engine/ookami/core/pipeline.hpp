//
// Created by galex on 9/8/2022.
//

#pragma once

namespace vk {
  class Viewport;
  class Rect2D;
  
  namespace raii {
    class RenderPass;
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
    explicit Pipeline(
      const shared<ookami::Context>& context,
      const shared<Swapchain>& swapchain,
      const shared<Shader>& shader
    );
    ~Pipeline();
    
    [[nodiscard]] auto viewport() const -> const vk::Viewport&;
    [[nodiscard]] auto scissor() const -> const vk::Rect2D&;
  
    auto operator*() -> unique<vk::raii::Pipeline>&;
    
  private:
    class Impl;
    unique<Impl> p_impl_;
  };
}  // foxy // vulkan
