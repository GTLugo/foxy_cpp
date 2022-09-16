//
// Created by galex on 9/8/2022.
//

#include "pipeline.hpp"

#include "context.hpp"
#include "shader.hpp"

namespace ookami {
  class Pipeline::Impl {
  public:
    explicit Impl(kyt::shared<Context> context)
      : context_{std::move(context)} {
      simple_shader_ = std::make_shared<Shader>(
        context_->logical_device(), 
        "res/foxy/shaders/simple", 
        Shader::BitFlags{BIT(Shader::Vertex) + BIT(Shader::Fragment)},
        true
      );

      LOG(TRACE) << "Created Vulkan pipeline.";
    }

    ~Impl() = default;
  private:
    kyt::shared<Context> context_;
    kyt::shared<Shader> simple_shader_;
  };

  //
  //  Pipeline
  //

  Pipeline::Pipeline(kyt::shared<Context> context)
    : pImpl_{ std::make_unique<Impl>(context) } {}

  Pipeline::~Pipeline() = default;
} // foxy // vulkan