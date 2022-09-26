//
// Created by galex on 9/8/2022.
//

#pragma once

namespace ookami {
  class Context;
  class Shader;
  class Swapchain;
}

namespace ookami {
  class Pipeline {
  public:
    explicit Pipeline(koyote::shared<Context> context, koyote::shared<Swapchain> swap_chain, koyote::shared<Shader> shader);
    ~Pipeline();

  private:
    class Impl;
    koyote::unique<Impl> p_impl_;
  };
}  // foxy // vulkan
