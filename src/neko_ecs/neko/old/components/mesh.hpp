//
// Created by galex on 4/3/2022.
//

#pragma once

#include <utility>

#include "core/renderer/vertex_array/vertex_array.hpp"
#include "core/renderer/shader.hpp"

namespace ff {
  struct MeshData {
    std::vector<vec3> vertices;
    std::vector<u32> indices;
    std::optional<std::vector<vec4>> colors;
  };

  struct Material {
    Shared<Shader> shader;
    vec4 color;
  };

  //TODO: Work in progress
  struct RenderMesh {
    RenderMesh(MeshData& data, Material& material) {
      //Log::trace_e("Building mesh...");
      std::vector<vec4> cols{data.colors.value_or(std::vector<vec4>(data.vertices.size()))};
      if (!data.colors.has_value()) {
        for (auto&& c: cols) {
          c = material.color;
        }
      }
      vertexArray = VertexArray::create(
          {
              VertexBuffer::create(data.vertices, {VertexAttribute<vec3>{"pos"}}),
              VertexBuffer::create(cols, {VertexAttribute<vec4>{"color"}})
          },
          IndexBuffer::create(data.indices)
      );
    }

    Shared<VertexArray> vertexArray{};
  };

  struct Mesh {
    MeshData meshData;
    Material material;

    Mesh(MeshData meshData_, Material material_):
        meshData{meshData_},
        material{material_},
        renderMesh_{meshData_, material_} {}

    void recalculateMesh() {
      renderMesh_ = RenderMesh{meshData, material};
    }
    [[nodiscard]] RenderMesh renderMesh() const { return renderMesh_; }

    void setDirty() { dirty_ = true; }
    [[nodiscard]] bool isDirty() const { return dirty_; }

  private:
    RenderMesh renderMesh_;
    bool dirty_{false};
  };
}

