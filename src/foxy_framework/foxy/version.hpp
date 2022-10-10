//
// Created by galex on 10/10/2022.
//

#pragma once

#include <foxy/foxy_config.hpp>

namespace fx {
  struct [[maybe_unused]] FoxyVersion {
    [[maybe_unused]] static inline constexpr u32 major{ FOXY_VERSION_MAJOR };
    [[maybe_unused]] static inline constexpr u32 minor{ FOXY_VERSION_MINOR };
    [[maybe_unused]] static inline constexpr u32 patch{ FOXY_VERSION_PATCH };
  
    [[maybe_unused]] static inline consteval auto str() -> std::string_view
    {
      return FOXY_VERSION_STR;
    }
  };
}