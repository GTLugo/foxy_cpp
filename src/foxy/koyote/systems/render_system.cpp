//
// Created by galex on 4/3/2022.
//

#include "render_system.hpp"

#include "core/app.hpp"

namespace ff {
  void RenderSystem::onUpdate() {
    auto& cameraEntity{App::instance().activeWorld().masterCamera()};
    auto& camera{cameraEntity.getRef<Camera>()};

    for (auto& entity : entities) {
      auto& mesh{entity.getRef<Mesh>()};
      if (mesh.isDirty()) {
        mesh.recalculateMesh();
      }

      //mesh.material.shader->pushMat4(camera.viewProjMatrix(), "viewProj");
      mesh.material.shader->bind();
      Renderer::submit(mesh.renderMesh().vertexArray);
      mesh.material.shader->unbind();
    }
  }
}