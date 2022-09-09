//
// Created by galex on 9/8/2022.
//

#include "pipeline.hpp"

#include "vulkan_context.hpp"
#include "shader.hpp"

namespace foxy::vulkan {
  class Pipeline::Impl {
  public:
    explicit Impl(Shared<Context> context)
      : context_{std::move(context)} {
      shader_ = std::make_shared<Shader>(
        "res/foxy/shaders/simple_shader_vertex.spv",
        "res/foxy/shaders/simple_shader_fragment.spv"
      );

      FOXY_TRACE << "Created Vulkan pipeline.";
    }

    ~Impl() = default;
  private:
    Shared<Context> context_;
    Shared<Shader> shader_;
  };

  //
  //  Pipeline
  //

  Pipeline::Pipeline(Shared<Context> context)
    : pImpl_{ std::make_unique<Impl>(context) } {}

  Pipeline::~Pipeline() = default;
} // foxy // vulkan