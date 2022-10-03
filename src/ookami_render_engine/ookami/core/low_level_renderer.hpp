//
// Created by galex on 10/2/2022.
//

#pragma once

class GLFWwindow;

namespace fx {
  class LowLevelRenderer {
  public:
    explicit LowLevelRenderer(const shared<GLFWwindow>& window);
    ~LowLevelRenderer();
    
    void draw();

  private:
    class Impl;
    unique<Impl> p_impl_;
  };
} // fx
