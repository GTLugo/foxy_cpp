//
// Created by galex on 8/21/2022.
//

#pragma once

#include "foxy/api/glfw/unique_window.hpp"

namespace foxy::glfw {
  class Context;
}

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
    Window(const WindowCreateInfo& properties);
    ~Window();

    void poll_events();
    void close();

    void set_icon(byte* image, i32 width, i32 height);
    void set_pos(ivec2 position);
    void set_vsync(bool enabled);
    void set_fullscreen(bool enabled);
    void set_hidden(bool hidden);

    [[nodiscard]] auto title() const -> std::string { return state_.title; }
    [[nodiscard]] auto native() -> glfw::UniqueWindow&;
    [[nodiscard]] auto bounds() const -> Rect { return state_.bounds; }
    [[nodiscard]] auto vsync() const -> bool { return state_.vsync; }
    [[nodiscard]] auto fullscreen() const -> bool { return state_.vsync; }
    [[nodiscard]] auto hidden() const -> bool { return state_.hidden; }
    [[nodiscard]] auto running() const -> bool;

  private:
    struct State {
      std::string title;
      Rect bounds;
      Rect bounds_before_fullscreen;
      bool vsync;
      bool fullscreen;
      bool borderless;
      bool hidden;
      ivec2 cursor_pos{}, cursor_pos_prev{}, cursor_delta{};
      bool running{true};

      // Window events
      Unique<Event<>> close_event;
      // Input events
      Unique<Event<>> key_event;
      Unique<Event<>> modifier_event;
      Unique<Event<>> mouse_event;
      Unique<Event<>> cursor_event;
      Unique<Event<>> scroll_event;

      explicit State(const WindowCreateInfo& properties);
    };

    static inline bool instantiated_{ false };

    // GLFW
    Unique<glfw::Context> glfw_context_;
    glfw::UniqueWindow glfw_window_;

    // Foxy
    State state_;
    Unique<ookami::Renderer> renderer_;

    void set_callbacks();
  };
}