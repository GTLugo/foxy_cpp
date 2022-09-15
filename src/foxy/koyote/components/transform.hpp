//
// Created by galex on 3/14/2022.
//

#pragma once

namespace ff {
  struct Transform {
    vec3 position{};
    quat rotation{};
    vec3 scale{1.f, 1.f, 1.f};

    [[nodiscard]] mat4 matrix() const {
      return glm::translate(MatID, position)
             * glm::rotate(MatID, glm::eulerAngles(rotation).z, vec3{0, 0, 1})
             * glm::rotate(MatID, glm::eulerAngles(rotation).y, vec3{0, 1, 0})
             * glm::rotate(MatID, glm::eulerAngles(rotation).x, vec3{1, 0, 0})
             * glm::scale(MatID, scale);
    }
  };
}

