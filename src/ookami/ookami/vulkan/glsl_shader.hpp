//
// Created by galex on 9/8/2022.
//

#pragma once

namespace vk::raii {
  class ShaderModule;
  class Device;
}

namespace ookami {
  class GLSLShader {
  public:
    enum Kind {
      Vertex   = 0,
      Fragment = 1,
      Compute  = 2,
      Geometry = 3,

      Max = Geometry,
    };

    using BitFlags = std::bitset<Kind::Max + 1>;

    explicit GLSLShader(const vk::raii::Device& device,
                    const std::filesystem::path& file_path,
                    BitFlags shader_bits,
                    bool optimize = false);
    ~GLSLShader();

    [[nodiscard]] auto module(Kind kind) const -> const vk::raii::ShaderModule&;
  private:
    class Impl;
    koyote::unique<Impl> pImpl_;
  };
}
