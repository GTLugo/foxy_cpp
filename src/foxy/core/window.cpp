#include "window.hpp"

#include "foxy/core/event_system/event.hpp"
#include "foxy/core/window/glfw/glfw_context.hpp"

namespace foxy {
  class Window::Impl {
  public:
    explicit Impl(const WindowCreateInfo& create_info)
      : glfw_context_{},
        state_{State{
          .title = create_info.title,
          .bounds = { { 69, 69 }, { create_info.width, create_info.height } },
          .bounds_before_fullscreen = { { 69, 69 }, { create_info.width, create_info.height } },
          .vsync = create_info.vsync ,
          .fullscreen = create_info.fullscreen,
          .borderless = create_info.borderless,
          .hidden = true,
        }} {
      FOXY_ASSERT(!instantiated_) << "Attempted second instantiation of foxy::App";
      instantiated_ = true;

      FOXY_TRACE << "Creating Window...";

      // Create GLFW window
      glfwWindowHint(GLFW_RESIZABLE, false);
      glfwWindowHint(GLFW_VISIBLE, false);
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

      glfw_window_ = glfw::create_window(glfw::WindowCreateInfo{
        .title = create_info.title,
        .width = create_info.width,
        .height = create_info.height,
      });
      glfwSetWindowUserPointer(glfw_window_.get(), &state_);

      set_vsync(create_info.vsync);
      set_fullscreen(create_info.fullscreen);
      set_callbacks();

      FOXY_TRACE << "Window ready.";
    }

    ~Impl() {
      instantiated_ = false;
      FOXY_TRACE << "Destroying Window";
    }

    void poll_events() {
      glfwPollEvents();
    }

    void close() {
      FOXY_TRACE << "Window close requested";
      glfwSetWindowShouldClose(glfw_window_.get(), true);
    }

    void set_icon(i8* image, i32 width, i32 height) {

    }

    void set_pos(ivec2 position) {

    }

    void set_vsync(bool enabled) {

    }

    void set_fullscreen(bool enabled) {

    }

    void set_hidden(bool hidden) {
      if (hidden) {
        glfwHideWindow(glfw_window_.get());
      } else {
        glfwShowWindow(glfw_window_.get());
      }
      state_.hidden = hidden;
    }

    [[nodiscard]] auto title() const -> std::string { return state_.title; }
    [[nodiscard]] auto native() -> UniqueWindow& { return glfw_window_; }
    [[nodiscard]] auto bounds() const -> Rect { return state_.bounds; }
    [[nodiscard]] auto vsync() const -> bool { return state_.vsync; }
    [[nodiscard]] auto fullscreen() const -> bool { return state_.vsync; }
    [[nodiscard]] auto hidden() const -> bool { return state_.hidden; }
    [[nodiscard]] auto running() const -> bool { return !static_cast<bool>(glfwWindowShouldClose(glfw_window_.get())); }
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

//      explicit State(const WindowCreateInfo& properties)
//        : title{ properties.title },
//          bounds{ { 69, 69 }, { properties.width, properties.height } },
//          bounds_before_fullscreen{ bounds },
//          vsync{ properties.vsync },
//          fullscreen{ properties.fullscreen },
//          borderless{ properties.borderless },
//          hidden{ true } {}
    };

    static inline bool instantiated_{ false };

    // GLFW
    glfw::Context glfw_context_;
    UniqueWindow glfw_window_{ nullptr, nullptr };

    // Foxy
    State state_;

    void set_callbacks() {
      // Foxy
      state_.close_event.set_callback(FOXY_LAMBDA(close));

      // GLFW
      glfwSetWindowCloseCallback(glfw_window_.get(), [](GLFWwindow* window) {
        auto ptr = reinterpret_cast<State*>(glfwGetWindowUserPointer(window));
        ptr->close_event();
      });
    }
  };

  //
  //  Window
  //

  Window::Window(const foxy::WindowCreateInfo&& properties)
      : pImpl_{std::make_unique<Impl>(properties)} {}

  Window::~Window() = default;

  auto Window::operator*() -> UniqueWindow& {
    return pImpl_->native();
  }

  void Window::poll_events() {
    pImpl_->poll_events();
  }

  void Window::close() {
    pImpl_->close();
  }

  void Window::set_icon(i8* image, i32 width, i32 height) {
    pImpl_->set_icon(image, width, height);
  }

  void Window::set_pos(ivec2 position) {
    pImpl_->set_pos(position);
  }

  void Window::set_vsync(bool enabled) {
    pImpl_->set_vsync(enabled);
  }

  void Window::set_fullscreen(bool enabled) {
    pImpl_->set_fullscreen(enabled);
  }

  void Window::set_hidden(bool hidden) {
    pImpl_->set_hidden(hidden);
  }

  auto Window::native() -> UniqueWindow& {
    return pImpl_->native();
  }

  auto Window::title() const -> std::string {
    return pImpl_->title();
  }

  auto Window::bounds() const -> Rect {
    return pImpl_->bounds();
  }

  auto Window::vsync() const -> bool {
    return pImpl_->vsync();
  }

  auto Window::fullscreen() const -> bool {
    return pImpl_->fullscreen();
  }

  auto Window::hidden() const -> bool {
    return pImpl_->hidden();
  }

  auto Window::running() const -> bool {
    return pImpl_->running();
  }
}
