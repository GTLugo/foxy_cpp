//
// Created by galex on 8/21/2022.
//

#pragma once

namespace foxy {
  class App {
  public:
    struct CreateInfo {
      std::string title{ "FOXY FRAMEWORK" };
      i32 width{ 800 };
      i32 height{ 450 };
      bool vsync{ true };
      bool fullscreen{ false };
      bool borderless{ false };
    };

    enum class Stage {
      Awake,
      Start,
      EarlyUpdate,
      Tick,
      LateUpdate,
      Stop,
    };

    using StageCallback = std::function<void(App&)>;

    explicit App(CreateInfo&& properties = {});
    ~App();

    void run();

    auto set_user_data(Shared<void> data) -> App&;
    auto add_stage_before() -> App&;
    auto add_function_to_stage(Stage stage, StageCallback&& callback) -> App&;

    [[nodiscard]] auto user_data() -> Shared<void>;
  private:
    class Impl;
    Unique<Impl> pImpl_;
  };
}
