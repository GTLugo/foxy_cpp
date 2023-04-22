//
// Created by galex on 9/8/2022.
//

#include "shader.hpp"

#include "vulkan/glslang.hpp"

namespace fx {
  class Shader::Impl {
  public:
    Impl(const vk::raii::Device& device, const ShaderCreateInfo& shader_create_info):
      name_{ shader_create_info.path.stem().string() }
    {
      Log::info("Please wait while shader[\"{}\"] loads...", name_);
      Stopwatch sw{ "shader" };
      
      if (fetch_shader_bytecode(shader_create_info)) {
        Log::trace("Fetched shader bytecode: {}", name_);
        create_shader_modules(shader_create_info, device);
        Log::trace("Shader \"{}\" ready.", name_);
      } else {
        Log::error("Shader \"{}\" failed creation.", name_);
      }
      
      Log::info("Shader[\"{}\"] loading complete! ({} s)", name_, sw.time_elapsed<secs>());
    }
    
    ~Impl() = default;
    
    [[nodiscard]] auto module(const Stage stage) const -> const vk::raii::ShaderModule&
    {
      if (!shader_modules_.contains(stage)) {
        Log::fatal("Shader module for {} stage not found.", *stage.to_string());
      }
      return shader_modules_.at(stage);
    }
  
    [[nodiscard]] auto entry_point(const Stage stage) const -> std::string_view
    {
      if (!shader_modules_.contains(stage)) {
        Log::fatal("Shader module for {} stage not found.", *stage.to_string());
      }
      return shader_entry_points_.at(stage);
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
    [[maybe_unused]] static constexpr inline word spirv_magic_number_{ 0x07230203 };
    
    std::string name_;
    std::unordered_map<Stage, std::vector<word>> bytecode_;
    std::unordered_map<Stage, vk::raii::ShaderModule> shader_modules_;
    std::unordered_map<Stage, std::string> shader_entry_points_;
    
    [[nodiscard]] auto fetch_shader_bytecode(const ShaderCreateInfo& create_info) -> bool
    {
      bool found_shader{ true };
      namespace fs = std::filesystem;
      const fs::path shader_cache_dir{
        fs::path{ "tmp" } / fs::path{ "shader_cache" } / relative(create_info.path, {"res/foxy/shaders"}).parent_path() / create_info.path.stem()
      };
      
      if (!exists(create_info.path)) {
        Log::error("File/Directory {} does not exist", create_info.path.string());
      } else {
        if (is_directory(create_info.path) && create_info.directory) {
          Log::trace("Fetching shader from dir: {}", name_);
        } else if (!is_directory(create_info.path) && !create_info.directory) {
          Log::trace("Fetching shader from single file: {}", name_);
        }
  
        for (const auto& stage: stages) {
          if (!create_info_has_stage(create_info, stage)) {
            Log::trace("Skipping {}: {}", *stage.to_string(), name_);
            continue;
          }
  
          shader_entry_points_[stage] = (create_info.directory) ? "main" : *stage.to_string() + "_main";
          if (auto bytecode{ fetch_stage_bytecode(create_info, stage, shader_cache_dir) }) {
            bytecode_[stage] = *bytecode;
          } else {
            found_shader = false;
          }
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
      const fs::path in_shader_path{
        (create_info.directory) ? create_info.path / fs::path{ *stage.to_string() + ".hlsl" }
                                : create_info.path
      };
      
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
      return compile_shader_type(in_shader_path, out_shader_path, stage, !create_info.directory);
    }
    
    [[nodiscard]] auto compile_shader_type(
      const std::filesystem::path& in_file,
      const std::filesystem::path& out_file,
      const Stage stage,
      bool same_file = false
    ) -> std::optional<std::vector<u32>>
    {
      if (const auto result{ io::read_file(in_file) }) {
        auto code_str{ (*result).c_str() };
        
        Log::trace("Compiling {}: {}...", *stage.to_string(), name_);

        glslang::InitializeProcess();

        auto messages{ static_cast<EShMessages>(EShMsgReadHlsl | EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules) };
        auto language{ static_cast<EShLanguage>(*stage.to_glslang()) };
        glslang::TShader shader{ language };
        shader.setStringsWithLengths(&code_str, nullptr, 1);
        shader.setEnvInput(glslang::EShSourceHlsl, language, glslang::EShClientVulkan, 1);
        shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_3);
        shader.setEnvTarget(glslang::EshTargetSpv, glslang::EShTargetSpv_1_3);
        
        shader.setEntryPoint(shader_entry_points_[stage].c_str());
        shader.setSourceEntryPoint(shader_entry_points_[stage].c_str());
        
        if (!shader.parse(&init_resources(), 130, false, messages)) {
          Log::error("Failed to parse shader[{}]: {} | {}", name_, shader.getInfoLog(), shader.getInfoDebugLog());
          return std::nullopt;
        }

        glslang::TProgram program;
        program.addShader(&shader);

        if (!program.link(messages)) {
          Log::error("Failed to compile shader[{}]: {} | {}", name_, shader.getInfoLog(), shader.getInfoDebugLog());
          return std::nullopt;
        }

        if (std::string str{ shader.getInfoLog() }; !str.empty()) {
          Log::trace("Shader[{}]: {} | {}", name_, shader.getInfoLog(), shader.getInfoDebugLog());
        }

        if (std::string str{ program.getInfoLog() }; !str.empty()) {
          Log::trace("Shader[{}]: {} | {}", name_, program.getInfoLog(), program.getInfoDebugLog());
        }

        glslang::TIntermediate* intermediate{ program.getIntermediate(language) };
        if (intermediate == nullptr) {
          Log::error("Failed to get intermediate code for shader[{}]", name_);
          return std::nullopt;
        }

        spv::SpvBuildLogger logger;
        std::vector<u32> spv;
        GlslangToSpv(*intermediate, spv, &logger);
        
        if (std::string str{ logger.getAllMessages() }; !str.empty()) {
          Log::trace("Shader[{}]: {}", name_, str);
        }
  
        glslang::FinalizeProcess();
        
        if (!io::write_words(out_file, spv)) {
          Log::error("Could not write to output shader file.");
        }
        
        return spv;
      }
      
      return std::nullopt;
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
                    shader_create_info.path, {"res/foxy/shaders"}
                  ).parent_path() / shader_create_info.path.stem()
                };
                const fs::path in_shader_path{ shader_create_info.path / fs::path{ *stage.to_string() + ".hlsl" } };
                const fs::path out_shader_path{ shader_cache_dir / fs::path{ *stage.to_string() + ".spv" } };
                bytecode_[stage] = *compile_shader_type(in_shader_path, out_shader_path, stage);
              } else {
                Log::error("Could not recover from shader module creation failure ({})", e.what());
              }
            }
          }
        }
      }
      
      Log::trace("Built shader modules: {}", name_);
    }
  
    [[nodiscard]] static constexpr auto create_info_has_stage(const ShaderCreateInfo& create_info, const Stage stage) -> bool {
      switch (stage) {
        case Stage::Vertex:   return create_info.vertex;
        case Stage::Fragment: return create_info.fragment;
        case Stage::Compute:  return create_info.compute;
        case Stage::Geometry: return create_info.geometry;
        default:             return false;  // NOLINT(clang-diagnostic-covered-switch-default)
      }
    }
  };
  
  //
  //  Shader
  //
  
  constexpr auto Shader::Stage::to_string() const -> std::optional<std::string>
  {
    switch (value_) {
      case Vertex:   return "vertex";
      case Fragment: return "fragment";
      case Compute:  return "compute";
      case Geometry: return "geometry";
      default:       return std::nullopt;  // NOLINT(clang-diagnostic-covered-switch-default)
    }
  }
  
  constexpr auto Shader::Stage::to_glslang() const -> std::optional<i32>
  {
    switch (value_) {
      case Vertex:   return EShLangVertex;
      case Fragment: return EShLangFragment;
      case Compute:  return EShLangCompute;
      case Geometry: return EShLangGeometry;
      default:       return std::nullopt;  // NOLINT(clang-diagnostic-covered-switch-default)
    }
  }
  
  auto Shader::Stage::to_vk_flag() const -> std::optional<i32> { return Impl::to_vk_flag(*this); }
  
  Shader::Shader(const vk::raii::Device& device, const ShaderCreateInfo& shader_create_info)
    : p_impl_{std::make_unique<Impl>(device, shader_create_info)} {}
  
  Shader::~Shader() = default;
  
  auto Shader::module(const Stage stage) const -> const vk::raii::ShaderModule& { return p_impl_->module(stage); }
  
  auto Shader::entry_point(const Stage stage) const -> std::string_view { return p_impl_->entry_point(stage); }
  
  auto Shader::has_stage(const Stage stage) const -> bool { return p_impl_->has_stage(stage); }
}
