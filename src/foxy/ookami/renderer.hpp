//
// Ookami Renderer by Gabriel Lugo
//

#pragma once

class GLFWwindow;

namespace foxy::ookami {
  class Renderer {
  public:
    explicit Renderer(Shared<GLFWwindow> window);
    ~Renderer();

  private:
    class Impl;
    Unique<Impl> pImpl_;
  };
}