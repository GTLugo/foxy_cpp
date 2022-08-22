//
// Created by galex on 8/21/2022.
//

#pragma once

namespace foxy {
  using byte = std::uint8_t;
  using i8 = std::int8_t;
  using u8 = std::uint8_t;
  using i16 = std::int16_t;
  using u16 = std::uint16_t;
  using i32 = std::int32_t;
  using u32 = std::uint32_t;
  using i64 = std::int64_t;
  using u64 = std::uint64_t;
  using i128 = boost::multiprecision::int128_t;
  using u128 = boost::multiprecision::uint128_t;

  // Floating point single precision
  using vec2 = glm::vec2;
  using vec3 = glm::vec3;
  using vec4 = glm::vec4;
  using mat3 = glm::mat3;
  using mat4 = glm::mat4;
  // Floating point double precision
  using dvec2 = glm::dvec2;
  using dvec3 = glm::dvec3;
  using dvec4 = glm::dvec4;
  using dmat3 = glm::dmat3;
  using dmat4 = glm::dmat4;
  // Integer
  using ivec2 = glm::ivec2;
  using ivec3 = glm::ivec3;
  using ivec4 = glm::ivec4;
  // Unsigned integer
  using uvec2 = glm::uvec2;
  using uvec3 = glm::uvec3;
  using uvec4 = glm::uvec4;

  // Misc
  using quat = glm::quat;
  using uuid = boost::uuids::uuid;

  const mat4 MatID{ mat4{1.0f} };
}
