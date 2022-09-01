#include "window.hpp"

#include "foxy/api/vulkan/renderer.hpp"
#include "foxy/api/glfw/glfw.hpp"

namespace foxy {
  Window::Window(const foxy::WindowCreateInfo& properties)
      : state_{properties} {
    FOXY_ASSERT(!instantiated_) << "Attempted second instantiation of foxy::App";
    instantiated_ = true;

    i32 glfwInitSuccess = glfwInit();
    DCHECK(glfwInitSuccess) << "Failed to initialize GLFW!";
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    glfwSetErrorCallback(glfw_error_callback);

    FOXY_INFO << "GLFW Version " << major << "." << minor << "." << revision;

    // Create GLFW window
    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_VISIBLE, false);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfw_window_ = glfwCreateWindow(
        properties.width,
        properties.height,
        properties.title.c_str(),
        nullptr,
        nullptr
    );
    glfwSetWindowUserPointer(glfw_window_, &state_);

    set_vsync(properties.vsync);
    set_fullscreen(properties.fullscreen);
    set_callbacks();

    renderer_ = std::make_unique<Renderer>();

    FOXY_TRACE << "Created Window";
  }

  Window::~Window() {
    instantiated_ = false;
  }

  void Window::poll_events() {
    glfwPollEvents();
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

  void Window::set_hidden(bool hidden) {
    if (hidden) {
      glfwHideWindow(glfw_window_);
    } else {
      glfwShowWindow(glfw_window_);
    }
    state_.hidden = hidden;
  }

  void Window::set_callbacks() {
    // Foxy
    state_.close_event.set_callback(FOXY_LAMBDA(close));

    // GLFW
    glfwSetWindowCloseCallback(glfw_window_, [](GLFWwindow* window) {
      auto ptr = reinterpret_cast<State*>(glfwGetWindowUserPointer(window));
      ptr->close_event();
    });
  }

  auto Window::native() -> GLFWwindow* { return glfw_window_; }
}