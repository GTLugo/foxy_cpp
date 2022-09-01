module;

#include "foxy/internal/foxy_includes.hpp"

export module foxy_vulkan_context;

import foxy_util;
import foxy_vulkan_hpp;

using namespace vkm;

namespace foxy {
  export class Context {
  public:
    Context() {
      {
        vkm::ApplicationInfo app_info{
          .pApplicationName = "FOXY APP",
          .pEngineName = "FOXY FRAMEWORK",
          .apiVersion = (u32)api_version(vkm::APIVersion::_1_3)
        };
        
        vkm::InstanceCreateInfo instance_create_info{
          .pApplicationInfo = &app_info
        };

        // Export function pointers to resolve linker issues with vulkan.hpp
        auto vk_get_instance_proc_addr = dynamic_loader_.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        vkm::export_instance_proc_adrr(vk_get_instance_proc_addr);

        instance_ = make_unique<vkm::Instance>(context_, instance_create_info);
        vkm::export_instance_functions(instance_);
      } // Cleanup scope

      vkm::PhysicalDevices physical_devices_{ *instance_ };
      FOXY_ASSERT(!physical_devices_.empty()) << "NO PHYSICAL VULKAN DEVICES FOUND";
      physical_device_ = make_unique<vkm::PhysicalDevice>(physical_devices_[0]); // this is a *move* (devices are not copyable)
      device_properties_ = physical_device_->getProperties();
      std::string device_name = device_properties_.deviceName;
      api_version_ = device_properties_.apiVersion;
      driver_version_ = device_properties_.driverVersion;
      device_features_ = physical_device_->getFeatures();
      device_memory_properties_ = physical_device_->getMemoryProperties();

      vkm::DeviceCreateInfo device_create_info{};
      logical_device_ = make_unique<vkm::Device>(physical_device_->createDevice(device_create_info, nullptr));
      vkm::export_device_functions(logical_device_);

      FOXY_INFO << "Vulkan Device: " << device_name << " | Device driver version: " << driver_version_.toString() << " | Vulkan API version: " << api_version_.toString();
    }

    ~Context() {

    }

  private:
    vkm::DynamicLoader dynamic_loader_{ "" };

    vkm::Context context_{};
    unique<vkm::Instance> instance_;

    unique<vkm::PhysicalDevice> physical_device_;
    vkm::PhysicalDeviceProperties device_properties_; // Stores physical device properties (for e.g. checking device limits)
    vkm::PhysicalDeviceFeatures device_features_; // Stores phyiscal device features (for e.g. checking if a feature is available)
    vkm::PhysicalDeviceMemoryProperties device_memory_properties_; // Stores all available memory (type) properties for the physical device
    Version api_version_;
    Version driver_version_;

    unique<vkm::Device> logical_device_;
  };
}