#include "window.hpp"

#include "foxy/core/event_system/event.hpp"
#include "foxy/api/vulkan/renderer.hpp"
#include "foxy/api/glfw/context.hpp"
// this is here to expose vulkan functions to glfw
#define FOXY_GLFW_INCLUDE_VULKAN
#include "foxy/api/glfw/glfw.hpp"

namespace foxy {
  Window::State::State(const WindowCreateInfo& properties)
      : title{ properties.title },
        bounds{ { 69, 69 }, { properties.width, properties.height } },
        bounds_before_fullscreen{ bounds },
        vsync{ properties.vsync },
        fullscreen{ properties.fullscreen },
        borderless{ properties.borderless },
        hidden{ true } {
    // Window events
    close_event = std::make_unique<Event<>>();
    // Input events
    key_event = std::make_unique<Event<>>();
    modifier_event = std::make_unique<Event<>>();
    mouse_event = std::make_unique<Event<>>();
    cursor_event = std::make_unique<Event<>>();
    scroll_event = std::make_unique<Event<>>();
  }

  Window::Window(const foxy::WindowCreateInfo& properties)
      : glfw_context_{new glfw::Context()},
        state_{properties} {
    FOXY_ASSERT(!instantiated_) << "Attempted second instantiation of foxy::App";
    instantiated_ = true;

    // Create GLFW window
    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_VISIBLE, false);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    glfw_window_ = glfw::UniqueWindow{glfwCreateWindow(
        properties.width,
        properties.height,
        properties.title.c_str(),
        nullptr,
        nullptr
    )};
    glfwSetWindowUserPointer(glfw_window_.get(), &state_);

    set_vsync(properties.vsync);
    set_fullscreen(properties.fullscreen);
    set_callbacks();

    renderer_ = std::make_unique<Renderer>(*this);

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
    glfwSetWindowShouldClose(glfw_window_.get(), true);
    //state_.running = false;
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
      glfwHideWindow(glfw_window_.get());
    } else {
      glfwShowWindow(glfw_window_.get());
    }
    state_.hidden = hidden;
  }

  void Window::set_callbacks() {
    // Foxy
    state_.close_event->set_callback(FOXY_LAMBDA(close));

    // GLFW
    glfwSetWindowCloseCallback(glfw_window_.get(), [](GLFWwindow* window) {
      auto ptr = reinterpret_cast<State*>(glfwGetWindowUserPointer(window));
      (*ptr->close_event)();
    });
  }

  auto Window::native() -> glfw::UniqueWindow& { return glfw_window_; }

  FN Window::running() const -> bool {
    return !static_cast<bool>(glfwWindowShouldClose(glfw_window_.get()));
  }
}
