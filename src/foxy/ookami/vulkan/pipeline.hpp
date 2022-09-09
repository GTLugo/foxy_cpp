//
// Created by galex on 9/8/2022.
//

#pragma once

namespace foxy::vulkan {
  class Shader;
  class Context;

  class Pipeline {
  public:
    Pipeline(Shared<Context> context);
    ~Pipeline();
  private:
    Shared<Context> context_;
    Shared<Shader> shader_;
  };
}  // foxy // vulkan
