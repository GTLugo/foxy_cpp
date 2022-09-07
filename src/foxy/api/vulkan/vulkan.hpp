//
// Created by galex on 9/1/2022.
//

#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

namespace foxy::vulkan {
  struct Surface {
    using Format = vk::Format;
    using ColorSpace = vk::ColorSpaceKHR;

    vk::raii::SurfaceKHR native;
    Format color_format;
    ColorSpace color_space;
  };

  struct ExtensionData {
    std::vector<const char*> window_extensions;
    std::vector<VkExtensionProperties> instance_extensions;
    std::vector<const char*> enabled_extensions;
  };

  struct QueueFamilyIndices {
    enum QueueID {
      GRAPHICS = 0,
    };

    std::optional<u32> graphics;
    float graphics_priority = 1.f;

    auto complete() const -> bool {
      return graphics.has_value();
    }
  };
}