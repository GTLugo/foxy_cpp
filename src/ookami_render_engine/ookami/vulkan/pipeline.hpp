//
// Created by galex on 9/8/2022.
//

#pragma once

namespace fx {
  class Shader;
  class Swapchain;
}

namespace fx::ookami {
  class Context;
}

namespace fx {
  class Pipeline {
  public:
    explicit Pipeline(shared<ookami::Context> context, shared<Swapchain> swap_chain, shared<Shader> shader);
    ~Pipeline();

  private:
    class Impl;
    fx::unique<Impl> p_impl_;
  };
}  // foxy // vulkan
