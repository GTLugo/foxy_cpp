#include "std.hpp"

#include "koyote/core/log.hpp"

namespace koyote {
  [[nodiscard]] auto read_file(const std::filesystem::path& file_path, 
                               std::ios::fmtflags flags) -> std::optional<std::string> const {
    std::ifstream file{file_path, flags};

    if (!file.is_open()) {
      Log::error("File \"{}\" does not exist.", file_path.string());
      return std::nullopt;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
  }
}