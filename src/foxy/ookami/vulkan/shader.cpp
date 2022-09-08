//
// Created by galex on 9/8/2022.
//

#include "shader.hpp"

#include "foxy/ookami/vulkan/vulkan.hpp"

namespace foxy::vulkan {

  Shader::Shader(const std::optional<std::filesystem::path>& vertex_path,
                 const std::optional<std::filesystem::path>& fragment_path) {
    if (vertex_path.has_value()) {
      shader_code_.insert({Type::VERTEX, read_file(vertex_path.value())});
    }

    if (fragment_path.has_value()) {
      shader_code_.insert({Type::VERTEX, read_file(fragment_path.value())});
    }
  }

  Shader::~Shader() = default;

  auto Shader::read_file(const std::filesystem::path& file_path) -> std::vector<char> {
    std::ifstream file{file_path, std::ios::ate | std::ios::binary};
    if (!file.is_open()) {
      FOXY_ERROR << "File \"" << file_path << "\" does not exist.";
    }

    size_t file_size{ static_cast<size_t>(file.tellg()) };
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    return buffer;
  }
}