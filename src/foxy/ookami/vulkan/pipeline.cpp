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
      simple_shader_ = std::make_shared<Shader>(
        context_->logical_device(), 
        "res/foxy/shaders/simple", 
        Shader::BitFlags{}.set(Shader::Vertex).set(Shader::Fragment),
        true
      );

      FOXY_TRACE << "Created Vulkan pipeline.";
    }

    ~Impl() = default;
  private:
    Shared<Context> context_;
    Shared<Shader> simple_shader_;
  };

  //
  //  Pipeline
  //

  Pipeline::Pipeline(Shared<Context> context)
    : pImpl_{ std::make_unique<Impl>(context) } {}

  Pipeline::~Pipeline() = default;
} // foxy // vulkan