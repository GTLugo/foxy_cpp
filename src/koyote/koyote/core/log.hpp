#pragma once

#include "easylogging++.h"
#include <filesystem>

namespace kyt {
  class Log {
  public:
    Log() {
      Log::init();

      LOG(INFO) << R"([]=============================[])";
      LOG(INFO) << R"(||  ______ ______   ___     __ ||)";
      LOG(INFO) << R"(|| |  ____/ __ \ \ / \ \   / / ||)";
      LOG(INFO) << R"(|| | |__ | |  | \ V / \ \_/ /  ||)";
      LOG(INFO) << R"(|| |  __|| |  | |> <   \   /   ||)";
      LOG(INFO) << R"(|| | |   | |__| / . \   | |    ||)";
      LOG(INFO) << R"(|| |_|    \____/_/ \_\  |_|    ||)";
      LOG(INFO) << R"(||                             ||)";
      LOG(INFO) << R"([]=============================[])";
      LOG(INFO) << "Foxy startup: Kon kon kitsune! Hi, friends!";
      #ifdef FOXY_DEBUG_MODE
      LOG(INFO) << "Build mode: DEBUG";
      #else
      LOG(INFO) << "Build mode: RELEASE";
      #endif
    }

    ~Log() {
      LOG(INFO) << "Foxy shutdown: Otsukon deshita! Bye bye!";
    }

    static void init() {
      std::filesystem::create_directories("./tmp/logs");
      el::Configurations conf("./res/foxy/log_config.conf");
      el::Loggers::reconfigureAllLoggers(conf);
      el::Helpers::setThreadName("main");
    }
  };
}