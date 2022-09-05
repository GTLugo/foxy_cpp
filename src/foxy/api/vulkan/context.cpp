#include "context.hpp"

#include "foxy/api/vulkan/version.hpp"
#include "foxy/api/vulkan/vulkan.hpp"
#define FOXY_GLFW_INCLUDE_VULKAN
#include "foxy/api/glfw/glfw.hpp"

namespace foxy::vulkan {
  Context::Context(glfw::UniqueWindow& window) {
    context_ = std::make_unique<vk::raii::Context>();

    { // vk::raii::Instance
      vk::ApplicationInfo app_info{
          .pApplicationName = "FOXY APP",
          .pEngineName = "FOXY FRAMEWORK",
          .apiVersion = VK_API_VERSION_1_3
      };

      vk::InstanceCreateInfo instance_create_info{
          .pApplicationInfo = &app_info
      };

      instance_ = std::make_unique<vk::raii::Instance>(*context_, instance_create_info);
    } // vk::raii::Instance cleanup scope

    vk::raii::PhysicalDevices physical_devices_{*instance_};
    FOXY_ASSERT(!physical_devices_.empty()) << "NO PHYSICAL VULKAN DEVICES FOUND";
    // this is a *move* (devices are not copyable)
    physical_device_ = std::make_unique<vk::raii::PhysicalDevice>(physical_devices_[0]);
    device_properties_ = std::make_unique<vk::PhysicalDeviceProperties>(physical_device_->getProperties());
    std::string device_name = device_properties_->deviceName;
    api_version_ = std::make_unique<Version>(device_properties_->apiVersion);
    driver_version_ = std::make_unique<Version>(device_properties_->driverVersion);
    device_features_ = std::make_unique<vk::PhysicalDeviceFeatures>(physical_device_->getFeatures());
    device_memory_properties_ = std::make_unique<vk::PhysicalDeviceMemoryProperties>(physical_device_->getMemoryProperties());

    vk::DeviceCreateInfo device_create_info{};
    logical_device_ = std::make_unique<vk::raii::Device>(physical_device_->createDevice(device_create_info, nullptr));

    {
      VkSurfaceKHR raw_surface;
      auto result = static_cast<vk::Result>(
          glfwCreateWindowSurface(static_cast<VkInstance>(**instance_), window.get(), nullptr, &raw_surface)
      );
      auto surface = vk::createResultValueType(result, raw_surface);
      surface_ = std::make_unique<vk::raii::SurfaceKHR>(*instance_, surface);
    }

    FOXY_INFO << "Vulkan Device: " << device_name << " | Device driver version: " << driver_version_->to_string()
              << " | Vulkan API version: " << api_version_->to_string();

  }

  Context::~Context() = default;

  auto Context::instance() -> Unique<vk::raii::Instance>& {
    return instance_;
  }

  auto Context::physical_device() -> Unique <vk::raii::PhysicalDevice>& {
    return physical_device_;
  }

  auto Context::logical_device() -> Unique<vk::raii::Device>& {
    return logical_device_;
  }

  auto Context::native() -> Unique<Context::VulkanContext>& {
    return context_;
  }
}