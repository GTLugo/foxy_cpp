#include "std.hpp"

#include "koyote/core/log.hpp"

namespace koyote {
  [[nodiscard]] auto read_file(const std::filesystem::path& file_path,
                               const std::ios::fmtflags flags) -> std::optional<std::string>
  {
    if (std::ifstream file{ file_path, std::ios::in | flags }; file.is_open()) {
      std::stringstream buffer;
      buffer << file.rdbuf();

      //koyote::Log::debug("{}", buffer.str().c_str());
      return buffer.str();

      // const auto size{ file_size(file_path) };
      // std::string str(size, '\0');
      // file.read(str.data(), size);
      //
      // return str;
    }

    Log::error("File \"{}\" does not exist.", file_path.string());
    return std::nullopt;
  }
}