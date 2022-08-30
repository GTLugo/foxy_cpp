module;

#include "foxy/internal/foxy_includes.hpp"

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR 1 // this is a temp fix becasue vk spaceship op is brokey
#include <vulkan/vulkan_raii.hpp>

#ifdef __INTELLISENSE__
#include "../util/util.ixx"
#endif

export module foxy_context;

#ifndef __INTELLISENSE__
export import foxy_util;
#endif

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace foxy {
  struct Version {
    //
    //  Created by Bradley Austin Davis on 2016/03/19
    //
    //  Distributed under the Apache License, Version 2.0.
    //  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
    //
    // Version information for Vulkan is stored in a single 32 bit integer
    // with individual bits representing the major, minor and patch versions.
    // The maximum possible major and minor version is 512 (look out nVidia)
    // while the maximum possible patch version is 2048
    Version()
      : vulkan_major(0)
      , vulkan_minor(0)
      , vulkan_patch(0) {}
    Version(uint32_t version)
      : Version() {
      *this = version;
    }

    Version& operator=(uint32_t version) {
      memcpy(this, &version, sizeof(uint32_t));
      return *this;
    }

    operator uint32_t() const {
      uint32_t result;
      memcpy(&result, this, sizeof(uint32_t));
      return result;
    }

    bool operator >=(const Version& other) const {
      return (operator uint32_t()) >= (other.operator uint32_t());
    }

    std::string toString() const {
      std::stringstream buffer;
      buffer << vulkan_major << "." << vulkan_minor << "." << vulkan_patch;
      return buffer.str();
    }

    const uint32_t vulkan_patch : 12;
    const uint32_t vulkan_minor : 10;
    const uint32_t vulkan_major : 10;
  };

  export class Context {
  public:
    Context() {
      {
        vk::ApplicationInfo app_info{
          "FOXY APP",
          1,
          "FOXY FRAMEWORK",
          1,
          VK_API_VERSION_1_3
        };

        vk::InstanceCreateInfo instance_create_info{
          {},
          &app_info
        };

        // Export function pointers to resolve linker issues with vulkan.hpp
        auto vk_get_instance_proc_addr = dynamic_loader_.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vk_get_instance_proc_addr);

        instance_ = make_unique<vk::raii::Instance>(context_, instance_create_info);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(**instance_);
      } // Cleanup scope

      vk::raii::PhysicalDevices physical_devices_{ *instance_ };
      FOXY_ASSERT(!physical_devices_.empty()) << "NO PHYSICAL VULKAN DEVICES FOUND";
      physical_device_ = make_unique<vk::raii::PhysicalDevice>(physical_devices_[0]); // this is a *move* (devices are not copyable)
      device_properties_ = physical_device_->getProperties();
      std::string device_name = device_properties_.deviceName;
      api_version_ = device_properties_.apiVersion;
      driver_version_ = device_properties_.driverVersion;
      device_features_ = physical_device_->getFeatures();
      device_memory_properties_ = physical_device_->getMemoryProperties();

      vk::DeviceCreateInfo device_create_info{};
      logical_device_ = make_unique<vk::raii::Device>(physical_device_->createDevice(device_create_info));
      VULKAN_HPP_DEFAULT_DISPATCHER.init(**logical_device_);

      FOXY_INFO << "Vulkan Device: " << device_name << " | Device driver version: " << driver_version_.toString() << " | Vulkan API version: " << api_version_.toString();
    }

    ~Context() {

    }

  private:
    vk::DynamicLoader dynamic_loader_{};

    vk::raii::Context context_{};
    unique<vk::raii::Instance> instance_;

    unique<vk::raii::PhysicalDevice> physical_device_;
    vk::PhysicalDeviceProperties device_properties_; // Stores physical device properties (for e.g. checking device limits)
    vk::PhysicalDeviceFeatures device_features_; // Stores phyiscal device features (for e.g. checking if a feature is available)
    vk::PhysicalDeviceMemoryProperties device_memory_properties_; // Stores all available memory (type) properties for the physical device
    Version api_version_;
    Version driver_version_;

    unique<vk::raii::Device> logical_device_;
  };
}