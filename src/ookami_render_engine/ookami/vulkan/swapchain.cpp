#include "swapchain.hpp"

#include "context.hpp"
#include "vulkan.hpp"
#include <GLFW/glfw3.h>

namespace fx {
  class Swapchain::Impl {
  public:
    explicit Impl(fx::shared<GLFWwindow> window, fx::shared<ookami::Context> context)
      : window_{ std::move(window) },
        context_{ std::move(context) },
        swapchain_image_format_{vk::Format::eB8G8R8A8Unorm},
        swapchain_{create_swapchain()},
        swap_images_{swapchain_.getImages()},
        swap_image_views_{create_image_views()} {
      fx::Log::trace("Created Vulkan swapchain.");
    }

    ~Impl() = default;


    auto format() -> vk::Format {
      return swapchain_image_format_;
    }

    auto extent() -> vk::Extent2D {
      return swapchain_extent_;
    }
  private:
    fx::shared<GLFWwindow> window_;
    fx::shared<ookami::Context> context_;

    vk::Format swapchain_image_format_;
    vk::Extent2D swapchain_extent_;
    vk::raii::SwapchainKHR swapchain_;

    std::vector<VkImage> swap_images_;
    std::vector<vk::raii::ImageView> swap_image_views_;

    [[nodiscard]] static auto pick_swap_surface_format(const std::vector<vk::SurfaceFormatKHR>& formats) -> vk::SurfaceFormatKHR {
      for (auto& format : formats) {
        if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
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
      if (capabilities.currentExtent.width != std::numeric_limits<fx::u32>::max()) {
        return capabilities.currentExtent;
      }

      fx::ivec2 size{};
      glfwGetFramebufferSize(window_.get(), &size.x, &size.y);

      const vk::Extent2D true_extent{
        std::clamp(static_cast<fx::u32>(size.x), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp(static_cast<fx::u32>(size.y), capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
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

      fx::u32 image_count{ std::clamp(
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
        fx::Log::fatal("Failed to create swapchain: ", e.what());
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
  };

  //
  //  Swapchain
  //

  Swapchain::Swapchain(const fx::shared<GLFWwindow>& window, const fx::shared<ookami::Context>& context)
    : p_impl_{std::make_unique<Impl>(window, context)} {}

  Swapchain::~Swapchain() = default;

  auto Swapchain::format() const -> vk::Format {
    return p_impl_->format();
  }

  auto Swapchain::extent() const -> vk::Extent2D {
    return p_impl_->extent();
  }
}