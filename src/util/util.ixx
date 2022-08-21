module;

#include "foxy/foxy_includes.hpp"

export module foxy.util;

export import <memory>;
export import <cstdint>;

namespace ff {
  export using byte = std::uint8_t;
  export using i8 = std::int8_t;
  export using u8 = std::uint8_t;
  export using i16 = std::int16_t;
  export using u16 = std::uint16_t;
  export using i32 = std::int32_t;
  export using u32 = std::uint32_t;
  export using i64 = std::int64_t;
  export using u64 = std::uint64_t;
  export using i128 = boost::multiprecision::int128_t;
  export using u128 = boost::multiprecision::uint128_t;

  // Floating point single precision
  export using vec2 = glm::vec2;
  export using vec3 = glm::vec3;
  export using vec4 = glm::vec4;
  export using mat3 = glm::mat3;
  export using mat4 = glm::mat4;
  // Floating point double precision
  export using dvec2 = glm::dvec2;
  export using dvec3 = glm::dvec3;
  export using dvec4 = glm::dvec4;
  export using dmat3 = glm::dmat3;
  export using dmat4 = glm::dmat4;
  // Integer
  export using ivec2 = glm::ivec2;
  export using ivec3 = glm::ivec3;
  export using ivec4 = glm::ivec4;
  // Unsigned integer
  export using uvec2 = glm::uvec2;
  export using uvec3 = glm::uvec3;
  export using uvec4 = glm::uvec4;

  // Misc
  export using quat = glm::quat;
  export using uuid = boost::uuids::uuid;

  export const mat4 MatID{ mat4{1.0f} };
}