#pragma once

#include <filesystem>
#include "easylogging++.h"

namespace foxy {
  class Log {
  public:
    Log() {
      Log::init();

      FOXY_INFO << R"([]=============================[])";
      FOXY_INFO << R"(||  ______ ______   ___     __ ||)";
      FOXY_INFO << R"(|| |  ____/ __ \ \ / \ \   / / ||)";
      FOXY_INFO << R"(|| | |__ | |  | \ V / \ \_/ /  ||)";
      FOXY_INFO << R"(|| |  __|| |  | |> <   \   /   ||)";
      FOXY_INFO << R"(|| | |   | |__| / . \   | |    ||)";
      FOXY_INFO << R"(|| |_|    \____/_/ \_\  |_|    ||)";
      FOXY_INFO << R"(||                             ||)";
      FOXY_INFO << R"([]=============================[])";
      FOXY_INFO << "Foxy startup: Kon kon kitsune! Hi, friends!";
      #ifdef FOXY_DEBUG_MODE
      FOXY_INFO << "Build mode: DEBUG";
      #else
      FOXY_INFO << "Build mode: RELEASE";
      #endif
    }

    ~Log() {
      FOXY_INFO << "Foxy shutdown: Otsukon deshita! Bye bye!";
    }

    static void init() {
      std::filesystem::create_directories("./tmp/logs");
      el::Configurations conf("./res/foxy/log_config.conf");
      el::Loggers::reconfigureAllLoggers(conf);
      el::Helpers::setThreadName("main");
    }
  };
}