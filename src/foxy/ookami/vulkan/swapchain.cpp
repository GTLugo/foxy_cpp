#include "swapchain.hpp"

#include "foxy/core/window/glfw/glfw.hpp"

namespace foxy::vulkan {
  Swapchain::Swapchain(Shared<vulkan::Context> context)
    : context_{std::move(context)},
      swapchain_{create_swapchain()},
      swap_images_{swapchain_->getImages()},
      swap_image_views_{create_image_views()} {

  }

  Swapchain::~Swapchain() {

  }

  auto Swapchain::pick_swap_surface_format(const std::vector<vk::SurfaceFormatKHR>& formats) -> vk::SurfaceFormatKHR {
    for (auto format: formats) {
      if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
        return format;
      }
    }

    return formats[0];
  }

  auto Swapchain::pick_swap_present_mode(const std::vector<vk::PresentModeKHR>& modes) -> vk::PresentModeKHR {
    std::vector<vk::PresentModeKHR> ordered_preferences{
        vk::PresentModeKHR::eMailbox,
        vk::PresentModeKHR::eImmediate,
    };

    for (const auto& preference: ordered_preferences) {
      for (auto mode: modes) {
        if (mode == preference) {
          return mode;
        }
      }
    }

    return vk::PresentModeKHR::eFifo;
  }

  auto Swapchain::pick_swap_extent(const vk::SurfaceCapabilitiesKHR& capabilities) -> vk::Extent2D {
    if (capabilities.currentExtent.width != std::numeric_limits<u32>::max()) {
      return capabilities.currentExtent;
    } else {
      ivec2 size{};
      glfwGetFramebufferSize(context_->window().get(), &size.x, &size.y);

      vk::Extent2D true_extent{
          std::clamp(static_cast<u32>(size.x), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
          std::clamp(static_cast<u32>(size.y), capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
      };

      return true_extent;
    }
  }

  auto Swapchain::create_swapchain() -> Shared<vk::raii::SwapchainKHR> {
    auto swapchain_support_info = context_->query_swapchain_support();
    SwapchainInfo swapchain_info{
      .format = pick_swap_surface_format(swapchain_support_info.formats),
      .present_mode = pick_swap_present_mode(swapchain_support_info.present_modes),
      .extent = swapchain_extent_ = pick_swap_extent(swapchain_support_info.capabilities),
    };
    swapchain_image_format_ = swapchain_info.format.format;

    u32 image_count{ std::clamp(
      swapchain_support_info.capabilities.minImageCount + 1,
      swapchain_support_info.capabilities.minImageCount,
      swapchain_support_info.capabilities.maxImageCount
    ) };

    const QueueFamilyIndices& indices{ context_->queue_families() };
    std::vector<u32> queue_family_indices{
        indices.graphics.value(),
        indices.present.value(),
    };

    vk::SwapchainCreateInfoKHR swapchain_create_info{
        .surface = **context_->surface(),
        .minImageCount = image_count,
        .imageFormat = swapchain_info.format.format,
        .imageColorSpace = swapchain_info.format.colorSpace,
        .imageExtent = swapchain_info.extent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .preTransform = swapchain_support_info.capabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = swapchain_info.present_mode,
        .clipped = true,
        .oldSwapchain = nullptr,
    };

    if (indices.graphics != indices.present) {
      swapchain_create_info.imageSharingMode = vk::SharingMode::eConcurrent;
      swapchain_create_info.queueFamilyIndexCount = 2;
      swapchain_create_info.pQueueFamilyIndices = queue_family_indices.data();
    } else {
      swapchain_create_info.imageSharingMode = vk::SharingMode::eExclusive;
      swapchain_create_info.queueFamilyIndexCount = 0;
      swapchain_create_info.pQueueFamilyIndices = nullptr;
    }

    try {
      return std::make_shared<vk::raii::SwapchainKHR>(context_->logical_device()->createSwapchainKHR(swapchain_create_info));
    } catch (const std::exception& e) {
      FOXY_FATAL << "Failed to create swapchain.";
      return nullptr;
    }
  }

  auto Swapchain::create_image_views() -> std::vector<vk::raii::ImageView> {
    std::vector<vk::raii::ImageView> image_views;
    std::ranges::transform(
      swap_images_,
      std::back_inserter(image_views),
      [&](VkImage& image) {
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
        return vk::raii::ImageView{*context_->logical_device(), create_info};
      }
    );
    return image_views;
  }
}