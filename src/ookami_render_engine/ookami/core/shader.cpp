//
// Created by galex on 9/8/2022.
//

#include "shader.hpp"

#include "vulkan.hpp"
#include <shaderc/shaderc.hpp>

namespace fx {
  class Shader::Impl {
  public:
    Impl(const vk::raii::Device& device, const ShaderCreateInfo& shader_create_info):
      name_{ shader_create_info.shader_directory.stem().string() }
    {
      Log::info("Please wait while shader[\"{}\"] loads...", name_);
      const auto sw{ Stopwatch() };
      
      if (fetch_shader_bytecode(shader_create_info)) {
        Log::trace("Fetched shader bytecode: {}", name_);
        create_shader_modules(shader_create_info, device);
        Log::trace("Shader \"{}\" ready.", name_);
      } else {
        Log::error("Shader \"{}\" failed creation.", name_);
      }
      
      Log::info("Shader[\"{}\"] loading complete! ({} s)", name_, sw.get_time_elapsed<secs>());
    }
    
    ~Impl() = default;
    
    [[nodiscard]] auto module(const Stage stage) const -> const vk::raii::ShaderModule&
    {
      if (!shader_modules_.contains(stage)) {
        Log::fatal("Shader module for {} stage not found.", *stage.to_string());
      }
      return shader_modules_.at(stage);
    }
    
    [[nodiscard]] auto has_stage(const Stage stage) const -> bool
    {
      return shader_modules_.contains(stage);
    }
    
    [[nodiscard]] static constexpr auto to_vk_flag(const Stage stage) -> std::optional<i32>
    {
      if (stage.underlying_value() == Stage::Vertex)   return static_cast<i32>(vk::ShaderStageFlagBits::eVertex);
      if (stage.underlying_value() == Stage::Fragment) return static_cast<i32>(vk::ShaderStageFlagBits::eFragment);
      if (stage.underlying_value() == Stage::Compute)  return static_cast<i32>(vk::ShaderStageFlagBits::eCompute);
      if (stage.underlying_value() == Stage::Geometry) return static_cast<i32>(vk::ShaderStageFlagBits::eGeometry);
      return std::nullopt;
    }
  private:
    static constexpr inline word spirv_magic_number_{ 0x07230203 };
    
    std::string name_;
    std::unordered_map<Stage, std::vector<word>> bytecode_;
    std::unordered_map<Stage, vk::raii::ShaderModule> shader_modules_;
    
    static inline const std::string preproc_token_type_{ "#type" };
    
    [[nodiscard]] auto fetch_shader_bytecode(const ShaderCreateInfo& create_info) -> bool
    {
      bool found_shader{ true };
      namespace fs = std::filesystem;
      const fs::path shader_cache_dir{
        fs::path{ "tmp" } / fs::path{ "shader_cache" } / relative(create_info.shader_directory, {"res/foxy/shaders"}).parent_path() / create_info.shader_directory.stem()
      };
      
      if (!exists(create_info.shader_directory)) {
        Log::error("Directory {} does not exist", create_info.shader_directory.string());
      } else {
        if (is_directory(create_info.shader_directory)) {
          Log::trace("Fetching shader from dir: {}", name_);
          
          for (const auto& stage: stages) {
            if (!create_info_has_stage(create_info, stage)) {
              Log::trace("Skipping {}: {}", *stage.to_string(), name_);
              continue;
            }
            
            if (auto bytecode{ fetch_stage_bytecode(create_info, stage, shader_cache_dir) }) {
              bytecode_[stage] = *bytecode;
            } else {
              found_shader = false;
            }
          }
        } else {
          Log::error("Failed to fetch shader from dir: {}", name_);
        }
      }
      
      return found_shader;
    }
    
    [[nodiscard]] auto fetch_stage_bytecode(
      const ShaderCreateInfo& create_info,
      const Stage stage,
      const std::filesystem::path& shader_cache_dir
    ) -> std::optional<std::vector<word>>
    {
      namespace fs = std::filesystem;
      const fs::path in_shader_path{ create_info.shader_directory / fs::path{ *stage.to_string() + ".hlsl" } };
      
      Log::trace("Looking for {}: {}", *stage.to_string(), name_);
      const fs::path out_shader_path{ shader_cache_dir / fs::path{ *stage.to_string() + ".spv" }};
      
      if (exists(out_shader_path)) {
        if (auto code{ fx::io::read_words(out_shader_path) }) {
          Log::trace("Found cached {} at location {}", *stage.to_string(), out_shader_path.string());
          return code;
        }
      }
      
      Log::warn(
        R"(Failed to fetch valid cached "{}" for shader "{}". Attempting fresh compile.)",
        *stage.to_string(),
        name_
      );
      
      create_directories(shader_cache_dir);
      return compile_shader_type(in_shader_path, out_shader_path, stage, create_info.disable_optimizations);
    }
    
    [[nodiscard]] auto compile_shader_type(
      const std::filesystem::path& in_file,
      const std::filesystem::path& out_file,
      const Stage stage,
      const bool disable_optimizations
    ) -> std::optional<std::vector<u32>>
    {
      if (auto result{ fx::io::read_file(in_file) }) {
        auto& code_str{ *result };
        
        Log::trace("Compiling {}: {}...", *stage.to_string(), name_);
        const shaderc::Compiler compiler;
        shaderc::CompileOptions options{};
        options.SetSourceLanguage(shaderc_source_language_hlsl);
        if (!disable_optimizations) {
          options.SetOptimizationLevel(shaderc_optimization_level_performance);
        }
        const auto compiled{ compiler.CompileGlslToSpv(
          preprocess(code_str, stage),
          static_cast<shaderc_shader_kind>(*stage.to_shaderc()),
          name_.c_str(),
          options
        ) };
        if (compiled.GetCompilationStatus() != shaderc_compilation_status_success) {
          Log::error("Shaderc Compile: {}", compiled.GetErrorMessage());
          return std::nullopt;
        }
        const std::vector<u32> spv = { compiled.begin(), compiled.end() };
        if (!fx::io::write_words(out_file, spv)) {
          Log::error("Could not write to output shader file.");
        }
        
        return spv;
      }
      
      return std::nullopt;
    }
    
    [[nodiscard]] auto preprocess(const std::string& code_str, const Stage stage) const -> std::string
    {
      const shaderc::Compiler compiler{};
      shaderc::CompileOptions options{};
      options.SetSourceLanguage(shaderc_source_language_hlsl);
      
      if (const auto result{ compiler.PreprocessGlsl(code_str, static_cast<shaderc_shader_kind>(*stage.to_shaderc()), name_.c_str(), options) };
          result.GetCompilationStatus() == shaderc_compilation_status_success) {
        return { result.begin(), result.end() };
      } else {
        Log::error("Shaderc Preprocess: {}", result.GetErrorMessage());
        return "";
      }
    }
    
    void create_shader_modules(const ShaderCreateInfo& shader_create_info, const vk::raii::Device& device)
    {
      Log::trace("Building shader modules: {}...", name_);
      
      for (const auto& stage: stages) {
        if (bytecode_.contains(stage)) {
          for (u32 attempt_num{ 0 }; attempt_num < 2; ++attempt_num) {
            const vk::ShaderModuleCreateInfo module_create_info{
              .codeSize = bytecode_.at(stage).size() * 4,
              .pCode = reinterpret_cast<const u32*>(bytecode_.at(stage).data())
            };
            
            try {
              shader_modules_.emplace(stage, device.createShaderModule(module_create_info));
            } catch (const std::exception& e) {
              Log::error("Shader module creation failure, attempting to recompile ({})", e.what());
              
              if (attempt_num == 0) {
                namespace fs = std::filesystem;
                const fs::path shader_cache_dir{
                  fs::path{ "tmp" } / fs::path{ "shader_cache" } / relative(
                    shader_create_info.shader_directory, {"res/foxy/shaders"}
                  ).parent_path() / shader_create_info.shader_directory.stem()
                };
                const fs::path in_shader_path{ shader_create_info.shader_directory / fs::path{ *stage.to_string() + ".hlsl" } };
                const fs::path out_shader_path{ shader_cache_dir / fs::path{ *stage.to_string() + ".spv" } };
                bytecode_[stage] = *compile_shader_type(in_shader_path, out_shader_path, stage, shader_create_info.disable_optimizations);
              } else {
                Log::error("Could not recover from shader module creation failure ({})", e.what());
              }
            }
          }
        }
      }
      
      Log::trace("Built shader modules: {}", name_);
    }
  
    [[nodiscard]] constexpr auto create_info_has_stage(const ShaderCreateInfo& create_info, const Shader::Stage stage) const -> bool {
      switch (stage) {
        case Shader::Stage::Vertex:   return create_info.vertex;
        case Shader::Stage::Fragment: return create_info.fragment;
        case Shader::Stage::Compute:  return create_info.compute;
        case Shader::Stage::Geometry: return create_info.geometry;
        default:             return false;  // NOLINT(clang-diagnostic-covered-switch-default)
      }
    }
  };
  
  //
  //  Shader
  //
  
  constexpr auto Shader::Stage::from_string(const std::string_view str) -> std::optional<Stage>
  {
    if (str == "vertex")   return Vertex;
    if (str == "fragment") return Fragment;
    if (str == "compute")  return Compute;
    if (str == "geometry") return Geometry;
    return std::nullopt;
  }
  
  constexpr auto Shader::Stage::to_string() const -> std::optional<std::string>
  {
    if (value_ == Vertex)   return "vertex";
    if (value_ == Fragment) return "fragment";
    if (value_ == Compute)  return "compute";
    if (value_ == Geometry) return "geometry";
    return std::nullopt;
  }
  
  constexpr auto Shader::Stage::to_shaderc() const -> std::optional<i32>
  {
    if (value_ == Vertex)   return shaderc_glsl_vertex_shader;
    if (value_ == Fragment) return shaderc_glsl_fragment_shader;
    if (value_ == Compute)  return shaderc_glsl_compute_shader;
    if (value_ == Geometry) return shaderc_glsl_geometry_shader;
    return std::nullopt;
  }
  
  auto Shader::Stage::to_vk_flag() const -> std::optional<i32> { return Impl::to_vk_flag(*this); }
  
  Shader::Shader(const vk::raii::Device& device, const ShaderCreateInfo& shader_create_info)
    : p_impl_{std::make_unique<Impl>(device, shader_create_info)} {}
  
  Shader::~Shader() = default;
  
  auto Shader::module(const Stage stage) const -> const vk::raii::ShaderModule& { return p_impl_->module(stage); }
  
  auto Shader::has_stage(const Stage stage) const -> bool { return p_impl_->has_stage(stage); }
}
