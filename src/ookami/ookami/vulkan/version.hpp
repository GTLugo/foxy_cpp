//
//  Created by Bradley Austin Davis on 2016/03/19
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#pragma once

namespace ookami {
  // Version information for Vulkan is stored in a single 32 bit integer
  // with individual bits representing the major, minor and patch versions.
  // The maximum possible major and minor version is 512 (look out nVidia)
  // while the maximum possible patch version is 2048
  struct Version {
    Version()
        : vulkan_major(0),
          vulkan_minor(0),
          vulkan_patch(0) {}
    Version(kyt::u32 version)
        : Version() {
      *this = version;
    }

    Version& operator=(kyt::u32 version) {
      memcpy(this, &version, sizeof(kyt::u32));
      return *this;
    }

    operator kyt::u32() const {
      kyt::u32 result;
      memcpy(&result, this, sizeof(kyt::u32));
      return result;
    }

    bool operator>=(const Version& other) const {
      return (operator kyt::u32()) >= (other.operator kyt::u32());
    }

    [[nodiscard]] std::string to_string() const {
      std::stringstream buffer;
      buffer << vulkan_major << "." << vulkan_minor << "." << vulkan_patch;
      return buffer.str();
    }

    const kyt::u32 vulkan_patch: 12;
    const kyt::u32 vulkan_minor: 10;
    const kyt::u32 vulkan_major: 10;
  };
}