//
// Created by galex on 8/21/2022.
//

#pragma once

class GLFWwindow;

namespace ifr {
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

    auto operator*() -> kyt::shared<GLFWwindow>&;

    void poll_events();
    void close();

    void set_icon(kyt::i8* image, kyt::i32 width, kyt::i32 height);
    void set_title(const std::string& title);
    void set_subtitle(const std::string& title);
    void set_pos(kyt::ivec2 position);
    void set_vsync(bool enabled);
    void set_fullscreen(bool enabled);
    void set_hidden(bool hidden);

    [[nodiscard]] auto native() -> kyt::shared<GLFWwindow>&;
    [[nodiscard]] auto title() const -> std::string;
    [[nodiscard]] auto bounds() const -> kyt::rect;
    [[nodiscard]] auto vsync() const -> bool;
    [[nodiscard]] auto fullscreen() const -> bool;
    [[nodiscard]] auto hidden() const -> bool;
    [[nodiscard]] auto running() const -> bool;

  private:
    class Impl;
    kyt::unique<Impl> pImpl_;
  };
}