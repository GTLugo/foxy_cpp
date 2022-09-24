//
// Created by galex on 8/21/2022.
//

#pragma once

namespace foxy {
  class App {
  public:
    struct CreateInfo {
      int argc{ 0 };
      char** argv{ nullptr };
      std::string title{ "FOXY FRAMEWORK" };
      koyote::i32 width{ 800 };
      koyote::i32 height{ 450 };
      bool vsync{ true };
      bool fullscreen{ false };
      bool borderless{ false };
    };

    enum class Stage {
      Awake,
      Start,
      Tick,
      EarlyUpdate,
      LateUpdate,
      Stop,
    };

    using StageCallback = std::function<void(App&)>;

    explicit App(CreateInfo&& create_info);
    ~App();

    void run();

    auto set_user_data_ptr(koyote::shared<void> data) -> App&;
    auto add_stage_before() -> App&;
    auto add_function_to_stage(Stage stage, StageCallback&& callback) -> App&;

    [[nodiscard]] auto user_data_ptr() -> koyote::shared<void>;
  private:
    class Impl;
    koyote::unique<Impl> p_impl_;
  };
}
