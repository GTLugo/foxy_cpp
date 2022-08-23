//
// Created by galex on 8/21/2022.
//

#pragma once

#include "foxy/core/event_system/event.hpp"

#include <glfwpp/glfwpp.h>

namespace foxy {
  class Window final: MoveOnly {
  public:
    struct Properties {
      const std::string title{ "FOXY FRAMEWORK" };
      const int width{ 800 };
      const int height{ 450 };
      //const Renderer::API render_api{ Renderer::API::OpenGL };
      const bool vsync{ true };
      const bool fullscreen{ false };
      const bool borderless{ false };
    };

    explicit Window(const Properties& properties);
    ~Window();

    void poll_events();
    void close();

    void set_icon(byte* image, i32 width, i32 height);
    void set_pos(ivec2 position);
    void set_vsync(bool enabled);
    void set_fullscreen(bool enabled);

    GLFWwindow* native() { return glfw_window_; }
    [[nodiscard]] std::string title() const { return state_.title; }
    [[nodiscard]] Rect bounds() const { return state_.bounds; }
    [[nodiscard]] bool vsync() const { return state_.vsync; }
    [[nodiscard]] bool fullscreen() const { return state_.vsync; }
    [[nodiscard]] bool running() const { return state_.running; }

  private:


    struct State {
      std::string title;
      Rect bounds;
      Rect bounds_before_fullscreen;
      bool vsync;
      bool fullscreen;
      bool borderless;
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

      explicit State(const Properties& properties = {
          "FOXY FRAMEWORK",
          800,
          450,
          true,
          false,
          false,
      })  : title{properties.title},
            bounds{{69, 69}, {properties.width, properties.height}},
            bounds_before_fullscreen{bounds},
            vsync{properties.vsync},
            fullscreen{properties.fullscreen},
            borderless{properties.borderless} {}
    };

    static inline bool instantiated_{ false };

    State state_{};
    glfw::GlfwLibrary glfw_library_;
    glfw::Window glfw_window_;
    const glfw::VideoMode video_mode_;

    void set_callbacks();
  };
}