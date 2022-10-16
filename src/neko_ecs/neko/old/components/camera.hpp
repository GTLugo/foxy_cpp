//
// Created by galex on 2/20/2022.
//

#pragma once

namespace ff {
  struct Camera {
  public:
    mat4 projMatrix{};
    mat4 viewMatrix{};

    Camera(float left, float right, float bottom, float top, float near_, float far_);
    Camera(float fovY, float aspectRatio, float near_, float far_);

    [[nodiscard]] mat4 viewProjMatrix() const;
  };
}

