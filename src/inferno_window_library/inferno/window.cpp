#include "window.hpp"

#include "glfw/context.hpp"

namespace fx {
  class Window::Impl {
  public:
    explicit Impl(const CreateInfo& create_info):
      glfw_context_{},
      state_{State{
        .title = create_info.title,
        .bounds = { { 69, 69 }, { create_info.width, create_info.height } },
        .bounds_before_fullscreen = { { 69, 69 }, { create_info.width, create_info.height } },
        .vsync = create_info.vsync ,
        .fullscreen = create_info.fullscreen,
        .borderless = create_info.borderless,
        .hidden = true,
      }}
    {
      if (instantiated_) {
        Log::fatal("Attempted second instantiation of foxy::Window");
      }
      instantiated_ = true;

      Log::trace("Creating Window...");

      // Create GLFW window
      glfwWindowHint(GLFW_RESIZABLE, false);
      glfwWindowHint(GLFW_VISIBLE, false);
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

      glfw_window_ = create_window(inferno::WindowCreateInfo{
        .title = create_info.title,
        .width = create_info.width,
        .height = create_info.height,
      });
      glfwSetWindowUserPointer(glfw_window_.get(), &state_);

      set_vsync(create_info.vsync);
      set_fullscreen(create_info.fullscreen);
      set_callbacks();

      Log::trace("Window ready.");
    }

    ~Impl()
    {
      instantiated_ = false;
      Log::trace("Destroying Window...");
    }

    void poll_events()
    {
      glfwPollEvents();
    }

    void close()
    {
      Log::trace("Window close requested.");
      state_.should_stop = true;
    }

    void set_icon(i8* image, i32 width, i32 height)
    {

    }

    void set_title(const std::string& title)
    {
      glfwSetWindowTitle(glfw_window_.get(), title.c_str());
      state_.title = title;
    }

    void set_subtitle(const std::string& title)
    {
      std::stringstream t;
      t << state_.title << " | " << title;
      if (!title.empty()) {
        glfwSetWindowTitle(glfw_window_.get(), t.str().c_str());
      } else {
        glfwSetWindowTitle(glfw_window_.get(), state_.title.c_str());
      }
      
    }

    void set_pos(ivec2 position)
    {

    }

    void set_vsync(bool enabled)
    {

    }

    void set_fullscreen(bool enabled)
    {

    }

    void set_hidden(bool hidden)
    {
      if (hidden) {
        glfwHideWindow(glfw_window_.get());
      } else {
        glfwShowWindow(glfw_window_.get());
      }
      state_.hidden = hidden;
    }

    [[nodiscard]] auto title() const -> std::string { return state_.title; }
    [[nodiscard]] auto native() -> shared<GLFWwindow>& { return glfw_window_; }
    [[nodiscard]] auto bounds() const -> rect { return state_.bounds; }
    [[nodiscard]] auto vsync() const -> bool { return state_.vsync; }
    [[nodiscard]] auto fullscreen() const -> bool { return state_.vsync; }
    [[nodiscard]] auto hidden() const -> bool { return state_.hidden; }
    [[nodiscard]] auto should_close() const -> const bool& { return state_.should_stop; }
  private:
    struct State {
      std::string title;
      rect bounds;
      rect bounds_before_fullscreen;
      bool vsync;
      bool fullscreen;
      bool borderless;
      bool hidden;
      ivec2 cursor_pos{}, cursor_pos_prev{}, cursor_delta{};
      bool should_stop{ false };

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
    inferno::Context glfw_context_;
    shared<GLFWwindow> glfw_window_{ nullptr };

    // Foxy
    State state_;

    void set_callbacks()
    {
      // Foxy
      shared<GLFWwindow> x;
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
      : p_impl_{std::make_unique<Impl>(properties)} {}

  Window::~Window() = default;

  auto Window::operator*() -> shared<GLFWwindow>&
    {
    return p_impl_->native();
  }

  void Window::poll_events()
  {
    p_impl_->poll_events();
  }

  void Window::close()
  {
    p_impl_->close();
  }

  void Window::set_icon(i8* image, i32 width, i32 height)
  {
    p_impl_->set_icon(image, width, height);
  }

  void Window::set_title(const std::string& title)
  {
    p_impl_->set_title(title);
  }

  void Window::set_subtitle(const std::string& title)
  {
    p_impl_->set_subtitle(title);
  }

  void Window::set_pos(ivec2 position)
  {
    p_impl_->set_pos(position);
  }

  void Window::set_vsync(bool enabled)
  {
    p_impl_->set_vsync(enabled);
  }

  void Window::set_fullscreen(bool enabled)
  {
    p_impl_->set_fullscreen(enabled);
  }

  void Window::set_hidden(bool hidden)
  {
    p_impl_->set_hidden(hidden);
  }

  auto Window::native() -> shared<GLFWwindow>&
    {
    return p_impl_->native();
  }

  auto Window::title() const -> std::string
  {
    return p_impl_->title();
  }

  auto Window::bounds() const -> rect
  {
    return p_impl_->bounds();
  }

  auto Window::vsync() const -> bool
  {
    return p_impl_->vsync();
  }

  auto Window::fullscreen() const -> bool
  {
    return p_impl_->fullscreen();
  }

  auto Window::hidden() const -> bool
  {
    return p_impl_->hidden();
  }

  auto Window::should_stop() const -> const bool&
  {
    return p_impl_->should_close();
  }
}
