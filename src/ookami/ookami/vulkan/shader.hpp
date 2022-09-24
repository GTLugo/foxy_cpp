//
// Created by galex on 9/8/2022.
//

#pragma once

namespace vk::raii {
  class ShaderModule;
  class Device;
}

namespace ookami {
  class Shader {
  public:
    class Stage {
    public:
      enum Value: koyote::u32 {
        Vertex = 0,
        Fragment = 1,
        Compute = 2,
        Geometry = 3,

        Max = Geometry,
      };

      Stage() = default;
      constexpr Stage(const Value value)
        : value_{ value } { }

      constexpr operator Value() const { return value_; }
      explicit operator bool() const = delete;
      constexpr bool operator==(const Stage a) const { return value_ == a.value_; }
      constexpr bool operator!=(const Stage a) const { return value_ != a.value_; }

      [[nodiscard]] static constexpr auto from_string(std::string_view str) -> std::optional<Stage>;
      [[nodiscard]] constexpr auto to_string() const -> std::optional<std::string>;
      [[nodiscard]] constexpr auto to_shaderc() const -> std::optional<koyote::i32>;

      [[nodiscard]] auto underlying_value() const -> Value { return value_; }

    private:
      Value value_;
    };

    using bit_flags = std::bitset<Stage::Max + 1>;

    explicit Shader(const vk::raii::Device& device,
                    const std::filesystem::path& file_path,
                    bit_flags shader_bits,
                    bool optimize = false);
    ~Shader();

    [[nodiscard]] auto module(Stage stage) const -> const vk::raii::ShaderModule&;
  private:
    class Impl;
    koyote::unique<Impl> p_impl_;
  };
}

template<>
struct std::hash<ookami::Shader::Stage> {
  std::size_t operator()(const ookami::Shader::Stage& s) const noexcept {
    return std::hash<koyote::u32>{}(s.underlying_value());
  }
};
