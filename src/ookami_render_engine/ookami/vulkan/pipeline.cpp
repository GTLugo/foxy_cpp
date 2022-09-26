//
// Created by galex on 9/8/2022.
//

#include "pipeline.hpp"

#include "ookami/vulkan/context.hpp"
#include "ookami/vulkan/swapchain.hpp"
#include "ookami/vulkan/shader.hpp"
#include "ookami/vulkan/vulkan.hpp"

namespace ookami {
  class Pipeline::Impl {
  public:
    explicit Impl(koyote::shared<Context> context, koyote::shared<Swapchain> swap_chain, koyote::shared<Shader> shader)
      : context_{ std::move(context) },
        swap_chain_{ std::move(swap_chain) },
        shader_{ std::move(shader) } {
      koyote::Log::trace("Creating Vulkan pipeline...");

      std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;
      if (shader_->has_stage(Shader::Kind::Vertex)) {
        const vk::PipelineShaderStageCreateInfo info{
          .stage = vk::ShaderStageFlagBits::eVertex,
          .module = *shader_->module(Shader::Kind::Vertex),
          .pName = "main",
        };
        shader_stages.push_back(info);
      }

      if (shader_->has_stage(Shader::Kind::Fragment)) {
        const vk::PipelineShaderStageCreateInfo info{
          .stage = vk::ShaderStageFlagBits::eFragment,
          .module = *shader_->module(Shader::Kind::Fragment),
          .pName = "main",
        };
        shader_stages.push_back(info);
      }

      if (shader_->has_stage(Shader::Kind::Compute)) {
        const vk::PipelineShaderStageCreateInfo info{
          .stage = vk::ShaderStageFlagBits::eCompute,
          .module = *shader_->module(Shader::Kind::Compute),
          .pName = "main",
        };
        shader_stages.push_back(info);
      }

      if (shader_->has_stage(Shader::Kind::Geometry)) {
        const vk::PipelineShaderStageCreateInfo info{
          .stage = vk::ShaderStageFlagBits::eGeometry,
          .module = *shader_->module(Shader::Kind::Geometry),
          .pName = "main",
        };
        shader_stages.push_back(info);
      }

      koyote::Log::trace("Shader stage count: {}", shader_stages.size());

      std::vector dynamic_states{
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
      };

      vk::PipelineDynamicStateCreateInfo dynamic_state{
        .dynamicStateCount = static_cast<koyote::u32>(dynamic_states.size()),
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
        .width = static_cast<float>(swap_chain_->extent().width),
        .height = static_cast<float>(swap_chain_->extent().height),
        .minDepth = 0.f,
        .maxDepth = 1.f,
      };

      scissor_ = vk::Rect2D{
        .offset = { 0, 0 },
        .extent = swap_chain_->extent()
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
        koyote::Log::fatal("Failed to create pipeline layout: {}", e.what());
      }

      vk::AttachmentDescription color_attachment{
        .format = swap_chain_->format(),
        .samples = vk::SampleCountFlagBits::e1,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::ePresentSrcKHR,
      };

      vk::AttachmentReference color_attachment_ref{
        .attachment = 0,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
      };

      vk::SubpassDescription subpass{
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment_ref,
      };

      try {
        render_pass_ = std::make_unique<vk::raii::RenderPass>(
          context_->logical_device(),
          vk::RenderPassCreateInfo{
            .attachmentCount = 1,
            .pAttachments = &color_attachment,
            .subpassCount = 1,
            .pSubpasses = &subpass,
          }
        );
      } catch (const std::exception& e) {
        koyote::Log::fatal("Failed to create render pass: {}", e.what());
      }

      vk::GraphicsPipelineCreateInfo pipeline_info{
        .stageCount = static_cast<koyote::u32>(shader_stages.size()),
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
        .renderPass = **render_pass_,
        .subpass = 0,
      };

      try {
        pipeline_ = std::make_unique<vk::raii::Pipeline>(context_->logical_device(), nullptr, pipeline_info);
      } catch (const std::exception& e) {
        koyote::Log::fatal("Failed to create graphics pipeline: {}", e.what());
      }

      koyote::Log::trace("Created Vulkan pipeline.");
    }

    ~Impl() = default;
  private:
    koyote::shared<Context> context_;
    koyote::shared<Swapchain> swap_chain_;

    koyote::shared<Shader> shader_;
    vk::Viewport viewport_;
    vk::Rect2D scissor_;
    vk::PipelineColorBlendAttachmentState color_blend_attachment_;
    koyote::unique<vk::raii::PipelineLayout> layout_;
    koyote::unique<vk::raii::RenderPass> render_pass_;
    koyote::unique<vk::raii::Pipeline> pipeline_;
  };

  //
  //  Pipeline
  //

  Pipeline::Pipeline(koyote::shared<Context> context, koyote::shared<Swapchain> swap_chain, koyote::shared<Shader> shader)
    : p_impl_{ std::make_unique<Impl>(std::move(context), std::move(swap_chain), std::move(shader)) } {}

  Pipeline::~Pipeline() = default;
} // foxy // vulkan