//
// Created by galex on 9/8/2022.
//

#include "shader.hpp"

#include "vulkan.hpp"
#include <shaderc/shaderc.hpp>

namespace ookami {
  class Shader::Impl {
  public:
    Impl(const vk::raii::Device& device, 
         const std::filesystem::path& file_path, 
         const bit_flags shader_bits, 
         const bool optimize = false)
      : name_{ file_path.stem().string() } {

      if (load_shader_code(file_path, shader_bits, optimize)) {
        koyote::Log::trace("Loaded shader: {}", name_);
        create_shader_modules(device);
        koyote::Log::trace("Shader \"{}\" ready.", name_);
      } else {
        koyote::Log::error("Shader \"{}\" failed creation.", name_);
      }

    }

    ~Impl() = default;

    [[nodiscard]] auto module(const Kind kind) const -> const vk::raii::ShaderModule& { return shader_modules_.at(kind); }
  private:
    std::string name_;
    std::unordered_map<Kind, std::vector<char>> bytecode_;
    std::unordered_map<Kind, vk::raii::ShaderModule> shader_modules_;

    static inline const std::string preproc_token_type_{ "#type" };
    static inline std::unordered_map<std::string, Kind> string_to_kind_{
        {"vertex"  , Kind::Vertex  },
        {"fragment", Kind::Fragment},
        {"compute" , Kind::Compute },
        {"geometry", Kind::Geometry},
    };
    static inline std::unordered_map<Kind, std::string> kind_to_string_{
        {Kind::Vertex  , "vertex"  },
        {Kind::Fragment, "fragment"},
        {Kind::Compute , "compute" },
        {Kind::Geometry, "geometry"},
    };
    static inline std::unordered_map<Kind, shaderc_shader_kind> kind_to_shaderc_kind_{
        {Kind::Vertex  , shaderc_shader_kind::shaderc_glsl_vertex_shader  },
        {Kind::Fragment, shaderc_shader_kind::shaderc_glsl_fragment_shader},
        {Kind::Compute , shaderc_shader_kind::shaderc_glsl_compute_shader },
        {Kind::Geometry, shaderc_shader_kind::shaderc_glsl_geometry_shader},
    };

    auto load_shader_code(const std::filesystem::path& file_path,
                          const bit_flags shader_bits, 
                          const bool optimize = false) -> bool {   
      namespace fs = std::filesystem;              
      bool found_shader{ false };

      if (!fs::exists(file_path)) {
        koyote::Log::error("Directory {} does not exist", file_path.string());
      } else {
        if (fs::is_directory(file_path)) {
          koyote::Log::trace("Loading shader from dir: {}", name_);
          found_shader = load_from_dir(file_path, shader_bits);
        } else {
          koyote::Log::error("Failed to load shader from dir: {}", name_);
        }
      }

      return found_shader;
    }

    auto load_from_dir(const std::filesystem::path& dir_path,
                       const bit_flags shader_bits,
                       const bool optimize = false) -> bool {
      namespace fs = std::filesystem;
      const fs::path tmp_shader_dir{ fs::path{ "tmp" } / fs::path{ "shader_cache" } / fs::relative(dir_path, {"res/foxy/shaders"}).parent_path() / dir_path.stem() };

      for (auto [i, kind] = std::tuple<koyote::u32, Kind>{ 0, static_cast<Kind>(0) }; i <= Kind::Max; ++i, kind = static_cast<Kind>(i)) {
        fs::path in_shader_path{ dir_path / fs::path{ kind_to_string_.at(kind) + ".hlsl" } };

        if (shader_bits.test(kind)) {
          koyote::Log::trace("Looking for {}: {}", kind_to_string_.at(kind), name_);

          if (fs::path out_shader_path{ tmp_shader_dir / fs::path{ kind_to_string_.at(kind) + ".spv" } }; fs::exists(out_shader_path)) {
            koyote::Log::trace("Found cached {} at location {}", kind_to_string_.at(kind), out_shader_path.string());

            if (auto code{ koyote::read_file(in_shader_path, std::ios::binary) }; code.has_value()) {
              bytecode_[kind] = { code->begin(), code->end() };
            } else {
              return false;
            }

          } else {
            koyote::Log::trace("Couldn't find cached {} at location {}", kind_to_string_.at(kind), out_shader_path.string());

            if (auto code{ koyote::read_file(in_shader_path) }; code.has_value()) {
              if (!fs::exists(tmp_shader_dir)) {
                fs::create_directories(tmp_shader_dir);
              }

              if (!compile_shader_type(out_shader_path, *code, kind, optimize)) {
                return false;
              }
            } else {
              return false;
            }
          }
        } else {
          koyote::Log::trace("Skipping {}", kind_to_string_.at(kind));
        }
      }

      return true;
    }

    std::unordered_map<Kind, std::string> parse_file(const std::filesystem::path& file_path) {
      koyote::Log::trace("Parsing shader: {}...", name_);

      std::unordered_map<Kind, std::string> srcs;

      std::ifstream file{ file_path };
      if (!file.is_open()) {
        koyote::Log::error("File {} does not exist.", file_path.string());
        return srcs;
      }

      // TODO: Re-Optimize this
      std::string next_word;
      file >> next_word;
      while (file) {
        if (next_word == preproc_token_type_) {
          std::stringstream section_stream;
          // Grab type of shader
          std::string kind;
          file >> kind;
          if (!string_to_kind_.contains(kind)) {
            koyote::Log::fatal("Shader type {} is not supported.", kind);
          }

          // Loop through shader section body
          file >> next_word;
          while (file && next_word != preproc_token_type_) {
            char line[256]{};
            file.getline(&line[0], 256);
            section_stream << next_word << line << '\n';
            file >> next_word;
          }

          // add to result map
          srcs[string_to_kind_[kind]] = section_stream.str();
        }
      }

      return srcs;
    }

    [[nodiscard]] auto compile_shader_type(const std::filesystem::path& out_file, 
                                           const std::string& code_str, 
                                           const Kind kind, 
                                           const bool optimize = false) -> bool {
      koyote::Log::trace("Compiling {}: {}...", kind_to_string_.at(kind), name_);

      const shaderc::Compiler compiler;
      shaderc::CompileOptions options{};
      options.SetSourceLanguage(shaderc_source_language_hlsl);

      if (optimize) {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
      }

      const auto result{ compiler.CompileGlslToSpvAssembly(
        preprocess(code_str, kind), 
        kind_to_shaderc_kind_.at(kind),
        name_.c_str(), 
        options) 
      };

      if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        koyote::Log::error("Shaderc Compile: {}", result.GetErrorMessage());
        return false; 
      }

      std::string assembly{ result.begin(), result.end() };
      std::ofstream spv_file{ out_file };
      spv_file << assembly;

      bytecode_[kind] = { assembly.begin(), assembly.end() };
      return true;
    }

    [[nodiscard]] auto preprocess(const std::string& code_str, const Kind kind) const -> std::string {
      const shaderc::Compiler compiler{};
      shaderc::CompileOptions options{};
      options.SetSourceLanguage(shaderc_source_language_hlsl);

      const auto result{ compiler.PreprocessGlsl(code_str, kind_to_shaderc_kind_.at(kind), name_.c_str(), options) };
      if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        koyote::Log::error("Shaderc Preprocess: {}", result.GetErrorMessage());
        return ""; 
      }

      return { result.begin(), result.end() };
    }

    void create_shader_modules(const vk::raii::Device& device) {
      koyote::Log::trace("Building shader modules: {}...", name_);

      for (koyote::u32 i{ 0 }; i <= Kind::Max; ++i) {
        if (auto kind{ static_cast<Kind>(i) }; shader_modules_.contains(kind)) {
          vk::ShaderModuleCreateInfo create_info{
            .codeSize = bytecode_.at(kind).size(),
            .pCode = reinterpret_cast<const koyote::u32*>(bytecode_.at(kind).data())
          };

          try {
            shader_modules_.insert(std::make_pair(kind, device.createShaderModule(create_info)));
          } catch (const std::exception& e) {
            koyote::Log::error(e.what());
          }
        }
      }

      koyote::Log::trace("Created shader modules: {}", name_);
    }
  };

  //
  //  Shader
  //

  Shader::Shader(const vk::raii::Device& device, const std::filesystem::path& file_path, const bit_flags shader_bits, const bool optimize)
    : pImpl_{std::make_unique<Impl>(device, file_path, shader_bits, optimize)} {}

  Shader::~Shader() = default;

  auto Shader::module(const Shader::Kind kind) const -> const vk::raii::ShaderModule& { return pImpl_->module(kind); }
}
