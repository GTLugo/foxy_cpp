module;

#include "foxy/log_macros.hpp"

export module foxy.log;

export INITIALIZE_EASYLOGGINGPP

import <memory>;
import <filesystem>;

namespace foxy {
  export class Log {
  public:
    static inline std::unique_ptr<Log> instance{ nullptr };

    static void init() {
      if (Log::instance == nullptr) {
        instance = std::unique_ptr<Log>{ new Log{} };
      }
    }

  private:
    Log() {
      std::filesystem::create_directories("./tmp/logs");
      el::Configurations conf("./res/foxy/log_config.conf");
      el::Loggers::reconfigureAllLoggers(conf);
      el::Helpers::setThreadName("main");
    }
  };
}