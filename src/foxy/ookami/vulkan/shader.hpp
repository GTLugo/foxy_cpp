//
// Created by galex on 9/8/2022.
//

#pragma once

// TODO: Swap to not precompiling shaders
// What will be done moving forward is shader chaching.
// Compiled shaders will be stored in /tmp/shaders and will be checked for.
// If failed to find cached shader, shader will be recompiled.

namespace vk::raii {
  class ShaderModule;
  class Device;
}

namespace foxy::vulkan {
  class Shader {
    enum Kind {
      Vertex = 0,
      Fragment = 1,
    };

  public:
    explicit Shader(const vk::raii::Device& device, const std::filesystem::path& file_path, bool optimize = false);
    ~Shader();

    [[nodiscard]] auto module(Kind kind) const -> const vk::raii::ShaderModule&;
  private:
    class Impl;
    Unique<Impl> pImpl_;
  };
}