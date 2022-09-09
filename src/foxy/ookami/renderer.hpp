//
// Ookami Renderer by Gabriel Lugo
//

#pragma once

class GLFWwindow;

namespace foxy::ookami {
  class Renderer {
    using UniqueWindow = Unique<GLFWwindow, void(*)(GLFWwindow*)>;

  public:
    explicit Renderer(UniqueWindow& window);
    ~Renderer();

  private:
    class Impl;
    Unique<Impl> pImpl_;
  };
}