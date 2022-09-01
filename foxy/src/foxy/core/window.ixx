//
// Created by galex on 8/21/2022.
//

module;

#include "foxy/internal/foxy_includes.hpp"
#include <glfwpp/glfwpp.h>

export module foxy_window;

import foxy_util;
import foxy_events;
import foxy_vulkan_context;
import foxy_vulkan_renderer;

namespace foxy {
  export void glfw_error_callback(int error, const char* message) {
    FOXY_ERROR << "GFLW: " << error << " | " << message;
  }

  export struct WindowCreateInfo {
    const std::string title{ "FOXY FRAMEWORK" };
    const int width{ 1600 };
    const int height{ 900 };
    const bool vsync{ true };
    const bool fullscreen{ false };
    const bool borderless{ false };
  };

  export class glfw::Window;

  export class Window {
  public:
    Window(const WindowCreateInfo& properties)
      : state_{ properties },
        glfw_library_{ glfw::init() },
        video_mode_{ glfw::getPrimaryMonitor().getVideoMode() } {
      FOXY_ASSERT(!instantiated_) << "Attempted second instantiation of foxy::App";
      instantiated_ = true;
      glfwSetErrorCallback(glfw_error_callback);

      auto version = glfw::getRuntimeVersion();
      FOXY_INFO << "GLFW Version " << version.major << "." << version.minor << "." << version.revision;

      // Create GLFW window
      glfw::WindowHints{
        .resizable = false,
        .visible = false, // hide the window until we're done with initialization to prevent white flash
        .clientApi = glfw::ClientApi::None,
      }.apply();
      glfw_window_ = make_unique<glfw::Window>(
        properties.width,
        properties.height,
        properties.title.c_str()
      );
      glfw_window_->setUserPointer(&state_);

      set_vsync(properties.vsync);
      set_fullscreen(properties.fullscreen);
      set_callbacks();

      renderer_ = make_unique<Renderer>();

      FOXY_TRACE << "Created Window";
    }

    ~Window() {
      instantiated_ = false;
    }

    void poll_events() {
      glfw::pollEvents();
    }

    void close() {
      FOXY_TRACE << "Window close requested";
      state_.running = false;
    }

    void set_icon(byte* image, i32 width, i32 height) {
        
    }

    void set_pos(ivec2 position) {
    
    }

    void set_vsync(bool enabled) {

    }

    void set_fullscreen(bool enabled) {

    }

    void set_hidden(bool hidden) {
      if (hidden) {
        glfw_window_->hide();
      } else {
        glfw_window_->show();
      }
      state_.hidden = hidden; 
    }

    glfw::Window& native() { return *glfw_window_; }

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
    unique<glfw::Window> glfw_window_;
    glfw::GlfwLibrary glfw_library_;
    glfw::VideoMode video_mode_;

    // Foxy
    State state_;
    unique<Renderer> renderer_;

    void set_callbacks() {
      // Foxy
      state_.close_event.set_callback(FOXY_LAMBDA(close));

      // GLFW
      glfw_window_->closeEvent.setCallback([](glfw::Window& window) {
        auto ptr = reinterpret_cast<State*>(window.getUserPointer());
        ptr->close_event();
      });
    }
  };
}