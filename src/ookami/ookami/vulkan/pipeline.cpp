//
// Created by galex on 9/8/2022.
//

#include "pipeline.hpp"

#include "context.hpp"
#include "shader.hpp"

namespace ookami {
  class Pipeline::Impl {
  public:
    explicit Impl(koyote::shared<Context> context)
      : context_{std::move(context)} {
      koyote::Log::trace("Creating Vulkan pipeline...");
      
      koyote::Log::info("Please wait while shaders load...");
      auto sw{ koyote::Stopwatch() };
      simple_shader_ = std::make_shared<Shader>(
        context_->logical_device(), 
        "res/foxy/shaders/simple", 
        Shader::BitFlags{BIT(Shader::Vertex) + BIT(Shader::Fragment)},
        true
      );
      koyote::Log::info("Shader loading complete! ({} s)", sw.get_time_elapsed<koyote::secs>());

      koyote::Log::trace("Created Vulkan pipeline.");
    }

    ~Impl() = default;
  private:
    koyote::shared<Context> context_;
    koyote::shared<Shader> simple_shader_;
  };

  //
  //  Pipeline
  //

  Pipeline::Pipeline(koyote::shared<Context> context)
    : pImpl_{ std::make_unique<Impl>(context) } {}

  Pipeline::~Pipeline() = default;
} // foxy // vulkan