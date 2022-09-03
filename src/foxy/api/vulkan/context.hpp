#pragma once

namespace vk {
  namespace raii {
    class Context;
    class Instance;
    class Instance;
    class PhysicalDevice;
    class Device;
  }

  class PhysicalDeviceProperties;
  class PhysicalDeviceFeatures;
  class PhysicalDeviceMemoryProperties;
}

namespace foxy {
  class Version;
}

namespace foxy::vulkan {
  class Context {
    using VulkanContext = vk::raii::Context;
    using Instance = vk::raii::Instance;
    using PhysicalDevice = vk::raii::PhysicalDevice;
    using LogicalDevice = vk::raii::Device;

    using DeviceProperties = vk::PhysicalDeviceProperties;
    using DeviceFeatures = vk::PhysicalDeviceFeatures;
    using DeviceMemoryProperties = vk::PhysicalDeviceMemoryProperties;

  public:
    Context();
    ~Context();

    auto instance() -> Unique<Instance>&;
    auto device() -> Unique<LogicalDevice>&;

  private:
    Unique<VulkanContext> context_;
    Unique<Instance> instance_;

    Unique<PhysicalDevice> physical_device_;
    Unique<LogicalDevice> logical_device_;
    Unique<Version> api_version_;
    Unique<Version> driver_version_;

    Unique<DeviceProperties> device_properties_; // Stores physical device properties (for e.g. checking device limits)
    Unique<DeviceFeatures> device_features_; // Stores phyiscal device features (for e.g. checking if a feature is available)
    Unique<DeviceMemoryProperties> device_memory_properties_; // Stores all available memory (type) properties for the physical device

  };
}