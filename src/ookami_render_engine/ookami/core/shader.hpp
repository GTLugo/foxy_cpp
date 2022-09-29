//
// Created by galex on 9/8/2022.
//

#pragma once

namespace vk::raii {
  class ShaderModule;
  class Device;
}

namespace fx {
  class Shader {
  public:
    class Kind {
    public:
      enum Value: u32 {
        Vertex   = 0,
        Fragment = 1,
        Compute  = 2,
        Geometry = 3,
      };

      static inline const std::array values{ Vertex, Fragment, Compute, Geometry };

      Kind() = default;
      constexpr Kind(const Value value):
        value_{ value } { }

      constexpr operator Value() const { return value_; }
      explicit operator bool() const = delete;
      constexpr bool operator==(const Kind a) const { return value_ == a.value_; }
      constexpr bool operator!=(const Kind a) const { return value_ != a.value_; }

      [[nodiscard]] static constexpr auto from_string(std::string_view str) -> std::optional<Kind>;
      [[nodiscard]] constexpr auto to_string() const -> std::optional<std::string>;
      [[nodiscard]] constexpr auto to_shaderc() const -> std::optional<i32>;
      [[nodiscard]] auto to_vk_flag() const->std::optional<i32>; // This cannot be constexpr or inline without introducing a linker error

      [[nodiscard]] constexpr auto underlying_value() const -> Value { return value_; }

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

    explicit Shader(const vk::raii::Device& device, const CreateInfo& shader_create_info);
    ~Shader();

    [[nodiscard]] auto module(Kind stage) const -> const vk::raii::ShaderModule&;
    [[nodiscard]] auto has_stage(Kind stage) const -> bool;

    // This cannot be constexpr or inline without introducing a linker error
    [[nodiscard]] static auto kind_to_vk_flag(const Kind kind) -> std::optional<i32>;
  private:
    class Impl;
    unique<Impl> p_impl_;
  };
}

template<>
struct std::hash<fx::Shader::Kind> {
  std::size_t operator()(const fx::Shader::Kind& s) const noexcept {
    return std::hash<fx::u32>{}(s.underlying_value());
  }
};
