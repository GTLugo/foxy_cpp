//
// Created by galex on 8/21/2022.
//

#include "foxy/core/app.hpp"

namespace foxy {
  App::App(const foxy::WindowProperties& props) {
    Log::init();
    FOXY_INFO << "Foxy startup: Kon kon kitsune! Hi, friends!";
  }

  App::~App() {
    FOXY_INFO << "Foxy shutdown: Otsukon deshita! Bye bye!";
  }

  void App::run() {

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

  }
}