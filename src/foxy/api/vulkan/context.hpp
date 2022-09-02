#pragma once

#include "foxy/api/vulkan/version.hpp"
#include "foxy/api/vulkan/vulkan.hpp"

namespace foxy {
  class Context {
    using VulkanContext = vk::raii::Context;
    using Instance = Unique<vk::raii::Instance>;
    using PhysicalDevice = Unique<vk::raii::PhysicalDevice>;
    using LogicalDevice = Unique<vk::raii::Device>;

    using DeviceProperties = vk::PhysicalDeviceProperties;
    using DeviceFeatures = vk::PhysicalDeviceFeatures;
    using DeviceMemoryProperties = vk::PhysicalDeviceMemoryProperties;

  public:
    Context();
    ~Context();

  private:
    VulkanContext context_{};
    Instance instance_;

    PhysicalDevice physical_device_;
    LogicalDevice logical_device_;
    Version api_version_;
    Version driver_version_;

    DeviceProperties device_properties_; // Stores physical device properties (for e.g. checking device limits)
    DeviceFeatures device_features_; // Stores phyiscal device features (for e.g. checking if a feature is available)
    DeviceMemoryProperties device_memory_properties_; // Stores all available memory (type) properties for the physical device

  };
}