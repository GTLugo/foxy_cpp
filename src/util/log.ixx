module;

#include "foxy/internal/foxy_includes.hpp"
#include "easylogging++.h"

#ifdef __INTELLISENSE__
#include "util/util.ixx"
#endif

export module foxy_log;

#ifndef __INTELLISENSE__
export import foxy_util;
#endif

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