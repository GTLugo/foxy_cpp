//
// Created by galex on 9/8/2022.
//

#pragma once

namespace ookami {
  class Context;
}

namespace ookami {
  class Pipeline {
  public:
    explicit Pipeline(const koyote::shared<Context>& context);
    ~Pipeline();

  private:
    class Impl;
    koyote::unique<Impl> p_impl_;
  };
}  // foxy // vulkan
