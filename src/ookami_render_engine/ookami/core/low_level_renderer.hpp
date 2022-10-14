//
// Created by galex on 10/2/2022.
//

#pragma once

namespace vk::raii {
  class CommandBuffer;
}

namespace fx {
  class Window;
  class Shader;
  
  namespace ookami {
    class Context;
  }
  
  class LowLevelRenderer {
  public:
    explicit LowLevelRenderer(
      const shared<Window>& window,
      const shared<ookami::Context>& context,
      const shared<Shader>& shader,
      u32 max_frames_in_flight = 1
    );
    ~LowLevelRenderer();
  
    void record_command_buffer(const vk::raii::CommandBuffer& command_buffer, u32 image_index) const;
    void draw() const;

  private:
    class Impl;
    unique<Impl> p_impl_;
  };
} // fx
