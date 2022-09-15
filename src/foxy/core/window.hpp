//
// Created by galex on 8/21/2022.
//

#pragma once

class GLFWwindow;

namespace foxy::ookami {
  class Renderer;
}

namespace foxy {
  template<class... Args>
  class Event;

  struct WindowCreateInfo {
    const std::string title{ "FOXY FRAMEWORK" };
    const int width{ 1600 };
    const int height{ 900 };
    const bool vsync{ true };
    const bool fullscreen{ false };
    const bool borderless{ false };
  };

  class Window {
  public:
    explicit Window(const WindowCreateInfo&& create_info);
    ~Window();

    auto operator*() -> Shared<GLFWwindow>&;

    void poll_events();
    void close();

    void set_icon(i8* image, i32 width, i32 height);
    void set_pos(ivec2 position);
    void set_vsync(bool enabled);
    void set_fullscreen(bool enabled);
    void set_hidden(bool hidden);

    [[nodiscard]] auto native() -> Shared<GLFWwindow>&;
    [[nodiscard]] auto title() const -> std::string;
    [[nodiscard]] auto bounds() const -> Rect;
    [[nodiscard]] auto vsync() const -> bool;
    [[nodiscard]] auto fullscreen() const -> bool;
    [[nodiscard]] auto hidden() const -> bool;
    [[nodiscard]] auto running() const -> bool;

  private:
    class Impl;
    Unique<Impl> pImpl_;
  };
}