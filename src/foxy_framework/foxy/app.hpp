//
// Created by galex on 8/21/2022.
//

#pragma once

namespace fx {
  class App {
  public:
    struct CreateInfo {
      int argc{ 0 };
      char** argv{ nullptr };
      std::string title{ "FOXY FRAMEWORK" };
      fx::i32 width{ 800 };
      fx::i32 height{ 450 };
      bool vsync{ true };
      bool fullscreen{ false };
      bool borderless{ false };
    };

    enum class Stage {
      Awake,
      Start,
      EarlyTick,
      Tick,
      LateTick,
      EarlyUpdate,
      Update,
      LateUpdate,
      Stop,
    };

    using StageCallback = std::function<void(App&)>;

    explicit App(CreateInfo&& create_info);
    ~App();

    void run();

    auto set_user_data_ptr(fx::shared<void> data) -> App&;
    auto add_stage_before() -> App&;
    auto add_function_to_stage(Stage stage, StageCallback&& callback) -> App&;

    [[nodiscard]] auto user_data_ptr() -> fx::shared<void>;
  private:
    class Impl;
    unique<Impl> p_impl_;
  };
}
