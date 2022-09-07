//
// Created by galex on 9/1/2022.
//

#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

namespace foxy::vulkan {
  struct DeviceCreateInfo: public vk::DeviceCreateInfo {
    std::vector<vk::DeviceQueueCreateInfo> device_queues;
    std::vector<std::vector<float>> device_queues_priorities;

    auto add_queue_family(u32 queue_family_index, vk::ArrayProxy<float> priorities) {
      device_queues.push_back(vk::DeviceQueueCreateInfo{
        .queueFamilyIndex = queue_family_index
      });
      std::vector<float> priorities_vector;
      priorities_vector.resize(priorities.size());
      memcpy(priorities_vector.data(), priorities.data(), sizeof(float) * priorities.size());
      device_queues_priorities.push_back(priorities_vector);
    }
    auto add_queue_family(u32 queue_family_index, size_t count = 1) {
      std::vector<float> priorities;
      priorities.resize(count);
      std::fill(priorities.begin(), priorities.end(), 0.0f);
      add_queue_family(queue_family_index, priorities);
    }

    void update() {
      DCHECK(device_queues_priorities.size() == device_queues.size()) << "Device queue priority size error";
      auto size = device_queues.size();
      for (auto i = 0; i < size; ++i) {
        auto& deviceQueue = device_queues[i];
        auto& deviceQueuePriorities = device_queues_priorities[i];
        deviceQueue.queueCount = static_cast<uint32_t>(deviceQueuePriorities.size());
        deviceQueue.pQueuePriorities = deviceQueuePriorities.data();
      }

      this->queueCreateInfoCount = static_cast<uint32_t>(device_queues.size());
      this->pQueueCreateInfos = device_queues.data();
    }
  };

  struct Surface {
    using Format = vk::Format;
    using ColorSpace = vk::ColorSpaceKHR;

    Unique<vk::raii::SurfaceKHR> native;
    Unique<Format> color_format;
    Unique<ColorSpace> color_space;
  };

  struct ExtensionData {
    std::vector<const char*> window_extensions;
    std::vector<VkExtensionProperties> instance_extensions;
    std::vector<const char*> enabled_extensions;
  };
}