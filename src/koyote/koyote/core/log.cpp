#include "log.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/pattern_formatter.h>

namespace koyote {
  class Log::Impl {
  public:
    class thread_name_flag : public spdlog::custom_flag_formatter {
    public:
      void format(const spdlog::details::log_msg&, const std::tm&, spdlog::memory_buf_t& dest) override {
        auto thread_id{ std::this_thread::get_id() };
        std::stringstream thread_name{};
        if (names_.contains(thread_id)) {
          thread_name << names_.at(thread_id);
        } else {
          thread_name << thread_id;
        }
        std::string thread_name_str{ std::format("{:^8}", thread_name.str()) };
        dest.append(thread_name_str.data(), thread_name_str.data() + thread_name_str.size());
      }

      std::unique_ptr<custom_flag_formatter> clone() const override {
        return spdlog::details::make_unique<thread_name_flag>();
      }
    };
    
    Impl(const std::string& name, const std::filesystem::path& log_file) {
      auto console_formatter = std::make_unique<spdlog::pattern_formatter>();
      console_formatter->add_flag<thread_name_flag>('~').set_pattern("[%^%4!L%$|%T.%f] %~ | %v");
      auto console_sink{ std::make_shared<spdlog::sinks::stdout_color_sink_mt>() };
      console_sink->set_level(spdlog::level::trace);
      console_sink->set_formatter(std::move(console_formatter));
      
      auto file_formatter = std::make_unique<spdlog::pattern_formatter>();
      file_formatter->add_flag<thread_name_flag>('~').set_pattern("[%^%4!L%$] [%m-%d %T.%f] %~ | %v");
      auto file_sink{ std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file.string(), 2097152, 5, false) };
      file_sink->set_level(spdlog::level::trace);
      file_sink->set_formatter(std::move(file_formatter));

      logger() = spdlog::logger{name, {console_sink, file_sink}};
      logger().set_level(spdlog::level::info);
      koyote::Log::set_thread_name("main");

      Log::info(R"(===========================[])");
      Log::info(R"(  ______ ______   ___     __ )");
      Log::info(R"( |  ____/ __ \ \ / \ \   / / )");
      Log::info(R"( | |__ | |  | \ V / \ \_/ /  )");
      Log::info(R"( |  __|| |  | |> <   \   /   )");
      Log::info(R"( | |   | |__| / . \   | |    )");
      Log::info(R"( |_|    \____/_/ \_\  |_|    )");
      Log::info(R"(                             )");
      Log::info(R"(===========================[])");
      Log::info("Foxy startup: Kon kon kitsune! Hi, friends!");
      #ifdef FOXY_DEBUG_MODE
      Log::info("Build mode: DEBUG");
      #else
      Log::info("Build mode: RELEASE");
      #endif
    }

    ~Impl() {
      Log::info("Foxy shutdown: Otsukon deshita! Bye bye!");
    }
    
    static inline void trace(std::string_view msg) {
      logger().trace(msg);
    }

    static inline void debug(std::string_view msg) {
      logger().debug(msg);
    }

    static inline void info(std::string_view msg) {
      logger().info(msg);
    }

    static inline void warn(std::string_view msg) {
      logger().warn(msg);
    }

    static inline void error(std::string_view msg) {
      logger().error(msg);
    }

    static inline void fatal(std::string_view msg) {
      logger().critical(msg);
      dump_backtrace();
      std::terminate();
    }

    static void set_thread_name(const std::string& name) {
      names_.insert_or_assign(std::this_thread::get_id(), name);
    }

    static void enable_backtrace(koyote::u32 count) {
      logger().enable_backtrace(32);
    }

    static void dump_backtrace() {
      logger().dump_backtrace();
    }

  private:
    static inline std::unordered_map<std::thread::id, std::string> names_{};
    
    static inline auto logger() -> spdlog::logger& {
      static spdlog::logger lg{""};
      return lg;
    }
  }; // Log

  koyote::shared<Log::Impl> Log::pImpl_ = std::make_shared<Impl>("koyote", "./tmp/logs/app.log");

  Log::Log() = default;

  Log::~Log() = default;

  void Log::trace_impl(std::string_view msg) {
    pImpl_->trace(msg);
  }

  void Log::debug_impl(std::string_view msg) {
    pImpl_->debug(msg);
  }

  void Log::info_impl(std::string_view msg) {
    pImpl_->info(msg);
  }

  void Log::warn_impl(std::string_view msg) {
    pImpl_->warn(msg);
  }

  void Log::error_impl(std::string_view msg) {
    pImpl_->error(msg);
  }

  void Log::fatal_impl(std::string_view msg) {
    pImpl_->fatal(msg);
  }
  
  void Log::set_thread_name(const std::string& name) {
    pImpl_->set_thread_name(name);
  }

  void Log::enable_backtrace(koyote::u32 count) {
    pImpl_->enable_backtrace(count);
  }

  void Log::dump_backtrace() {
    pImpl_->dump_backtrace();
  }
} // koyote