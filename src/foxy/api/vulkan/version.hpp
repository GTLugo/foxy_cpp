//
// Created by galex on 9/1/2022.
//

#pragma once

#include "foxy/internal/includes.hpp"

namespace foxy {
  struct Version {
    Version()
        : vulkan_major(0),
          vulkan_minor(0),
          vulkan_patch(0) {}
    Version(u32 version)
        : Version() {
      *this = version;
    }

    Version& operator=(u32 version) {
      memcpy(this, &version, sizeof(u32));
      return *this;
    }

    operator u32() const {
      u32 result;
      memcpy(&result, this, sizeof(u32));
      return result;
    }

    bool operator>=(const Version& other) const {
      return (operator u32()) >= (other.operator u32());
    }

    [[nodiscard]] std::string to_string() const {
      std::stringstream buffer;
      buffer << vulkan_major << "." << vulkan_minor << "." << vulkan_patch;
      return buffer.str();
    }

    const u32 vulkan_patch : 12;
    const u32 vulkan_minor : 10;
    const u32 vulkan_major : 10;
  };
}