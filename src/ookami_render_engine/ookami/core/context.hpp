#pragma once

class GLFWwindow;

namespace vk::raii {
  class Context;
  class Instance;
  class PhysicalDevice;
  class Device;
  class SurfaceKHR;
  class PhysicalDevice;
  class Queue;
  class Fence;
}

namespace fx {
  class SwapchainSupportInfo;
  class QueueFamilyIndices;
  class Shader;
  class ShaderCreateInfo;

  namespace ookami {
    auto required_instance_extensions_strings() -> std::vector<std::string>;
    auto required_instance_extensions() -> std::vector<const char*>;

    class Context {
      using VulkanContext = vk::raii::Context;
      using Instance = vk::raii::Instance;
      using PhysicalDevice = vk::raii::PhysicalDevice;
      using LogicalDevice = vk::raii::Device;
      using Surface = vk::raii::SurfaceKHR;

    public:
      explicit Context(
        const shared<GLFWwindow>& window,
      #ifdef FOXY_DEBUG_MODE
        bool enable_validation = true
      #else
        bool enable_validation = false
      #endif
      );
      ~Context();
  
      void wait_for_fence(const vk::raii::Fence& fence);
      void reset_fence(const vk::raii::Fence& fence);
      
      [[nodiscard]] auto window() -> shared<GLFWwindow>;
      [[nodiscard]] auto native() -> VulkanContext&;
      [[nodiscard]] auto instance() -> Instance&;
      [[nodiscard]] auto surface() -> Surface&;
      [[nodiscard]] auto graphics_queue() -> vk::raii::Queue&;
      [[nodiscard]] auto present_queue() -> vk::raii::Queue&;
      [[nodiscard]] auto query_swapchain_support() const -> SwapchainSupportInfo;
      [[nodiscard]] auto queue_families() const -> const QueueFamilyIndices&;
      [[nodiscard]] auto physical_device() -> PhysicalDevice&;
      [[nodiscard]] auto logical_device() -> LogicalDevice&;
      
      [[nodiscard]] auto create_shader(const ShaderCreateInfo& shader_create_info) -> unique<Shader>;
  
      auto operator*() -> VulkanContext&;

    private:
      class Impl;
      unique<Impl> p_impl_;
    };
  }
}

