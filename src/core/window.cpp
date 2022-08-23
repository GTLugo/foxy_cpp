#include "foxy/core/window.hpp"

namespace foxy {
  static void glfwErrorCallback(int error, const char* message) {
    FOXY_ERROR << "GFLW: " << error << " | " << message;
  }

  Window::Window(const Properties& properties)
    : state_{properties},
      glfw_library_{glfw::init()},
      video_mode_{glfw::getPrimaryMonitor().getVideoMode()} {
    FOXY_ASSERT(!instantiated_) << "Attempted second instantiation of foxy::App";
    instantiated_ = true;
    glfwSetErrorCallback(glfwErrorCallback);

    i32 major, minor, revision;
    auto version = glfw::getRuntimeVersion();
    FOXY_INFO << "GLFW Version " << version.major << "." << version.minor << "." << version.revision;

    // Create GLFW window
    glfw::WindowHints{
      .resizable = false,
      .clientApi = glfw::ClientApi::None,
    }.apply();
    glfw_window_ = glfw::Window{
      properties.width,
      properties.height,
      properties.title.c_str(),
    };
    glfw_window_.setUserPointer(&state_);

    set_vsync(properties.vsync);
    set_fullscreen(properties.fullscreen);
    set_callbacks();

    FOXY_TRACE << "Created Window";
  }

  Window::~Window() {
    instantiated_ = false;
  }

  void Window::poll_events() {
    glfw::pollEvents();
  }

  void Window::close() {
    FOXY_TRACE << "Window close requested";
    state_.running = false;
  }

  void Window::set_icon(byte* image, i32 width, i32 height) {

  }

  void Window::set_pos(ivec2 position) {

  }

  void Window::set_vsync(bool enabled) {

  }

  void Window::set_fullscreen(bool enabled) {

  }

  void Window::set_callbacks() {
    // Foxy
    state_.close_event.set_callback(FOXY_LAMBDA(close));

    // GLFW
    glfw_window_.closeEvent.setCallback([](glfw::Window& window) {
      auto ptr = reinterpret_cast<State*>(window.getUserPointer());
      ptr->close_event();
    });
  }
}