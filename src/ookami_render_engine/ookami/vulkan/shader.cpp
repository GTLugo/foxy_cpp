//
// Created by galex on 9/8/2022.
//

#include "shader.hpp"

#include "vulkan.hpp"
#include <shaderc/shaderc.hpp>

namespace ookami {
  class Shader::Impl {
  public:
    Impl(const vk::raii::Device& device, const CreateInfo& shader_create_info)
      : name_{ shader_create_info.shader_directory.stem().string() } {

      if (load_shader_code(shader_create_info)) {
        koyote::Log::trace("Compiled or read in shader: {}", name_);
        create_shader_modules(device);
        koyote::Log::trace("Shader \"{}\" ready.", name_);
      } else {
        koyote::Log::error("Shader \"{}\" failed creation.", name_);
      }
    }

    ~Impl() = default;

    [[nodiscard]] auto module(const Kind stage) const -> const vk::raii::ShaderModule& {
      if (!shader_modules_.contains(stage)) {
        koyote::Log::fatal("Shader module for {} stage not found.", *stage.to_string());
      }
      return shader_modules_.at(stage);
    }

    [[nodiscard]] auto has_stage(const Kind stage) const -> bool {
      return shader_modules_.contains(stage);
    }
  private:
    std::string name_;
    std::unordered_map<Kind, std::vector<koyote::u32>> bytecode_;
    std::unordered_map<Kind, vk::raii::ShaderModule> shader_modules_;

    static inline const std::string preproc_token_type_{ "#type" };

    [[nodiscard]] auto load_shader_code(const CreateInfo& create_info) -> bool {
      bool found_shader{ false };

      if (!exists(create_info.shader_directory)) {
        koyote::Log::error("Directory {} does not exist", create_info.shader_directory.string());
      } else {

        if (is_directory(create_info.shader_directory)) {
          koyote::Log::trace("Loading shader from dir: {}", name_);
          found_shader = load_from_dir(create_info);
        } else {
          koyote::Log::error("Failed to load shader from dir: {}", name_);
        }
      }

      return found_shader;
    }

    [[nodiscard]] auto load_from_dir(const CreateInfo& create_info) -> bool {
      namespace fs = std::filesystem;
      const fs::path shader_cache_dir{
        fs::path{ "tmp" } / fs::path{ "shader_cache" } / relative(create_info.shader_directory, {"res/foxy/shaders"}).parent_path() / create_info.shader_directory.stem()
      };

      for (auto [i, stage] = std::tuple<koyote::u32, Kind>{ 0, static_cast<Kind::Value>(0) }; i <= Kind::Max; ++i, stage = static_cast<Kind::Value>(i)) {
        if (!load_stage(create_info, stage, shader_cache_dir)) {
          return false;
        }
      }

      return true;
    }

    [[nodiscard]] auto load_stage(const CreateInfo& create_info, const Kind stage, const std::filesystem::path& shader_cache_dir) -> bool {
      namespace fs = std::filesystem;
      const fs::path in_shader_path{ create_info.shader_directory / fs::path{ *stage.to_string() + ".hlsl" } };
      // std::unordered_map<Kind, std::vector<koyote::u32>> quadbytecode;
      // std::unordered_map<Kind, std::vector<koyote::byte>> bytecode;

      if (create_info.has_kind(stage)) {
        koyote::Log::trace("Looking for {}: {}", *stage.to_string(), name_);

        if (const fs::path out_shader_path{ shader_cache_dir / fs::path{ *stage.to_string() + ".spv" } }; exists(out_shader_path)) {
          koyote::Log::trace("Found cached {} at location {}", *stage.to_string(), out_shader_path.string());

          if (const auto code{ read_spv(in_shader_path) }; code.has_value()) {
            bytecode_[stage] = code.value();
          } else {
            return false;
          }

        } else {
          koyote::Log::trace("Couldn't find cached {} at location {}", *stage.to_string(), out_shader_path.string());

          if (const auto code{ koyote::read_file(in_shader_path) }; code.has_value()) {
            if (!exists(shader_cache_dir)) {
              create_directories(shader_cache_dir);
            }

            if (auto result{ compile_shader_type(out_shader_path, *code, stage, !create_info.disable_optimizations) }; result.has_value()) {
              bytecode_[stage] = *result;
            } else {
              return false;
            }
          } else {
            return false;
          }
        }
      } else {
        koyote::Log::trace("Skipping {}", *stage.to_string());
      }

      return true;
    }

    [[nodiscard]] auto compile_shader_type(const std::filesystem::path& out_file, 
                                           const std::string& code_str, 
                                           const Kind stage, 
                                           const bool optimize) -> std::optional<std::vector<koyote::u32>> {
      koyote::Log::trace("Compiling {}: {}...", *stage.to_string(), name_);

      const shaderc::Compiler compiler;
      shaderc::CompileOptions options{};
      options.SetSourceLanguage(shaderc_source_language_hlsl);

      if (optimize) {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
      }

      const auto result{ compiler.CompileGlslToSpv(
        preprocess(code_str, stage), 
        static_cast<shaderc_shader_kind>(*stage.to_shaderc()),
        name_.c_str(), 
        options
      ) };

      if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        koyote::Log::error("Shaderc Compile: {}", result.GetErrorMessage());
        return std::nullopt;
      }

      std::vector<koyote::u32> spv{ result.begin(), result.end() };
      std::ofstream spv_file{ out_file, std::ios::binary };
      //std::ranges::copy(spv, std::ostreambuf_iterator(spv_file));
      spv_file.write(reinterpret_cast<char*>(spv.data()), spv.size() * sizeof(koyote::u32));

      if (!spv_file.good()) {
        koyote::Log::error("Error while writing to file: {}", out_file.string());
      }
      koyote::Log::info("Vector size: {} File size: {}", spv.size(), file_size(out_file));

      //auto res = compiler.AssembleToSpv(assembly, options);

      return spv;
    }

    [[nodiscard]] auto preprocess(const std::string& code_str, const Kind stage) const -> std::string {
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

    [[nodiscard]] static auto read_spv(const std::filesystem::path& path) -> std::optional<std::vector<koyote::byte>> {
      if (std::ifstream file{ path, std::ios::binary }; file.is_open()) {
        return std::vector<koyote::byte>{ std::istreambuf_iterator(file), {} };
      }
      koyote::Log::error("Failed to open file: {}", path.string());
      return std::nullopt;
    }

    void create_shader_modules(const vk::raii::Device& device) {
      koyote::Log::trace("Building shader modules: {}...", name_);

      for (koyote::u32 i{ 0 }; i <= Kind::Max; ++i) {
        if (auto stage{ static_cast<Kind::Value>(i) }; bytecode_.contains(stage)) {
          vk::ShaderModuleCreateInfo create_info{
            .codeSize = bytecode_.at(stage).size() * 4,
            .pCode = reinterpret_cast<const koyote::u32*>(bytecode_.at(stage).data())
          };

          try {
            shader_modules_.emplace(stage, device.createShaderModule(create_info));
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
  constexpr auto Shader::Kind::from_string(const std::string_view str)->std::optional<Kind> {
    if (str == "vertex")   return Vertex;
    if (str == "fragment") return Fragment;
    if (str == "compute")  return Compute;
    if (str == "geometry") return Geometry;
    return std::nullopt;
  }

  constexpr auto Shader::Kind::to_string() const -> std::optional<std::string> {
    if (value_ == Vertex)   return "vertex";
    if (value_ == Fragment) return "fragment";
    if (value_ == Compute)  return "compute";
    if (value_ == Geometry) return "geometry";
    return std::nullopt;
  }

  constexpr auto Shader::Kind::to_shaderc() const -> std::optional<koyote::i32> {
    if (value_ == Vertex)   return shaderc_glsl_vertex_shader;
    if (value_ == Fragment) return shaderc_glsl_fragment_shader;
    if (value_ == Compute)  return shaderc_glsl_compute_shader;
    if (value_ == Geometry) return shaderc_glsl_geometry_shader;
    return std::nullopt;
  }

  Shader::Shader(const vk::raii::Device& device, const CreateInfo& shader_create_info)
    : p_impl_{std::make_unique<Impl>(device, shader_create_info)} {}

  Shader::~Shader() = default;

  auto Shader::module(const Kind stage) const -> const vk::raii::ShaderModule& { return p_impl_->module(stage); }

  auto Shader::has_stage(const Kind stage) const -> bool { return p_impl_->has_stage(stage); }
}
