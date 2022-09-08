//
// Created by galex on 9/8/2022.
//

#pragma once

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

    [[nodiscard]] auto bytecode(Type type) const -> const std::vector<char>& { return shader_code_.at(type); }
  private:
    std::unordered_map<Type, std::vector<char>> shader_code_;

    [[nodiscard]] static auto read_file(const std::filesystem::path& file_path) -> std::vector<char>;
  };
}
