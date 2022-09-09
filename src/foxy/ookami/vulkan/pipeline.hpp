//
// Created by galex on 9/8/2022.
//

#pragma once

namespace foxy::vulkan {
  class Context;
}

namespace foxy::vulkan {
  class Pipeline {
  public:
    explicit Pipeline(Shared<Context> context);
    ~Pipeline();

  private:
    class Impl;
    Unique<Impl> pImpl_;
  };
}  // foxy // vulkan
