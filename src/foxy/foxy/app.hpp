//
// Created by galex on 8/21/2022.
//

#pragma once

namespace fox {
  class App {
  public:
    struct CreateInfo {
      std::string title{ "FOXY FRAMEWORK" };
      kyt::i32 width{ 800 };
      kyt::i32 height{ 450 };
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

    explicit App(CreateInfo&& properties);
    ~App();

    void run();

    auto set_user_data_ptr(kyt::shared<void> data) -> App&;
    auto add_stage_before() -> App&;
    auto add_function_to_stage(Stage stage, StageCallback&& callback) -> App&;

    [[nodiscard]] auto user_data_ptr() -> kyt::shared<void>;
  private:
    PIMPL(Impl) pImpl_;
  };
}
