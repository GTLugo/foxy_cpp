//
// Created by galex on 9/1/2022.
//

#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

namespace foxy::vulkan {
  struct ExtensionData {
    std::vector<const char*> window_extensions;
    std::vector<VkExtensionProperties> instance_extensions;
    std::vector<const char*> enabled_extensions;
    std::vector<const char*> device_extensions;
  };

  struct QueueFamilyIndices {
    std::optional<u32> graphics;
    std::optional<u32> present;

    [[nodiscard]] auto complete() const -> bool {
      return graphics.has_value() && present.has_value();
    }
  };

  struct SwapchainSupportInfo {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> present_modes;
  };
}