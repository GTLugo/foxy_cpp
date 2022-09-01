#pragma once

#include "foxy/internal/includes.hpp"

#include "foxy/api/vulkan/version.hpp"
#include "foxy/api/vulkan/vulkan.hpp"

namespace foxy {
  class Context {
  public:
    Context() {
      {
        vk::ApplicationInfo app_info{
          .pApplicationName = "FOXY APP",
          .pEngineName = "FOXY FRAMEWORK",
          .apiVersion = VK_API_VERSION_1_3
        };

        vk::InstanceCreateInfo instance_create_info{
          .pApplicationInfo = &app_info
        };

        instance_ = std::make_unique<vk::raii::Instance>(context_, instance_create_info);
      } // Cleanup scope

      vk::raii::PhysicalDevices physical_devices_{ *instance_ };
      FOXY_ASSERT(!physical_devices_.empty()) << "NO PHYSICAL VULKAN DEVICES FOUND";
      physical_device_ = std::make_unique<vk::raii::PhysicalDevice>(physical_devices_[0]); // this is a *move* (devices are not copyable)
      device_properties_ = physical_device_->getProperties();
      std::string device_name = device_properties_.deviceName;
      api_version_ = device_properties_.apiVersion;
      driver_version_ = device_properties_.driverVersion;
      device_features_ = physical_device_->getFeatures();
      device_memory_properties_ = physical_device_->getMemoryProperties();

      vk::DeviceCreateInfo device_create_info{};
      logical_device_ = std::make_unique<vk::raii::Device>(physical_device_->createDevice(device_create_info, nullptr));

      FOXY_INFO << "Vulkan Device: " << device_name << " | Device driver version: " << driver_version_.to_string() << " | Vulkan API version: " << api_version_.to_string();
    }

    ~Context() = default;

  private:
    //vk::DynamicLoader dynamic_loader_{ "" };

    vk::raii::Context context_{};
    Unique<vk::raii::Instance> instance_;

    Unique<vk::raii::PhysicalDevice> physical_device_;
    vk::PhysicalDeviceProperties device_properties_; // Stores physical device properties (for e.g. checking device limits)
    vk::PhysicalDeviceFeatures device_features_; // Stores phyiscal device features (for e.g. checking if a feature is available)
    vk::PhysicalDeviceMemoryProperties device_memory_properties_; // Stores all available memory (type) properties for the physical device
    Version api_version_;
    Version driver_version_;

    Unique<vk::raii::Device> logical_device_;
  };
}