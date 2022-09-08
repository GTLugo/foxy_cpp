//
// Created by galex on 9/8/2022.
//

#pragma once

#include "context.hpp"

namespace foxy::vulkan {
  class Shader;

  class Pipeline {
  public:
    Pipeline(Shared<vulkan::Context> context);
    ~Pipeline();
  private:
    Shared<vulkan::Context> context_;
    Shared<Shader> shader_;
  };
}  // foxy // vulkan
