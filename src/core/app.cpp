//
// Created by galex on 8/21/2022.
//

#include "foxy/core/app.hpp"

namespace foxy {
  App::App(const Window::Properties& properties) {
    FOXY_ASSERT(!instantiated_) << "Attempted second instantiation of foxy::App";
    instantiated_ = true;

    Log::init();
    FOXY_INFO << "Foxy startup: Kon kon kitsune! Hi, friends!";

    window_ = make_unique<Window>(properties);
  }

  App::~App() {
    instantiated_ = false;
    FOXY_INFO << "Foxy shutdown: Otsukon deshita! Bye bye!";
  }

  void App::run() {
    game_loop();
  }

  void App::close() {

  }

  App& App::add_global_data() {
    return *this;
  }

  App& App::add_step_before() {
    return *this;
  }

  App& App::add_system_to_step() {
    return *this;
  }

  void App::game_loop() {
    while(window_->running()) {
      window_->poll_events();
    }
  }
}