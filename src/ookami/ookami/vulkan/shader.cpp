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

    [[nodiscard]] auto module(const Stage stage) const -> const vk::raii::ShaderModule& { return shader_modules_.at(stage); }
  private:
    std::string name_;
    std::unordered_map<Stage, std::vector<char>> bytecode_;
    std::unordered_map<Stage, vk::raii::ShaderModule> shader_modules_;

    static inline const std::string preproc_token_type_{ "#type" };

    [[nodiscard]] auto load_shader_code(const std::filesystem::path& file_path,
                                        const bit_flags shader_bits, 
                                        const bool optimize) -> bool {            
      bool found_shader{ false };

      if (!exists(file_path)) {
        koyote::Log::error("Directory {} does not exist", file_path.string());
      } else {

        if (is_directory(file_path)) {
          koyote::Log::trace("Loading shader from dir: {}", name_);
          found_shader = load_from_dir(file_path, shader_bits, optimize);
        } else {
          koyote::Log::error("Failed to load shader from dir: {}", name_);
        }
      }

      return found_shader;
    }

    [[nodiscard]] auto load_from_dir(const std::filesystem::path& dir_path,
                                     const bit_flags shader_bits,
                                     const bool optimize) -> bool {
      namespace fs = std::filesystem;
      const fs::path tmp_shader_dir{ fs::path{ "tmp" } / fs::path{ "shader_cache" } / fs::relative(dir_path, {"res/foxy/shaders"}).parent_path() / dir_path.stem() };

      for (auto [i, stage] = std::tuple<koyote::u32, Stage>{ 0, static_cast<Stage::Value>(0) }; i <= Stage::Max; ++i, stage = static_cast<Stage::Value>(i)) {
        fs::path in_shader_path{ dir_path / fs::path{ *stage.to_string() + ".hlsl" } };

        if (shader_bits.test(stage)) {
          koyote::Log::trace("Looking for {}: {}", *stage.to_string(), name_);

          if (fs::path out_shader_path{ tmp_shader_dir / fs::path{ *stage.to_string() + ".spv" } }; fs::exists(out_shader_path)) {
            koyote::Log::trace("Found cached {} at location {}", *stage.to_string(), out_shader_path.string());

            if (auto code{ koyote::read_file(in_shader_path, std::ios::binary) }; code.has_value()) {
              bytecode_[stage] = { code->begin(), code->end() };
            } else {
              return false;
            }

          } else {
            koyote::Log::trace("Couldn't find cached {} at location {}", *stage.to_string(), out_shader_path.string());

            if (auto code{ koyote::read_file(in_shader_path) }; code.has_value()) {
              if (!exists(tmp_shader_dir)) {
                create_directories(tmp_shader_dir);
              }

              if (!compile_shader_type(out_shader_path, *code, stage, optimize)) {
                return false;
              }
            } else {
              return false;
            }
          }
        } else {
          koyote::Log::trace("Skipping {}", *stage.to_string());
        }
      }

      return true;
    }

    [[nodiscard]] auto parse_file(const std::filesystem::path& file_path) -> std::unordered_map<Stage, std::string>{
      koyote::Log::trace("Parsing shader: {}...", name_);

      std::unordered_map<Stage, std::string> srcs;

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
          std::string stage;
          file >> stage;
          if (!Stage::from_string(stage).has_value()) {
            koyote::Log::fatal("Shader type {} is not supported.", stage);
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
          srcs[*Stage::from_string(stage)] = section_stream.str();
        }
      }

      return srcs;
    }

    [[nodiscard]] auto compile_shader_type(const std::filesystem::path& out_file, 
                                           const std::string& code_str, 
                                           const Stage stage, 
                                           const bool optimize) -> bool {
      koyote::Log::trace("Compiling {}: {}...", *stage.to_string(), name_);

      const shaderc::Compiler compiler;
      shaderc::CompileOptions options{};
      options.SetSourceLanguage(shaderc_source_language_hlsl);

      if (optimize) {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
      }

      const auto result{ compiler.CompileGlslToSpvAssembly(
        preprocess(code_str, stage), 
        static_cast<shaderc_shader_kind>(*stage.to_shaderc()),
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

      bytecode_[stage] = { assembly.begin(), assembly.end() };
      return true;
    }

    [[nodiscard]] auto preprocess(const std::string& code_str, const Stage stage) const -> std::string {
      const shaderc::Compiler compiler{};
      shaderc::CompileOptions options{};
      options.SetSourceLanguage(shaderc_source_language_hlsl);

      if (const auto result{ compiler.PreprocessGlsl(code_str, static_cast<shaderc_shader_kind>(*stage.to_shaderc()), name_.c_str(), options) }; 
          result.GetCompilationStatus() == shaderc_compilation_status_success) {
        return { result.begin(), result.end() };
      } else {
        koyote::Log::error("Shaderc Preprocess: {}", result.GetErrorMessage());
        return "";
      }
    }

    void create_shader_modules(const vk::raii::Device& device) {
      koyote::Log::trace("Building shader modules: {}...", name_);

      for (koyote::u32 i{ 0 }; i <= Stage::Max; ++i) {
        if (auto stage{ static_cast<Stage::Value>(i) }; shader_modules_.contains(stage)) {
          vk::ShaderModuleCreateInfo create_info{
            .codeSize = bytecode_.at(stage).size(),
            .pCode = reinterpret_cast<const koyote::u32*>(bytecode_.at(stage).data())
          };

          try {
            shader_modules_.insert(std::make_pair(stage, device.createShaderModule(create_info)));
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
  constexpr auto Shader::Stage::from_string(std::string_view str)->std::optional<Stage> {
    if (str == "vertex")   return Vertex;
    if (str == "fragment") return Fragment;
    if (str == "compute")  return Compute;
    if (str == "geometry") return Geometry;
    return std::nullopt;
  }

  constexpr auto Shader::Stage::to_string() const -> std::optional<std::string> {
    if (value_ == Vertex)   return "vertex";
    if (value_ == Fragment) return "fragment";
    if (value_ == Compute)  return "compute";
    if (value_ == Geometry) return "geometry";
    return std::nullopt;
  }

  constexpr auto Shader::Stage::to_shaderc() const -> std::optional<koyote::i32> {
    if (value_ == Vertex)   return shaderc_glsl_vertex_shader;
    if (value_ == Fragment) return shaderc_glsl_fragment_shader;
    if (value_ == Compute)  return shaderc_glsl_compute_shader;
    if (value_ == Geometry) return shaderc_glsl_geometry_shader;
    return std::nullopt;
  }

  Shader::Shader(const vk::raii::Device& device, const std::filesystem::path& file_path, bit_flags shader_bits, bool optimize)
    : p_impl_{std::make_unique<Impl>(device, file_path, shader_bits, optimize)} {}

  Shader::~Shader() = default;

  auto Shader::module(const Shader::Stage stage) const -> const vk::raii::ShaderModule& { return p_impl_->module(stage); }
}
