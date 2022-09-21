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
    explicit Pipeline(koyote::shared<Context> context);
    ~Pipeline();

  private:
    class Impl;
    koyote::unique<Impl> pImpl_;
  };
}  // foxy // vulkan
