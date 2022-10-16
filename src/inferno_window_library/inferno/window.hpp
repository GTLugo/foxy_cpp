//
// Created by galex on 8/21/2022.
//

#pragma once

class GLFWwindow;

namespace fx {
  template<class... Args>
  class Event;

  class Window {
  public:
    struct CreateInfo {
      const std::string title{ "FOXY FRAMEWORK" };
      const int width{ 1600 };
      const int height{ 900 };
      const bool vsync{ true };
      const bool fullscreen{ false };
      const bool borderless{ false };
    };

    explicit Window(const CreateInfo&& create_info);
    ~Window();
  
    void add_framebuffer_resized_callback(const std::function<void(i32, i32)>& callback);

    void poll_events();
    void close();
    
    void set_icon(byte* image, i32 width, i32 height);
    void set_icon(const std::filesystem::path& image_path);
    void set_title(const std::string& title);
    void set_subtitle(const std::string& title);
    void set_pos(ivec2 position);
    void set_vsync(bool enabled);
    void set_fullscreen(bool enabled);
    void set_hidden(bool hidden);

    [[nodiscard]] auto native() -> shared<GLFWwindow>&;
    [[nodiscard]] auto title() const -> std::string;
    [[nodiscard]] auto bounds() const -> rect;
    [[nodiscard]] auto vsync() const -> bool;
    [[nodiscard]] auto fullscreen() const -> bool;
    [[nodiscard]] auto hidden() const -> bool;
    [[nodiscard]] auto should_continue() const -> const bool&;
  
    auto operator*() -> shared<GLFWwindow>&;

  private:
    class Impl;
    unique<Impl> p_impl_;
  };
}