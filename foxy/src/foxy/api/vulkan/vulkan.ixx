module;

#include "foxy/internal/foxy_includes.hpp"
#include "foxy/internal/vulkan.hpp"

export module foxy_vulkan_hpp;

import foxy_util;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

export namespace vkm {
  struct Version {
    Version()
      : vulkan_major(0), 
        vulkan_minor(0), 
        vulkan_patch(0) {}
    Version(foxy::u32 version)
      : Version() {
      *this = version;
    }

    Version& operator=(foxy::u32 version) {
      memcpy(this, &version, sizeof(foxy::u32));
      return *this;
    }

    operator foxy::u32() const {
      foxy::u32 result;
      memcpy(&result, this, sizeof(foxy::u32));
      return result;
    }

    bool operator>=(const Version& other) const {
      return (operator foxy::u32()) >= (other.operator foxy::u32());
    }

    std::string toString() const {
      std::stringstream buffer;
      buffer << vulkan_major << "." << vulkan_minor << "." << vulkan_patch;
      return buffer.str();
    }

    const foxy::u32 vulkan_patch : 12;
    const foxy::u32 vulkan_minor : 10;
    const foxy::u32 vulkan_major : 10;
  };

  enum class APIVersion {
    _1_3,
    _1_2,
    _1_1,
    _1_0,
  };

  void export_instance_proc_adrr(PFN_vkGetInstanceProcAddr vk_get_instance_proc_addr) {
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vk_get_instance_proc_addr);
  }
  
  void export_instance_functions(foxy::unique<vk::raii::Instance>& instance) {
    VULKAN_HPP_DEFAULT_DISPATCHER.init(**instance);
  }
  
  void export_device_functions(foxy::unique<vk::raii::Device>& device) {
    VULKAN_HPP_DEFAULT_DISPATCHER.init(**device);
  }

  Version api_version(APIVersion version) {
    switch (version) {
    case APIVersion::_1_3: return Version{ VK_API_VERSION_1_3 };
    case APIVersion::_1_2: return Version{ VK_API_VERSION_1_2 };
    case APIVersion::_1_1: return Version{ VK_API_VERSION_1_1 };
    case APIVersion::_1_0: return Version{ VK_API_VERSION_1_0 };
    default:               return Version{ VK_API_VERSION_1_0 };
    }
  }

  using ::PFN_vkGetInstanceProcAddr;
  using vk::DynamicLoader;
  using vk::ApplicationInfo;
  using vk::InstanceCreateInfo;
  using vk::DeviceCreateInfo;
  using vk::raii::Context;
  using vk::raii::Instance;
  using vk::raii::PhysicalDevices;
  using vk::raii::PhysicalDevice;
  using vk::PhysicalDeviceProperties;
  using vk::PhysicalDeviceFeatures;
  using vk::PhysicalDeviceMemoryProperties;
  using vk::raii::Device;
}