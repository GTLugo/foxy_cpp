//
// Created by galex on 8/21/2022.
//

#pragma once

#include "easylogging++.h"

#ifndef EASYPP_INITIALIZED
  INITIALIZE_EASYLOGGINGPP
  #define EASYPP_INITIALIZED
#endif

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

#define FOXY_TRACE LOG(TRACE)
#define FOXY_DEBUG LOG(DEBUG)
#define FOXY_INFO  LOG(INFO)
#define FOXY_WARN  LOG(WARNING)
#define FOXY_ERROR LOG(ERROR)
#define FOXY_FATAL LOG(FATAL)
