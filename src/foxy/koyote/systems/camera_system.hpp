//
// Created by galex on 4/3/2022.
//

#pragma once

#include "core/ecs/ecs.hpp"

namespace ff {
  class CameraSystem : public System<Transform, Camera> {
  public:
    void onUpdate() override {
      parallelFor([](Entity& entity) {
        auto& transform{entity.getRef<Transform>()};
        auto& camera{entity.getRef<Camera>()};
        camera.viewMatrix = glm::inverse(transform.matrix());
      });
    }
  };
}
