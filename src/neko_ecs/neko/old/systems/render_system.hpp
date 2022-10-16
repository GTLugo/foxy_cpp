//
// Created by galex on 4/3/2022.
//

#pragma once

#include "core/ecs/ecs.hpp"

namespace ff {
  class RenderSystem : public System<Transform, Mesh> {
  public:
    void onUpdate() override;
  };
}
