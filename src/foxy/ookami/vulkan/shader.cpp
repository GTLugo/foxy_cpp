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
      FOXY_TRACE << "Read shader: " << vertex_path.value().filename();
    }

    if (fragment_path.has_value()) {
      shader_code_.insert({Type::FRAGMENT, read_file(fragment_path.value())});
      FOXY_TRACE << "Read shader: " << fragment_path.value().filename();
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
    file.read(buffer.data(), static_cast<std::streamsize>(file_size));

    return buffer;
  }

  auto Shader::module(Shader::Type type) const -> const vk::raii::ShaderModule& { return shader_modules_.at(type); }

  auto Shader::bytecode(Shader::Type type) const -> const std::vector<char>& { return shader_code_.at(type); }
}