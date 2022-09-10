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
}

namespace foxy::vulkan {
  class Shader {
    enum Type {
      VERTEX = 0,
      FRAGMENT = 1,
    };

  public:
    Shader(const std::optional<std::filesystem::path>& vertex_path,
           const std::optional<std::filesystem::path>& fragment_path);
    ~Shader();

    [[nodiscard]] auto bytecode(Type type) const -> const std::vector<char>&;
    [[nodiscard]] auto module(Type type) const -> const vk::raii::ShaderModule&;
  private:
    std::unordered_map<Type, std::vector<char>> shader_code_;
    std::unordered_map<Type, vk::raii::ShaderModule> shader_modules_;

    [[nodiscard]] static auto read_file(const std::filesystem::path& file_path) -> std::vector<char>;
  };
}
