//
// Created by galex on 9/8/2022.
//

#include "pipeline.hpp"

#include "context.hpp"
#include "shader.hpp"

namespace foxy::vulkan {
  Pipeline::Pipeline(Shared<Context> context)
    : context_{std::move(context)} {
    shader_ = std::make_shared<Shader>(
      "res/foxy/shaders/simple_shader_vertex.spv",
      "res/foxy/shaders/simple_shader_fragment.spv"
    );

    FOXY_TRACE << "Created Vulkan pipeline.";
  }

  Pipeline::~Pipeline() = default;
} // foxy // vulkan