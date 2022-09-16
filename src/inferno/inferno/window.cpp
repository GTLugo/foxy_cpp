#include "window.hpp"

#include "event/event.hpp"
#include "glfw/context.hpp"

namespace ifr {
  class Window::Impl {
  public:
    explicit Impl(const CreateInfo& create_info)
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
      DCHECK(!instantiated_) << "Attempted second instantiation of foxy::App";
      instantiated_ = true;

      LOG(TRACE) << "Creating Window...";

      // Create GLFW window
      glfwWindowHint(GLFW_RESIZABLE, false);
      glfwWindowHint(GLFW_VISIBLE, false);
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

      glfw_window_ = create_window(WindowCreateInfo{
        .title = create_info.title,
        .width = create_info.width,
        .height = create_info.height,
      });
      glfwSetWindowUserPointer(glfw_window_.get(), &state_);

      set_vsync(create_info.vsync);
      set_fullscreen(create_info.fullscreen);
      set_callbacks();

      LOG(TRACE) << "Window ready.";
    }

    ~Impl() {
      instantiated_ = false;
      LOG(TRACE) << "Destroying Window...";
    }

    void poll_events() {
      glfwPollEvents();
    }

    void close() {
      LOG(TRACE) << "Window close requested.";
      glfwSetWindowShouldClose(glfw_window_.get(), true);
    }

    void set_icon(kyt::i8* image, kyt::i32 width, kyt::i32 height) {

    }

    void set_title(const std::string& title) {
      glfwSetWindowTitle(glfw_window_.get(), title.c_str());
      state_.title = title;
    }

    void set_subtitle(const std::string& title) {
      std::stringstream t;
      t << state_.title << " | " << title;
      glfwSetWindowTitle(glfw_window_.get(), t.str().c_str());
    }

    void set_pos(kyt::ivec2 position) {

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
    [[nodiscard]] auto native() -> kyt::shared<GLFWwindow>& { return glfw_window_; }
    [[nodiscard]] auto bounds() const -> kyt::rect { return state_.bounds; }
    [[nodiscard]] auto vsync() const -> bool { return state_.vsync; }
    [[nodiscard]] auto fullscreen() const -> bool { return state_.vsync; }
    [[nodiscard]] auto hidden() const -> bool { return state_.hidden; }
    [[nodiscard]] auto running() const -> bool { return !static_cast<bool>(glfwWindowShouldClose(glfw_window_.get())); }
  private:
    struct State {
      std::string title;
      kyt::rect bounds;
      kyt::rect bounds_before_fullscreen;
      bool vsync;
      bool fullscreen;
      bool borderless;
      bool hidden;
      kyt::ivec2 cursor_pos{}, cursor_pos_prev{}, cursor_delta{};
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
    Context glfw_context_;
    kyt::shared<GLFWwindow> glfw_window_{ nullptr };

    // Foxy
    State state_;

    void set_callbacks() {
      // Foxy
      kyt::shared<GLFWwindow> x;
      state_.close_event.add_callback(FOXY_LAMBDA(close));

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

  Window::Window(const Window::CreateInfo&& properties)
      : pImpl_{std::make_unique<Impl>(properties)} {}

  Window::~Window() = default;

  auto Window::operator*() -> kyt::shared<GLFWwindow>& {
    return pImpl_->native();
  }

  void Window::poll_events() {
    pImpl_->poll_events();
  }

  void Window::close() {
    pImpl_->close();
  }

  void Window::set_icon(kyt::i8* image, kyt::i32 width, kyt::i32 height) {
    pImpl_->set_icon(image, width, height);
  }

  void Window::set_title(const std::string& title) {
    pImpl_->set_title(title);
  }

  void Window::set_subtitle(const std::string& title) {
    pImpl_->set_subtitle(title);
  }

  void Window::set_pos(kyt::ivec2 position) {
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

  auto Window::native() -> kyt::shared<GLFWwindow>& {
    return pImpl_->native();
  }

  auto Window::title() const -> std::string {
    return pImpl_->title();
  }

  auto Window::bounds() const -> kyt::rect {
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
