#pragma once

#include "koyote/core/std.hpp"

namespace koyote {
  class Log {
  public:
    enum Level {
      Trace,
      Debug,
      Info,
      Warn,
      Error,
      Fatal,
    };

    struct FormatLocation {
      std::string fmt_str;
      std::source_location location{};

      FormatLocation(const char* fmt_str_, const std::source_location& location_ = std::source_location::current())
        : fmt_str{fmt_str_}, location{location_} {}

      FormatLocation(const std::string& fmt_str_, const std::source_location& location_ = std::source_location::current())
        : fmt_str{fmt_str_}, location{location_} {}

      FormatLocation(std::string_view fmt_str_, const std::source_location& location_ = std::source_location::current())
        : fmt_str{fmt_str_}, location{location_} {}
        
      template<typename Arg>
      FormatLocation(Arg&& obj_, const std::source_location& location_ = std::source_location::current())
        : location{location_} {
          std::stringstream ss;
          ss << obj_;
          fmt_str = ss.str();
        }

      [[nodiscard]] inline auto file_name() -> std::string {
        return std::filesystem::path{ location.file_name() }.filename().string();
      }
      
      [[nodiscard]] inline auto line_num() -> u32 {
        return location.line();
      }
    };

  #ifdef FOXY_DEBUG_MODE
    #define LOGGING_FUNC_TEMPLATE_IMPL(x)\
    template<class... Args>\
    static inline void x ## (\
      FormatLocation format,\
      Args&&... args\
    ) {\
      x ## _impl(std::vformat(format.fmt_str, std::make_format_args(args..., format.file_name(), format.line_num())));\
    }

    template<class... Args>
    static inline void error(
      FormatLocation format,
      Args&&... args
    ) {
      error_impl(std::vformat(format.fmt_str + " ({}:{})", std::make_format_args(args..., format.file_name(), format.line_num())));
    }

    template<class... Args>
    static inline void fatal(
      FormatLocation format,
      Args&&... args
    ) {
      fatal_impl(std::vformat(format.fmt_str + " ({}:{})", std::make_format_args(args..., format.file_name(), format.line_num())));
    }
  #else
    #define LOGGING_FUNC_TEMPLATE_IMPL(x)\
    template<class... Args>\
    static inline void x ## (\
      FormatLocation format,\
      Args&&... args\
    ) {}

    template<class... Args>
    static inline void error(
      FormatLocation format,
      Args&&... args
    ) {}

    template<class... Args>
    static inline void fatal(
      FormatLocation format,
      Args&&... args
    ) {}
  #endif

    LOGGING_FUNC_TEMPLATE_IMPL(trace)
    LOGGING_FUNC_TEMPLATE_IMPL(debug)
    LOGGING_FUNC_TEMPLATE_IMPL(info)
    LOGGING_FUNC_TEMPLATE_IMPL(warn)

    static void set_level_filter(Level level);
    static void set_thread_name(const std::string& name);
    static void enable_backtrace(koyote::u32 count);
    static void dump_backtrace();
  private:
    class Impl;
    static koyote::shared<Impl> p_impl_;

    Log();
    ~Log();

    //static void x_impl(std::string_view msg);
    static void trace_impl(std::string_view msg);
    static void debug_impl(std::string_view msg);
    static void info_impl(std::string_view msg);
    static void warn_impl(std::string_view msg);
    static void error_impl(std::string_view msg);
    static void fatal_impl(std::string_view msg);
  };
}