//
// Created by galex on 8/21/2022.
//

#pragma once

#include "foxy/core/event_system/event.hpp"

class GLFWwindow;

namespace foxy {
  inline static void glfw_error_callback(int error, const char* message) {
    FOXY_ERROR << "GFLW: " << error << " | " << message;
  }

  class Renderer;

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

    auto native() -> GLFWwindow*;

    [[nodiscard]] auto title() const -> std::string { 
      return state_.title; 
    }

    [[nodiscard]] auto bounds() const -> Rect { return state_.bounds; }
    [[nodiscard]] auto vsync() const -> bool { return state_.vsync; }
    [[nodiscard]] auto fullscreen() const -> bool { return state_.vsync; }
    [[nodiscard]] auto hidden() const -> bool { return state_.hidden; }
    [[nodiscard]] auto running() const -> bool { return state_.running; }

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
      Event<> close_event;
      // Input events
      Event<> key_event;
      Event<> modifier_event;
      Event<> mouse_event;
      Event<> cursor_event;
      Event<> scroll_event;

      explicit State(const WindowCreateInfo& properties) 
      : title{ properties.title },
        bounds{ { 69, 69 }, { properties.width, properties.height } },
        bounds_before_fullscreen{ bounds },
        vsync{ properties.vsync },
        fullscreen{ properties.fullscreen },
        borderless{ properties.borderless },
        hidden{ true } {}
    };

    static inline bool instantiated_{ false };

    // GLFW
    GLFWwindow* glfw_window_;

    // Foxy
    State state_;
    Unique<Renderer> renderer_;

    void set_callbacks();
  };
}