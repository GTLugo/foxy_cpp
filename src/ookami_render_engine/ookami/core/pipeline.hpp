//
// Created by galex on 9/8/2022.
//

#pragma once

namespace vk::raii {
  class RenderPass;
}

namespace fx {
  class Shader;
  class Swapchain;
  class Framebuffer;

  namespace ookami {
    class Context;
  }

  class Pipeline {
  public:
    explicit Pipeline(shared<ookami::Context> context, shared<Swapchain> swap_chain, shared<Shader> shader);
    ~Pipeline();
  private:
    class Impl;
    unique<Impl> p_impl_;
  };
}  // foxy // vulkan
