//
// Created by galex on 9/8/2022.
//

#include "pipeline.hpp"

#include "context.hpp"
#include "swapchain.hpp"
#include "shader.hpp"

#include "vulkan/static.hpp"

namespace fx {
  class Pipeline::Impl {
  public:
    explicit Impl(
      const shared<ookami::Context>& context,
      const shared<Swapchain>& swapchain,
      const shared<Shader>& shader
    ):
      context_{ context },
      swapchain_{ swapchain },
      shader_{ shader }
    {
      Log::trace("Creating Vulkan pipeline...");

      std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;
      for (const auto& stage: Shader::stages) {
        if (shader_->has_stage(stage)) {
          const vk::PipelineShaderStageCreateInfo info{
            .stage = static_cast<vk::ShaderStageFlagBits>(*stage.to_vk_flag()),
            .module = *shader_->module(stage),
            .pName = "main",
          };
          shader_stages.push_back(info);
        }
      }
      Log::trace("Shader stage count: {}", shader_stages.size());

      std::vector dynamic_states{
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
      };

      vk::PipelineDynamicStateCreateInfo dynamic_state{
        .dynamicStateCount = static_cast<u32>(dynamic_states.size()),
        .pDynamicStates = dynamic_states.data(),
      };

      vk::PipelineVertexInputStateCreateInfo vertex_input_info{
        .vertexBindingDescriptionCount = 0,
        .vertexAttributeDescriptionCount = 0,
      };

      vk::PipelineInputAssemblyStateCreateInfo input_assembly_info{
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = false,
      };

      viewport_ = vk::Viewport{
        .x = 0.f,
        .y = 0.f,
        .width = static_cast<float>(swapchain_->extent().width),
        .height = static_cast<float>(swapchain_->extent().height),
        .minDepth = 0.f,
        .maxDepth = 1.f,
      };

      scissor_ = vk::Rect2D{
        .offset = { 0, 0 },
        .extent = swapchain_->extent()
      };

      vk::PipelineViewportStateCreateInfo viewport_state_info{
        .viewportCount = 1,
        .pViewports = &viewport_,
        .scissorCount = 1,
        .pScissors = &scissor_,
      };

      vk::PipelineRasterizationStateCreateInfo rasterizer_info{
        .depthClampEnable = false,
        .rasterizerDiscardEnable = false,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable = false,
        .lineWidth = 1.f,
      };

      vk::PipelineMultisampleStateCreateInfo multisampling_info{
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = false,
      };

      color_blend_attachment_ = vk::PipelineColorBlendAttachmentState{
        .blendEnable = true,
        .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
        .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
        .colorBlendOp = vk::BlendOp::eAdd,
        .srcAlphaBlendFactor = vk::BlendFactor::eOne,
        .dstAlphaBlendFactor = vk::BlendFactor::eZero,
        .alphaBlendOp = vk::BlendOp::eAdd,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
      };

      vk::PipelineColorBlendStateCreateInfo color_blend_info{
        .logicOpEnable = false,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment_,
      };

      try {
        layout_ = std::make_unique<vk::raii::PipelineLayout>(
          context_->logical_device(),
          vk::PipelineLayoutCreateInfo{
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
          }
        );
      } catch (const std::exception& e) {
        Log::fatal("Failed to create pipeline layout: {}", e.what());
      }

      vk::GraphicsPipelineCreateInfo pipeline_info{
        .stageCount = static_cast<u32>(shader_stages.size()),
        .pStages = shader_stages.data(),
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly_info,
        .pViewportState = &viewport_state_info,
        .pRasterizationState = &rasterizer_info,
        .pMultisampleState = &multisampling_info,
        .pDepthStencilState = nullptr,
        .pColorBlendState = &color_blend_info,
        .pDynamicState = &dynamic_state,
        .layout = **layout_,
        .renderPass = **swapchain_->render_pass(),
        .subpass = 0,
      };

      try {
        pipeline_ = std::make_unique<vk::raii::Pipeline>(context_->logical_device(), nullptr, pipeline_info);
      } catch (const std::exception& e) {
        Log::fatal("Failed to create graphics pipeline: {}", e.what());
      }

      Log::trace("Created Vulkan pipeline.");
    }

    ~Impl() = default;
  
    [[nodiscard]] auto viewport() const -> const vk::Viewport&
    {
      return viewport_;
    }
  
    [[nodiscard]] auto scissor() const -> const vk::Rect2D&
    {
      return scissor_;
    }
  
    auto operator*() -> unique<vk::raii::Pipeline>&
    {
      return pipeline_;
    }

  private:
    shared<ookami::Context> context_;
    shared<Swapchain> swapchain_;
    shared<Shader> shader_;
    
    vk::Viewport viewport_;
    vk::Rect2D scissor_;
    vk::PipelineColorBlendAttachmentState color_blend_attachment_;
    unique<vk::raii::PipelineLayout> layout_;
    unique<vk::raii::Pipeline> pipeline_;
  };

  //
  //  Pipeline
  //

  Pipeline::Pipeline(
    const shared<ookami::Context>& context,
    const shared<Swapchain>& swapchain,
    const shared<Shader>& shader
  ):
    p_impl_{ std::make_unique<Impl>(context, swapchain, shader) } {}

  Pipeline::~Pipeline() = default;
  
  auto Pipeline::viewport() const -> const vk::Viewport&
  {
    return p_impl_->viewport();
  }
  
  auto Pipeline::scissor() const -> const vk::Rect2D&
  {
    return p_impl_->scissor();
  }
  
  auto Pipeline::operator*() -> unique<vk::raii::Pipeline>&
  {
    return **p_impl_;
  }
} // foxy // vulkan