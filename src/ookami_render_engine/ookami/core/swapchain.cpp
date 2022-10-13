#include "swapchain.hpp"

#include "context.hpp"

#include "vulkan/static.hpp"
#include <GLFW/glfw3.h>

namespace fx {
  class Swapchain::Impl {
  public:
    explicit Impl(const shared<ookami::Context>& context):
      window_{ context->window() },
      context_{ context },
      swapchain_image_format_{ vk::Format::eB8G8R8A8Unorm },
      swapchain_{create_swapchain()},
      swap_images_{swapchain_.getImages()},
      swap_image_views_{create_image_views()},
      render_pass_{ create_render_pass() }
    {
      for (const auto& image_view: swap_image_views_) {
        framebuffers_.emplace_back(
          context_->logical_device(),
          vk::FramebufferCreateInfo{
            .renderPass = **render_pass_,
            .attachmentCount = 1,
            .pAttachments = &*image_view,
            .width = swapchain_extent_.width,
            .height = swapchain_extent_.height,
            .layers = 1,
          }
        );
      }
      
      Log::trace("Created Vulkan swapchain.");
    }

    ~Impl() = default;
  
    [[nodiscard]] auto dirty() const -> bool
    {
      return false;
    }
  
    void rebuild()
    {
      Log::trace("Rebuilding Vulkan swapchain...");
      dirty_ = true;
      
      ivec2 size{};
      glfwGetFramebufferSize(window_.get(), &size.x, &size.y);
      if (size.x == 0 || size.y == 0) {
        return;
      }
      
      context_->logical_device().waitIdle();
      
      // Reset
      swapchain_.clear();
      swap_images_.clear();
      swap_image_views_.clear();
      render_pass_->clear();
      framebuffers_.clear();
  
      // Rebuild
      swapchain_ = create_swapchain();
      swap_images_ = swapchain_.getImages();
      swap_image_views_ = create_image_views();
      render_pass_ = create_render_pass();
      for (const auto& image_view: swap_image_views_) {
        framebuffers_.emplace_back(
          context_->logical_device(),
          vk::FramebufferCreateInfo{
            .renderPass = **render_pass_,
            .attachmentCount = 1,
            .pAttachments = &*image_view,
            .width = swapchain_extent_.width,
            .height = swapchain_extent_.height,
            .layers = 1,
          }
        );
      }
    
      Log::trace("Rebuilt Vulkan swapchain.");
      dirty_ = false;
    }
  
    auto acquire_next_image(const vk::raii::Semaphore& semaphore) -> std::optional<u32>
    {
      try {
        auto [acquire_result, image_index]{ swapchain_.acquireNextImage(std::numeric_limits<u64>::max(), *semaphore) };
        return image_index;
      } catch (const vk::OutOfDateKHRError& e) {
        // recreate swapchain and try drawing in the next frame (make sure not to draw THIS frame!)
        rebuild();
      } catch (const std::exception& e) {
        Log::error(e.what());
      }
      return std::nullopt;
    }
  
    [[nodiscard]] auto context() const -> const shared<ookami::Context>&
    {
      return context_;
    }
    
    [[nodiscard]] auto format() -> vk::Format {
      return swapchain_image_format_;
    }

    [[nodiscard]] auto extent() -> vk::Extent2D {
      return swapchain_extent_;
    }

    [[nodiscard]] auto image_views() -> std::vector<vk::raii::ImageView>& {
      return swap_image_views_;
    }
  
    [[nodiscard]] auto render_pass() const -> const shared<vk::raii::RenderPass>&
    {
      return render_pass_;
    }
  
    [[nodiscard]] auto framebuffers() -> std::vector<vk::raii::Framebuffer>&
    {
      return framebuffers_;
    }
  
    auto operator*() -> vk::raii::SwapchainKHR&
    {
      return swapchain_;
    }
    
  private:
    bool dirty_{ false };
    
    shared<GLFWwindow> window_;
    shared<ookami::Context> context_;

    vk::Format swapchain_image_format_;
    vk::Extent2D swapchain_extent_;
    vk::raii::SwapchainKHR swapchain_;

    std::vector<VkImage> swap_images_;
    std::vector<vk::raii::ImageView> swap_image_views_;
    
    shared<vk::raii::RenderPass> render_pass_;
    std::vector<vk::raii::Framebuffer> framebuffers_;

    [[nodiscard]] auto pick_swap_surface_format(const std::vector<vk::SurfaceFormatKHR>& formats) const -> vk::SurfaceFormatKHR {
      for (auto& format: formats) {
        if (format.format == swapchain_image_format_ && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
          return format;
        }
      }

      return formats[0];
    }

    [[nodiscard]] static auto pick_swap_present_mode(const std::vector<vk::PresentModeKHR>& modes) -> vk::PresentModeKHR {
      for (const std::vector ordered_preferences{
             vk::PresentModeKHR::eMailbox,
             vk::PresentModeKHR::eImmediate,
           }; const auto& preference: ordered_preferences) {
        for (const auto mode: modes) {
          if (mode == preference) {
            return mode;
          }
        }
      }

      return vk::PresentModeKHR::eFifo;
    }

    [[nodiscard]] auto pick_swap_extent(const vk::SurfaceCapabilitiesKHR& capabilities) const -> vk::Extent2D {
      if (capabilities.currentExtent.width != std::numeric_limits<u32>::max()) {
        return capabilities.currentExtent;
      }

      ivec2 size{};
      glfwGetFramebufferSize(window_.get(), &size.x, &size.y);

      const vk::Extent2D true_extent{
        std::clamp(static_cast<u32>(size.x), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp(static_cast<u32>(size.y), capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
      };

      return true_extent;
    }

    [[nodiscard]] auto create_swapchain() -> vk::raii::SwapchainKHR {
      auto [capabilities, formats, present_modes]{ context_->query_swapchain_support() };
      SwapchainInfo swapchain_info{
        .format = pick_swap_surface_format(formats),
        .present_mode = pick_swap_present_mode(present_modes),
        .extent = swapchain_extent_ = pick_swap_extent(capabilities),
      };
      swapchain_image_format_ = swapchain_info.format.format;

      u32 image_count{ std::clamp(
        capabilities.minImageCount + 1,
        capabilities.minImageCount,
        capabilities.maxImageCount
      ) };

      const auto& [graphics, present]{ context_->queue_families() };
      const std::vector queue_family_indices{
        graphics.value(),
        present.value(),
      };

      vk::SwapchainCreateInfoKHR swapchain_create_info{
        .surface = *context_->surface(),
        .minImageCount = image_count,
        .imageFormat = swapchain_info.format.format,
        .imageColorSpace = swapchain_info.format.colorSpace,
        .imageExtent = swapchain_info.extent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = swapchain_info.present_mode,
        .clipped = true,
        .oldSwapchain = nullptr,
      };

      if (graphics != present) {
        swapchain_create_info.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queue_family_indices.data();
      } else {
        swapchain_create_info.imageSharingMode = vk::SharingMode::eExclusive;
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices = nullptr;
      }

      try {
        return { context_->logical_device().createSwapchainKHR(swapchain_create_info) };
      } catch (const std::exception& e) {
        Log::fatal("Failed to create swapchain: ", e.what());
        return nullptr;
      }
    }

    [[nodiscard]] auto create_image_views() -> std::vector<vk::raii::ImageView> {
      std::vector<vk::raii::ImageView> image_views;
      std::ranges::transform(
        swap_images_,
        std::back_inserter(image_views),
        [&](VkImage& image) -> vk::raii::ImageView {
          vk::ImageViewCreateInfo create_info{
            .image = image,
            .viewType = vk::ImageViewType::e2D,
            .format = swapchain_image_format_,
            .components = {
              .r = vk::ComponentSwizzle::eIdentity,
              .g = vk::ComponentSwizzle::eIdentity,
              .b = vk::ComponentSwizzle::eIdentity,
              .a = vk::ComponentSwizzle::eIdentity,
            },
            .subresourceRange = {
              .aspectMask = vk::ImageAspectFlagBits::eColor,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
            }
          };
          return { context_->logical_device(), create_info };
        }
      );
      return image_views;
    }
  
    [[nodiscard]] auto create_render_pass() const -> unique<vk::raii::RenderPass> {
      vk::AttachmentDescription color_attachment{
        .format = swapchain_image_format_,
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
    
      vk::SubpassDependency dependency{
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .srcAccessMask = vk::AccessFlagBits::eNone,
        .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
      };
    
      try {
        return std::make_unique<vk::raii::RenderPass>(
          context_->logical_device(),
          vk::RenderPassCreateInfo{
            .attachmentCount = 1,
            .pAttachments = &color_attachment,
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = 1,
            .pDependencies = &dependency,
          }
        );
      } catch (const std::exception& e) {
        Log::fatal("Failed to create render pass: {}", e.what());
        return nullptr;
      }
    }
  };

  //
  //  Swapchain
  //

  Swapchain::Swapchain(const shared<ookami::Context>& context)
    : p_impl_{std::make_unique<Impl>(context)} {}

  Swapchain::~Swapchain() = default;
  
  auto Swapchain::dirty() const -> bool
  {
    return p_impl_->dirty();
  }
  
  void Swapchain::rebuild()
  {
    p_impl_->rebuild();
  }
  
  auto Swapchain::context() const -> const shared<ookami::Context>&
  {
    return p_impl_->context();
  }

  auto Swapchain::format() const -> vk::Format {
    return p_impl_->format();
  }

  auto Swapchain::extent() const -> vk::Extent2D {
    return p_impl_->extent();
  }

  auto Swapchain::image_views() const -> std::vector<vk::raii::ImageView>& {
    return p_impl_->image_views();
  }
  
  auto Swapchain::render_pass() const -> const shared<vk::raii::RenderPass>&
  {
    return p_impl_->render_pass();
  }
  
  auto Swapchain::framebuffers() -> std::vector<vk::raii::Framebuffer>&
  {
    return p_impl_->framebuffers();
  }
  
  auto Swapchain::operator*() -> vk::raii::SwapchainKHR&
  {
    return **p_impl_;
  }
  
  auto Swapchain::acquire_next_image(const vk::raii::Semaphore& semaphore) -> std::optional<u32>
  {
    return p_impl_->acquire_next_image(semaphore);
  }
}
