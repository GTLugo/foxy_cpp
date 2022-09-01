#pragma once

#include <filesystem>
#include "easylogging++.h"

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