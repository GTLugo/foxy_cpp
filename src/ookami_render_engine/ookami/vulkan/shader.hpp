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
    class Kind {
    public:
      enum Value: koyote::u32 {
        Vertex   = 0,
        Fragment = 1,
        Compute  = 2,
        Geometry = 3,

        Max = Geometry,
      };

      Kind() = default;
      constexpr Kind(const Value value)
        : value_{ value } { }

      constexpr operator Value() const { return value_; }
      explicit operator bool() const = delete;
      constexpr bool operator==(const Kind a) const { return value_ == a.value_; }
      constexpr bool operator!=(const Kind a) const { return value_ != a.value_; }

      [[nodiscard]] static constexpr auto from_string(std::string_view str) -> std::optional<Kind>;
      [[nodiscard]] constexpr auto to_string() const -> std::optional<std::string>;
      [[nodiscard]] constexpr auto to_shaderc() const -> std::optional<koyote::i32>;
      [[nodiscard]] auto to_vk_flag() const->std::optional<koyote::i32>; // This cannot be constexpr or inline without introducing a linker error

      [[nodiscard]] constexpr auto underlying_value() const -> Value { return value_; }

      template<typename... Args>
      [[nodiscard]] static constexpr auto bits(Args&&... args) -> koyote::u32 {
        return (BIT(args) | ...);
      }

    private:
      Value value_;
    };

    struct CreateInfo {
      bool vertex{ false };
      bool fragment{ false };
      bool compute{ false };
      bool geometry{ false };
      std::filesystem::path shader_directory;
      bool disable_optimizations{
      #if defined(FOXY_DEBUG_MODE) and not(defined(FOXY_RELEASE_MODE))
        true
      #else
        false
      #endif
      };

      [[nodiscard]] constexpr auto has_kind(const Kind kind) const -> bool {
        switch (kind) {
        case Kind::Vertex:   return vertex;
        case Kind::Fragment: return fragment;
        case Kind::Compute:  return compute;
        case Kind::Geometry: return geometry;
        default:             return false;  // NOLINT(clang-diagnostic-covered-switch-default)
        }
      }
    };

    using bit_flags = std::bitset<Kind::Max + 1>;

    explicit Shader(const vk::raii::Device& device, const CreateInfo& shader_create_info);
    ~Shader();

    [[nodiscard]] auto module(Kind stage) const -> const vk::raii::ShaderModule&;
    [[nodiscard]] auto has_stage(Kind stage) const -> bool;

    // This cannot be constexpr or inline without introducing a linker error
    [[nodiscard]] static auto kind_to_vk_flag(const Kind kind) -> std::optional<koyote::i32>;
  private:
    class Impl;
    koyote::unique<Impl> p_impl_;
  };
}

// Shader::bit_flags{ Shader::Kind::bits(Shader::Kind::Vertex, Shader::Kind::Fragment) } // this is how to write it without macros
// Shader::bit_flags{ OOKAMI_SHADER_VERTEX | OOKAMI_SHADER_FRAGMENT }, // this is another way to write it with macros
#define OOKAMI_SHADER_VERTEX   BIT(Shader::Kind::Vertex)
#define OOKAMI_SHADER_FRAGMENT BIT(Shader::Kind::Fragment)
#define OOKAMI_SHADER_COMPUTE  BIT(Shader::Kind::Compute)
#define OOKAMI_SHADER_GEOMETRY BIT(Shader::Kind::Geometry)

template<>
struct std::hash<ookami::Shader::Kind> {
  std::size_t operator()(const ookami::Shader::Kind& s) const noexcept {
    return std::hash<koyote::u32>{}(s.underlying_value());
  }
};
