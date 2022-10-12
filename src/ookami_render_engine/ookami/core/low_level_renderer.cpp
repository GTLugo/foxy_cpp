//
// Created by galex on 10/2/2022.
//

#include "low_level_renderer.hpp"

#include "context.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "shader.hpp"
#include "vulkan.hpp"

namespace fx {
  class LowLevelRenderer::Impl: types::SingleInstance<LowLevelRenderer> {
  public:
    explicit Impl(const shared<ookami::Context>& context, const shared<Shader>& shader):
      context_{ context },
      swapchain_{ std::make_shared<Swapchain>(context_) },
      command_pool_{
        context_->logical_device().createCommandPool(vk::CommandPoolCreateInfo{
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = *context_->queue_families().graphics,
        })
      },
      command_buffers_{
        context->logical_device(),
        vk::CommandBufferAllocateInfo{
          .commandPool = *command_pool_,
          .level = vk::CommandBufferLevel::ePrimary,
          .commandBufferCount = 1,
        }
      },
      pipeline_{ std::make_shared<Pipeline>(context_, swapchain_, shader) },
      image_available_{ context_->logical_device().createSemaphore({}) },
      render_complete_{ context_->logical_device().createSemaphore({}) },
      image_in_flight_{
        context_->logical_device().createFence(vk::FenceCreateInfo{
          .flags = vk::FenceCreateFlagBits::eSignaled
        })
      }
    {
      Log::trace("Low Level Renderer ready.");
    }
    
    ~Impl() = default;
    
    void draw()
    {
      if (auto result{ context_->logical_device().waitForFences(*image_in_flight_, true, std::numeric_limits<u64>::max()) };
          result != vk::Result::eSuccess) {
        Log::error("{}", to_string(result));
      }
  
      context_->logical_device().resetFences(*image_in_flight_);
      
      switch (auto [acquire_result, image_index]{
                context_->logical_device().acquireNextImage2KHR(vk::AcquireNextImageInfoKHR{
                  .swapchain = ***swapchain_,
                  .timeout = std::numeric_limits<u64>::max(),
                  .semaphore = *image_available_,
                  .deviceMask = 1,
                })
              }; acquire_result) {
        case vk::Result::eSuccess: {
          for (auto& command_buffer: command_buffers_) {
            command_buffer.reset();
          }
  
          { // Command buffer [0]
            auto& command_buffer{ command_buffers_[0] };
            
            command_buffer.reset();
            record_command_buffer(command_buffer, image_index);
            
            vk::PipelineStageFlags wait_stage_flags{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
            vk::SubmitInfo submit_info{
              .waitSemaphoreCount = 1,
              .pWaitSemaphores = &*image_available_,
              .pWaitDstStageMask = &wait_stage_flags,
              .commandBufferCount = 1,
              .pCommandBuffers = &*command_buffer,
              .signalSemaphoreCount = 1,
              .pSignalSemaphores = &*render_complete_,
            };
            
            context_->graphics_queue().submit(submit_info, *image_in_flight_);
          } // Command buffer [0]
          
          vk::PresentInfoKHR present_info{
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*render_complete_,
            .swapchainCount = 1,
            .pSwapchains = &***swapchain_,
            .pImageIndices = &image_index
          };
          
          if (const auto present_result{ context_->present_queue().presentKHR(present_info) };
              present_result != vk::Result::eSuccess ) {
            Log::error("{}", to_string(present_result));
          }
          break;
        } // case vk::Result::eSuccess
        case vk::Result::eTimeout:
        case vk::Result::eNotReady:
        case vk::Result::eSuboptimalKHR: {
          Log::error("{}", to_string(acquire_result));
          break;
        } // vk::Result::eSuboptimalKHR
        default: {
          // should not happen, as other return codes are considered to be an error and throw an exception
          break;
        }
      }
    }
  
    void record_command_buffer(vk::raii::CommandBuffer& command_buffer, u32 image_index)
    {
      command_buffer.begin(vk::CommandBufferBeginInfo{});
    
      vk::ClearValue clear_value{
        .color = {{{ 0.f, 0.f, 0.f, 1.f }}}
      };
    
      vk::RenderPassBeginInfo render_pass_begin_info{
        .renderPass = **pipeline_->render_pass(),
        .framebuffer = *pipeline_->framebuffers()[image_index],
        .renderArea = {
          .offset = { 0, 0 },
          .extent = swapchain_->extent()
        },
        .clearValueCount = 1,
        .pClearValues = &clear_value,
      };
    
      command_buffer.beginRenderPass2(render_pass_begin_info, vk::SubpassBeginInfo{
        .contents = vk::SubpassContents::eInline
      });
    
      command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, ****pipeline_);
    
      command_buffer.setViewport(0, pipeline_->viewport());
      command_buffer.setScissor(0, pipeline_->scissor());
    
      command_buffer.draw(3, 1, 0, 0);
    
      command_buffer.endRenderPass();
      command_buffer.end();
    }
  
  private:
    shared<ookami::Context> context_;
    shared<Swapchain> swapchain_;
    shared<Pipeline> pipeline_;
    
    vk::raii::CommandPool command_pool_;
    vk::raii::CommandBuffers command_buffers_;
    
    vk::raii::Semaphore image_available_;
    vk::raii::Semaphore render_complete_;
    vk::raii::Fence image_in_flight_;
  };
  
  //
  //  Renderer
  //
  
  LowLevelRenderer::LowLevelRenderer(shared<ookami::Context> context, shared<Shader> shader):
    p_impl_{ std::make_unique<Impl>(std::move(context), std::move(shader)) } {}
  
  LowLevelRenderer::~LowLevelRenderer() = default;
  
  void LowLevelRenderer::draw()
  {
    p_impl_->draw();
  }
  
  void LowLevelRenderer::record_command_buffer(vk::raii::CommandBuffer& command_buffer, u32 image_index)
  {
    p_impl_->record_command_buffer(command_buffer, image_index);
  }
} // fx