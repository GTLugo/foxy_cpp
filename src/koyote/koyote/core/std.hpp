#pragma once

#include "koyote/internal/includes.hpp"
#include <uuid_v4.h>

namespace koyote {
  using byte = std::uint8_t;
  using i8 = std::int8_t;
  using u8 = std::uint8_t;
  using i16 = std::int16_t;
  using u16 = std::uint16_t;
  using i32 = std::int32_t;
  using u32 = std::uint32_t;
  using i64 = std::int64_t;
  using u64 = std::uint64_t;

  struct uuid: public UUIDv4::UUID {
    uuid() : UUIDv4::UUID{ generate() } {}
  private:
    [[nodiscard]] static inline auto generate() -> UUIDv4::UUID {
      static UUIDv4::UUIDGenerator<std::mt19937_64> gen{};
      return gen.getUUID();
    }
  };

  template<typename T, typename D = std::default_delete<T>>
  using unique = std::unique_ptr<T, D>;

  template<typename T>
  using shared = std::shared_ptr<T>;

  template<typename T>
  using weak = std::weak_ptr<T>;

  template<class T, class U>
  concept Derives = std::is_base_of<U, T>::value;

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

  template<typename T>
  class pimpl {
  public:
    pimpl();
    template<typename... Args>
    pimpl(Args&&...);
    ~pimpl();
    T* operator->();
    T& operator*();

  private:
    koyote::unique<T> ptr_;
  };

  [[nodiscard]] auto read_file(const std::filesystem::path& file_path, 
                               std::ios::fmtflags flags = {}) -> std::optional<std::string> const;
}