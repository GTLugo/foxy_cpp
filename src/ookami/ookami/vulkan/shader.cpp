//
// Created by galex on 9/8/2022.
//

#include "shader.hpp"

#include "vulkan.hpp"
#include <shaderc/shaderc.hpp>

namespace ookami {
  class Shader::Impl {
  public:
    Impl(const vk::raii::Device& device, const std::filesystem::path& file_path, BitFlags shader_bits, bool optimize = false)
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

    [[nodiscard]] auto module(Kind kind) const -> const vk::raii::ShaderModule& { return shader_modules_.at(kind); }
  private:
    std::string name_;
    std::unordered_map<Kind, std::vector<char>> bytecode_;
    std::unordered_map<Kind, vk::raii::ShaderModule> shader_modules_;

    static inline const std::string preproc_token_type_{ "#type" };
    static inline std::unordered_map<Kind, std::string> out_file_endings_{
        {Kind::Vertex  , ".vert.spv"},
        {Kind::Fragment, ".frag.spv"},
        {Kind::Compute , ".comp.spv"},
        {Kind::Geometry, ".geom.spv"},
    };
    static inline std::unordered_map<Kind, std::string> in_file_endings_{
        {Kind::Vertex  , ".vert"},
        {Kind::Fragment, ".frag"},
        {Kind::Compute , ".comp"},
        {Kind::Geometry, ".geom"},
    };
    static inline std::unordered_map<std::string, Kind> kinds_{
        {"vertex"  , Kind::Vertex  },
        {"fragment", Kind::Fragment},
        {"compute" , Kind::Compute },
        {"geometry", Kind::Geometry},
    };
    static inline std::unordered_map<Kind, std::string> kind_names_{
        {Kind::Vertex  , "vertex"  },
        {Kind::Fragment, "fragment"},
        {Kind::Compute , "compute" },
        {Kind::Geometry, "geometry"},
    };
    static inline std::unordered_map<Kind, shaderc_shader_kind> kind_to_kind_{
        {Kind::Vertex  , shaderc_shader_kind::shaderc_glsl_vertex_shader  },
        {Kind::Fragment, shaderc_shader_kind::shaderc_glsl_fragment_shader},
        {Kind::Compute , shaderc_shader_kind::shaderc_glsl_compute_shader },
        {Kind::Geometry, shaderc_shader_kind::shaderc_glsl_geometry_shader},
    };

    auto load_shader_code(const std::filesystem::path& file_path, 
                          BitFlags shader_bits, 
                          bool optimize = false) -> bool {   
      namespace fs = std::filesystem;              
      bool found_shader{ false };

      if (!fs::exists(file_path)) {
        koyote::Log::error("File or directory {} does not exist", file_path.string());
        found_shader = false;
      }

      if (fs::is_directory(file_path)) {
        koyote::Log::trace("Loading shader from dir: {}", name_);
        found_shader = load_from_dir(file_path, shader_bits);
      } else {
        koyote::Log::trace("Loading shader from file: {}", name_);
        found_shader = load_from_file(file_path, shader_bits);
      }

      return found_shader;
    }

    auto load_from_dir(const std::filesystem::path& dir_path, 
                       BitFlags shader_bits, 
                       bool optimize = false) -> bool {
      namespace fs = std::filesystem;
      fs::path tmp_shader_dir{ fs::path{ "tmp" } / fs::path{ "shader_cache" } / fs::relative(dir_path, {"res/foxy/shaders"}).parent_path() / dir_path.stem() };
      fs::path out_file_stem{ tmp_shader_dir / name_ };

      for (koyote::u32 i{ 0 }; i <= Kind::Max; ++i) {
        Kind kind{ static_cast<Kind>(i) };
        fs::path shader_path{ (dir_path / dir_path.stem()).string() + in_file_endings_.at(kind) };
        if (shader_bits.test(kind)) {
          koyote::Log::trace("Looking for {}: {}", kind_names_.at(kind), name_);
          fs::path tmp_name{ out_file_stem.string() + out_file_endings_.at(kind) };
          if (fs::exists(tmp_name)) {
            koyote::Log::trace("Found {}: {}", kind_names_.at(kind), tmp_name.string());
            auto code{ koyote::read_file(shader_path, std::ios::binary) };
            if (!code.has_value()) {
              return false;
            }
            bytecode_[kind] = { code->begin(), code->end() };
          } else {
            koyote::Log::trace("Didn't find {} in cache: {}", kind_names_.at(kind), tmp_name.string());
            auto code{ koyote::read_file(shader_path) };
            if (!code.has_value()) {
              return false;
            }
            if (!fs::exists(tmp_shader_dir)) {
              fs::create_directories(tmp_shader_dir);
            }
            if (!compile_shader_type(out_file_stem, *code, kind, optimize)) {
              return false;
            }
          }
        } else {
          koyote::Log::trace("Skipping {}", kind_names_.at(kind));
        }
      }

      return true;
    }

    auto load_from_file(const std::filesystem::path& file_path, 
                        BitFlags shader_bits, 
                        bool optimize = false) -> bool {
      namespace fs = std::filesystem;
      
      // example: res/shaders/simple.glsl -> tmp/res/shaders/simple/
      fs::path tmp_shader_dir{ fs::path{ "tmp" } / fs::path{ "shader_cache" } / fs::relative(file_path, {"res/foxy/shaders"}).parent_path() / file_path.stem() };
      
      if (fs::exists(tmp_shader_dir)) {
        fs::path out_file_stem{ tmp_shader_dir / name_ };
        for (koyote::u32 i{ 0 }; i <= Kind::Max; ++i) {
          Kind kind{ static_cast<Kind>(i) };
          if (shader_bits.test(kind)) {
            koyote::Log::trace("Looking for {}: {}", kind_names_.at(kind), name_);
            fs::path tmp_name{ out_file_stem.string() + out_file_endings_.at(kind) };
            if (fs::exists(tmp_name)) {
              koyote::Log::trace("Found {}: {}", kind_names_.at(kind), tmp_name.string());
              auto code{ koyote::read_file(tmp_name, std::ios::binary) };
              if (!code.has_value()) {
                return false;
              }
              bytecode_[kind] = { code->begin(), code->end() };
            } else {
              koyote::Log::trace("Didn't find {} in cache: {}", kind_names_.at(kind), tmp_name.string());
              if (!fs::exists(tmp_shader_dir)) {
                fs::create_directories(tmp_shader_dir);
              }
              if (!compile_combo_file(tmp_shader_dir, file_path, optimize)) {
                return false;
              }
            }
          } else {
            koyote::Log::trace("Skipping {}", kind_names_.at(kind));
          }
        }
      } else {
        if (!fs::exists(tmp_shader_dir)) {
          fs::create_directories(tmp_shader_dir);
        }
        if (!compile_combo_file(tmp_shader_dir, file_path, optimize)) {
          return false;
        }
      } 

      return true;   
    }

    auto compile_combo_file(const std::filesystem::path& tmp_shader_dir,
                            const std::filesystem::path& file_path, 
                            bool optimize = false) -> bool {
      namespace fs = std::filesystem;

      auto code_map = parse_file(file_path);
      if (code_map.empty()) {
        koyote::Log::error("Attempted to parse empty/non-existant shader: {}", file_path.filename().string());
        return false;
      }

      fs::path out_file_stem{ tmp_shader_dir / name_ };

      for (koyote::u32 i{ 0 }; i <= Kind::Max; ++i) {
        Kind kind{ static_cast<Kind>(i) };
        if (code_map.contains(kind)) {
          if (!compile_shader_type(out_file_stem, code_map.at(kind), kind, optimize)) {
            return false;
          }
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
      std::string nextWord;
      file >> nextWord;
      while (file) {
        std::stringstream sectionStream;
        if (nextWord == preproc_token_type_) {
          // Grab type of shader
          std::string kind;
          file >> kind;
          if (kinds_.find(kind) == kinds_.end()) {
            koyote::Log::fatal("Shader type {} is not supported.", kind);
          }

          // Loop through shader section body
          file >> nextWord;
          while (file && nextWord != preproc_token_type_) {
            char line[256];
            file.getline(&line[0], 256);
            sectionStream << nextWord << line << '\n';
            file >> nextWord;
          }

          // add to result map
          srcs[kinds_[kind]] = sectionStream.str();
        }
      }

      return srcs;
    }

    [[nodiscard]] auto compile_shader_type(const std::filesystem::path& out_file_stem, 
                                           const std::string& code_str, 
                                           Kind kind, 
                                           bool optimize = false) -> bool {
      namespace fs = std::filesystem;
      fs::path out_file{ out_file_stem.string() + out_file_endings_.at(kind) };
      koyote::Log::trace("Compiling {}: {}...", kind_names_.at(kind), name_);

      shaderc::Compiler compiler{};
      shaderc::CompileOptions options{};

      if (optimize) {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
      }

      auto result{ compiler.CompileGlslToSpvAssembly(
        preprocess(code_str, kind), 
        kind_to_kind_.at(kind), 
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

    [[nodiscard]] auto preprocess(const std::string& code_str, Kind kind) -> std::string {
      shaderc::Compiler compiler{};
      shaderc::CompileOptions options{};

      auto result{ compiler.PreprocessGlsl(code_str, kind_to_kind_.at(kind), name_.c_str(), options) };
      if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        koyote::Log::error("Shaderc Preprocess: {}", result.GetErrorMessage());
        return ""; 
      }

      return { result.begin(), result.end() };
    }

    void create_shader_modules(const vk::raii::Device& device) {
      koyote::Log::trace("Building shader modules: {}...", name_);

      for (koyote::u32 i{ 0 }; i <= Kind::Max; ++i) {
        Kind kind{ static_cast<Kind>(i) };
        if (shader_modules_.contains(kind)) {
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

  Shader::Shader(const vk::raii::Device& device, const std::filesystem::path& file_path, BitFlags shader_bits, bool optimize)
    : pImpl_{std::make_unique<Impl>(device, file_path, shader_bits, optimize)} {}

  Shader::~Shader() = default;

  auto Shader::module(Shader::Kind kind) const -> const vk::raii::ShaderModule& { return pImpl_->module(kind); }
}