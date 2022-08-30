module;

#include "foxy/util/log_macros.hpp"

#include "foxy/util/glm_types.hpp"
#include "foxy/util/boost_types.hpp"

#include <memory>

export module foxy_util;

namespace foxy {
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

  export struct Rect {
    vec2 position;
    vec2 size;

    [[nodiscard]] float width() const { return size.x; }
    [[nodiscard]] float height() const { return size.y; }
    [[nodiscard]] float half_width() const { return size.x * .5f; }
    [[nodiscard]] float half_height() const { return size.y * .5f; }
    [[nodiscard]] vec2 center() const { return vec2{ position.x + half_width(), position.y + half_height() }; }
    [[nodiscard]] vec2 left() const { return position - vec2{ half_width(), 0 }; }
    [[nodiscard]] vec2 right() const { return position + vec2{ half_width(), 0 }; }
    [[nodiscard]] vec2 top() const { return position + vec2{ 0, half_height() }; }
    [[nodiscard]] vec2 bottom() const { return position - vec2{ 0, half_height() }; }
  };

  export struct Bounds {
    vec3 center;
    vec3 size;

    [[nodiscard]] float width() const { return size.x; }
    [[nodiscard]] float height() const { return size.y; }
    [[nodiscard]] float depth() const { return size.z; }
    [[nodiscard]] float half_width() const { return size.x * .5f; }
    [[nodiscard]] float half_height() const { return size.y * .5f; }
    [[nodiscard]] float half_depth() const { return size.z * .5f; }
    [[nodiscard]] vec3 anchor_corner() const { return vec3{ center.x - half_width(), center.y - half_height(), center.z - half_depth() }; }
    [[nodiscard]] vec3 left() const { return center - vec3{ half_width(), 0, 0 }; }
    [[nodiscard]] vec3 right() const { return center + vec3{ half_width(), 0, 0 }; }
    [[nodiscard]] vec3 top() const { return center + vec3{ 0, half_height(), 0 }; }
    [[nodiscard]] vec3 bottom() const { return center - vec3{ 0, half_height(), 0 }; }
    [[nodiscard]] vec3 front() const { return center - vec3{ 0, 0, half_depth() }; }
    [[nodiscard]] vec3 back() const { return center + vec3{ 0, 0, half_depth() }; }
  };

  export template<typename T, typename D = std::default_delete<T>>
  using unique = std::unique_ptr<T, D>;
  export template<typename T, typename... Args>
  constexpr unique<T> make_unique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }

  export template<typename T>
  using shared = std::shared_ptr<T>;
  export template<typename T, typename... Args>
  constexpr shared<T> make_shared(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }

  export template<typename T>
  using weak = std::weak_ptr<T>;

  export template<class T, class U>
  concept derives = std::is_base_of<U, T>::value;

  export class Copyable {
  public:
    Copyable() = default;
    Copyable(const Copyable& other) = default;
    Copyable& operator=(const Copyable& other) = default;
  };

  export class MoveOnly {
  public:
    MoveOnly() = default;
    MoveOnly(MoveOnly&& other) = default;
    MoveOnly& operator=(MoveOnly&& other) = default;

    MoveOnly(const MoveOnly& other) = delete;
    MoveOnly& operator=(const MoveOnly& other) = delete;
  };

  export class NoCopyOrMove {
  public:
    NoCopyOrMove() = default;
    NoCopyOrMove(const NoCopyOrMove& other) = delete;
    NoCopyOrMove& operator=(const NoCopyOrMove& other) = delete;
    NoCopyOrMove(NoCopyOrMove&& other) = delete;
    NoCopyOrMove& operator=(NoCopyOrMove&& other) = delete;
  };
}