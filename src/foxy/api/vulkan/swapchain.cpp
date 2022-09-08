#include "swapchain.hpp"

#include "foxy/api/glfw/glfw.hpp"

namespace foxy::vulkan {
  Swapchain::Swapchain(Shared<vulkan::Context> context)
    : context_{std::move(context)} {
    auto swapchain_support_info = context_->query_swapchain_support();
    surface_format_ = pick_swap_surface_format(swapchain_support_info.formats);
    present_mode_ = pick_swap_present_mode(swapchain_support_info.present_modes);
    extent_ = pick_swap_extent(swapchain_support_info.capabilities);

    image_count_ = std::clamp(
        swapchain_support_info.capabilities.minImageCount + 1,
        swapchain_support_info.capabilities.minImageCount,
        swapchain_support_info.capabilities.maxImageCount
    );

    const QueueFamilyIndices& indices{ context_->queue_families() };
    std::vector<u32> queue_family_indices{
        indices.graphics.value(),
        indices.present.value(),
    };

    vk::SwapchainCreateInfoKHR swapchain_create_info{
      .surface = **context_->surface(),
      .minImageCount = image_count_,
      .imageFormat = surface_format_.format,
      .imageColorSpace = surface_format_.colorSpace,
      .imageExtent = extent_,
      .imageArrayLayers = 1,
      .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
      .preTransform = swapchain_support_info.capabilities.currentTransform,
      .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
      .presentMode = present_mode_,
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
      swapchain_ = std::make_shared<vk::raii::SwapchainKHR>(context_->logical_device()->createSwapchainKHR(swapchain_create_info));
    } catch (const std::exception& e) {
      FOXY_FATAL << "Failed to create swapchain.";
    }
  }

  Swapchain::~Swapchain() {

  }

  auto Swapchain::pick_swap_surface_format(const std::vector<vk::SurfaceFormatKHR>& formats) -> vk::SurfaceFormatKHR {
    for (const auto& format: formats) {
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
      for (const auto& mode: modes) {
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


}