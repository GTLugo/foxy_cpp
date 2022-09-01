module;

#define __SPECSTRINGS_STRICT_LEVEL 0
#include "foxy/internal/foxy_includes.hpp"
#include "easylogging++.h"

export module foxy_log;

import foxy_util;
import <filesystem>;

export INITIALIZE_EASYLOGGINGPP

export namespace foxy {
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