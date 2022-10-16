//
// Created by galex on 8/21/2022.
//

#pragma once

namespace fx {
  struct Time_C {
    double delta{ 0 };
  };
  
  class App {
  public:
    struct CreateInfo {
      int argc{ 0 };
      char** argv{ nullptr };
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
      EarlyTick,
      Tick,
      LateTick,
      EarlyUpdate,
      Update,
      LateUpdate,
      Stop,
      Asleep,
    };

    using StageCallback = std::function<void(App&, const Time&)>;

    explicit App(CreateInfo&& create_info);
    ~App();

    auto set_user_data_ptr(shared<void> data) -> App&;
    auto add_function_to_stage(Stage stage, StageCallback&& callback) -> App&;
    
    [[nodiscard]] auto user_data_ptr() -> shared<void>;
  
    void run();
  
    auto operator()() -> void;
    
  private:
    class Impl;
    unique<Impl> p_impl_;
  };
}
