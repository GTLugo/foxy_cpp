//
// Created by galex on 8/21/2022.
//

#pragma once

#include "foxy/util/log.hpp"

#include "glm/glm.hpp"
#include "glm/matrix.hpp"
#include "glm/gtx/quaternion.hpp"

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_hash.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <memory>

#ifdef NDEBUG
  #define FOXY_DEBUG_BREAK
#else
  #ifdef _WIN32 // Windows
    #define FOXY_DEBUG_BREAK __debugbreak()
  #else
    #include <csignal>
    #ifdef SIGTRAP // POSIX
      #define FOXY_DEBUG_BREAK raise(SIGTRAP)
    #else // Other
      #define FOXY_DEBUG_BREAK raise(SIGABRT)
    #endif
  #endif
#endif

#ifdef FOXY_ENABLE_ASSERTS
  #define FOXY_ASSERT(x) if(!(x)) FOXY_FATAL << "FAILED ASSERT: "
#else
  #define FOXY_ASSERT(x) if(false) std::clog
#endif

#define BIT(x) (1 << x)
#define FOXY_BIT_COMPARE_TO(x, y) (x & y) == y
#define FOXY_STRINGIFY_VAL(x) FOXY_STRINGIFY(x)
#define FOXY_STRINGIFY(x) #x
#define FOXY_LAMBDA_INS(fn, instance) [objPtr = instance](auto&&... args) { return objPtr->fn(std::forward<decltype(args)>(args)...); }
#define FOXY_LAMBDA(fn) FOXY_LAMBDA_INS(fn, this)

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

  struct Rect {
    vec2 position;
    vec2 size;

    [[nodiscard]] float width() const { return size.x; }
    [[nodiscard]] float height() const { return size.y; }
    [[nodiscard]] float half_width() const { return size.x * .5f; }
    [[nodiscard]] float half_height() const { return size.y * .5f; }
    [[nodiscard]] vec2 center() const { return vec2{position.x + half_width(), position.y + half_height()}; }
    [[nodiscard]] vec2 left() const   { return position - vec2{half_width(), 0}; }
    [[nodiscard]] vec2 right() const  { return position + vec2{half_width(), 0}; }
    [[nodiscard]] vec2 top() const    { return position + vec2{0, half_height()}; }
    [[nodiscard]] vec2 bottom() const { return position - vec2{0, half_height()}; }
  };

  struct Bounds {
    vec3 center;
    vec3 size;

    [[nodiscard]] float width() const { return size.x; }
    [[nodiscard]] float height() const { return size.y; }
    [[nodiscard]] float depth() const { return size.z; }
    [[nodiscard]] float half_width() const { return size.x * .5f; }
    [[nodiscard]] float half_height() const { return size.y * .5f; }
    [[nodiscard]] float half_depth() const { return size.z * .5f; }
    [[nodiscard]] vec3 anchor_corner() const { return vec3{center.x - half_width(), center.y - half_height(), center.z - half_depth()}; }
    [[nodiscard]] vec3 left() const   { return center - vec3{half_width(), 0, 0}; }
    [[nodiscard]] vec3 right() const  { return center + vec3{half_width(), 0, 0}; }
    [[nodiscard]] vec3 top() const    { return center + vec3{0, half_height(), 0}; }
    [[nodiscard]] vec3 bottom() const { return center - vec3{0, half_height(), 0}; }
    [[nodiscard]] vec3 front() const  { return center - vec3{0, 0, half_depth()}; }
    [[nodiscard]] vec3 back() const   { return center + vec3{0, 0, half_depth()}; }
  };

  template<typename T, typename D = std::default_delete<T>>
  using unique = std::unique_ptr<T, D>;
  template<typename T, typename... Args>
  constexpr unique<T> make_unique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }

  template<typename T>
  using shared = std::shared_ptr<T>;
  template<typename T, typename... Args>
  constexpr shared<T> make_shared(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }

  template<typename T>
  using weak = std::weak_ptr<T>;

  template<class T, class U>
  concept derives = std::is_base_of<U, T>::value;

  class Copyable {
  public:
    Copyable() = default;
    Copyable(const Copyable& other) = default;
    Copyable& operator=(const Copyable& other) = default;
  };

  class MoveOnly {
  public:
    MoveOnly() = default;
    MoveOnly(MoveOnly&& other) = default;
    MoveOnly& operator=(MoveOnly&& other) = default;

    MoveOnly(const MoveOnly& other) = delete;
    MoveOnly& operator=(const MoveOnly& other) = delete;
  };

  class NoCopyOrMove {
  public:
    NoCopyOrMove() = default;
    NoCopyOrMove(const NoCopyOrMove& other) = delete;
    NoCopyOrMove& operator=(const NoCopyOrMove& other) = delete;
    NoCopyOrMove(NoCopyOrMove&& other) = delete;
    NoCopyOrMove& operator=(NoCopyOrMove&& other) = delete;
  };
}
