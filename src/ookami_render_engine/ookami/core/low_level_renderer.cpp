//
// Created by galex on 10/2/2022.
//

#include "low_level_renderer.hpp"

#include "context.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "shader.hpp"

#include "vulkan/static.hpp"
#include <inferno/window.hpp>

namespace fx {
  class LowLevelRenderer::Impl: types::SingleInstance<LowLevelRenderer> {
  public:
    explicit Impl(
      const shared<Window>& window,
      const shared<ookami::Context>& context,
      const shared<Shader>& shader,
      const u32 max_frames_in_flight
    ):
      max_frames_in_flight_{ max_frames_in_flight },
      window_{ window },
      context_{ context },
      swapchain_{ std::make_shared<Swapchain>(context_) },
      pipeline_{ std::make_shared<Pipeline>(context_, swapchain_, shader) },
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
          .commandBufferCount = max_frames_in_flight,
        }
      }
    {
      Log::trace("Preparing Low Level Renderer...");
      
      for ([[maybe_unused]] u32 i: std::views::iota(0U, max_frames_in_flight_)) {
        try {
          image_available_semaphores_.emplace_back(context_->logical_device(), vk::SemaphoreCreateInfo{});
          render_complete_semaphores_.emplace_back(context_->logical_device(), vk::SemaphoreCreateInfo{});
          image_in_flight_fences_.emplace_back(
            context_->logical_device().createFence(vk::FenceCreateInfo{
              .flags = vk::FenceCreateFlagBits::eSignaled
            })
          );
        } catch (const std::exception& e) {
          Log::error(e.what());
        }
      }
      
      window_->add_framebuffer_resized_callback([this](i32, i32) {
        framebuffer_resized_ = true;
      });
      
      Log::trace("Low Level Renderer ready.");
    }
    
    ~Impl() = default;
    
    void draw()
    {
      context_->wait_for_fence(image_in_flight_fences_[current_frame_index_]);
      if (const auto image_index{ swapchain_->acquire_next_image(image_available_semaphores_[current_frame_index_]) }) {
        context_->reset_fence(image_in_flight_fences_[current_frame_index_]);
        submit(*image_index);
        present(*image_index);
        current_frame_index_ = (current_frame_index_ + 1) % max_frames_in_flight_;
      }
    }
  
    void record_command_buffer(const vk::raii::CommandBuffer& command_buffer, const u32 image_index) const
    {
      command_buffer.begin(vk::CommandBufferBeginInfo{});
    
      vk::ClearValue clear_value{
        .color = {{{ 0.f, 0.f, 0.f, 1.f }}}
      };

      const vk::RenderPassBeginInfo render_pass_begin_info{
        .renderPass = **swapchain_->render_pass(),
        .framebuffer = *swapchain_->framebuffers()[image_index],
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
    const u32 max_frames_in_flight_;
    u32 current_frame_index_{ 0 };
    bool framebuffer_resized_{ false };
  
    shared<Window> window_;
    shared<ookami::Context> context_;
    
    shared<Swapchain> swapchain_;
    shared<Pipeline> pipeline_;
    
    vk::raii::CommandPool command_pool_;
    vk::raii::CommandBuffers command_buffers_;
    
    std::vector<vk::raii::Semaphore> image_available_semaphores_;
    std::vector<vk::raii::Semaphore> render_complete_semaphores_;
    std::vector<vk::raii::Fence> image_in_flight_fences_;
    
    void submit(const u32 image_index) const
    {
      const auto& command_buffer{ command_buffers_[current_frame_index_] };
  
      command_buffer.reset();
      record_command_buffer(command_buffer, image_index);
  
      vk::PipelineStageFlags wait_stage_flags{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
      vk::SubmitInfo submit_info{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*image_available_semaphores_[current_frame_index_],
        .pWaitDstStageMask = &wait_stage_flags,
        .commandBufferCount = 1,
        .pCommandBuffers = &*command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &*render_complete_semaphores_[current_frame_index_],
      };
  
      context_->graphics_queue().submit(submit_info, *image_in_flight_fences_[current_frame_index_]);
    }
    
    void present(u32 image_index)
    {
      const vk::PresentInfoKHR present_info{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*render_complete_semaphores_[current_frame_index_],
        .swapchainCount = 1,
        .pSwapchains = &***swapchain_,
        .pImageIndices = &image_index
      };
  
      try {
        if (!swapchain_->dirty()) {
          [[maybe_unused]] auto present_result{ context_->present_queue().presentKHR(present_info) };
        } else {
          swapchain_->rebuild();
        }
    
        if (framebuffer_resized_) {
          // recreate swapchain and try drawing in the next frame (make sure not to draw THIS frame!)
          framebuffer_resized_ = false;
          swapchain_->rebuild();
        }
      } catch (const vk::OutOfDateKHRError& e) {
        swapchain_->rebuild();
      } catch (const vk::SurfaceLostKHRError& e) {
        swapchain_->rebuild();
      } catch (const std::exception& e) {
        Log::error("Presentation failure. {}", e.what());
      }
    }
  };
  
  //
  //  Renderer
  //
  
  LowLevelRenderer::LowLevelRenderer(
    const shared<Window>& window,
    const shared<ookami::Context>& context,
    const shared<Shader>& shader,
    const u32 max_frames_in_flight
  ):
    p_impl_{ std::make_unique<Impl>(window, context, shader, max_frames_in_flight) } {}
  
  LowLevelRenderer::~LowLevelRenderer() = default;
  
  void LowLevelRenderer::draw() const
  {
    p_impl_->draw();
  }
  
  void LowLevelRenderer::record_command_buffer(const vk::raii::CommandBuffer& command_buffer, const u32 image_index) const
  {
    p_impl_->record_command_buffer(command_buffer, image_index);
  }
} // fx