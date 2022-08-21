//
// Created by galex on 8/21/2022.
//

#pragma once

#include "foxy/log_macros.hpp"

INITIALIZE_EASYLOGGINGPP

#include <filesystem>

namespace foxy {
  class Log {
  public:
    static void init() {
      std::filesystem::create_directories("./tmp/logs");
      el::Configurations conf("./res/foxy/log_config.conf");
      el::Loggers::reconfigureAllLoggers(conf);
      el::Helpers::setThreadName("main");
    }
  };
}
