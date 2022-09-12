//
// Created by galex on 9/8/2022.
//

#include "shader.hpp"

#include "foxy/ookami/vulkan/vulkan.hpp"
#include <shaderc/shaderc.hpp>

namespace foxy::vulkan {
  class Shader::Impl {
  public:
    Impl(const vk::raii::Device& device, const std::filesystem::path& file_path, BitFlags shader_bits, bool optimize = false)
      : name_{ file_path.stem().string() } {

      if (load_shader_code(file_path, shader_bits, optimize)) {
        create_shader_modules(device);
      }

      FOXY_TRACE << "Shader \"" << name_ << "\" ready.";
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
        FOXY_ERROR << "File or directory " << file_path << " does not exist";
        found_shader = false;
      }

      if (fs::is_directory(file_path)) {
        FOXY_TRACE << "Loading shader from dir: " << name_;
        found_shader = load_from_dir(file_path, shader_bits);
      } else {
        FOXY_TRACE << "Loading shader from file: " << name_;
        found_shader = load_from_file(file_path, shader_bits);
      }   

      if (found_shader) {
        FOXY_TRACE << "Loaded shader: " << name_;
      } else {
        // TODO: Make this a recoverable error
        FOXY_FATAL << "Failed to load shader code.";
      }
      return found_shader;
    }

    auto load_from_dir(const std::filesystem::path& dir_path, 
                       BitFlags shader_bits, 
                       bool optimize = false) -> bool {
      namespace fs = std::filesystem;
      fs::path tmp_shader_dir{ "tmp" / dir_path.parent_path() / dir_path.stem() };
      fs::path out_file_stem{ tmp_shader_dir / name_ };

      for (u32 i{ 0 }; i < Kind::Max; ++i) {
        Kind kind{ static_cast<Kind>(i) };
        fs::path file_path{ (dir_path / dir_path.stem()).string() + in_file_endings_.at(kind) };
        if (shader_bits.test(kind)) {
          FOXY_TRACE << "Looking for " << kind_names_.at(kind) << ": " << name_;
          fs::path tmp_name{ out_file_stem.string() + out_file_endings_.at(kind) };
          if (fs::exists(tmp_name)) {
            FOXY_TRACE << "Found " << kind_names_.at(kind) << ": " << tmp_name;
            auto code{ read_file(file_path, std::ios::binary).value() };
            bytecode_[kind] = { code.begin(), code.end() };
          } else {
            FOXY_TRACE << "Didn't find " << kind_names_.at(kind) << " in cache: " << tmp_name;
            auto code_str{ read_file(file_path) };
            if (!fs::exists(tmp_shader_dir)) {
              fs::create_directories(tmp_shader_dir);
            }
            compile_shader_type(out_file_stem, *code_str, kind, optimize);
          }
        } else {
            FOXY_TRACE << "Skipping " << kind_names_.at(kind);
        }
      }

      return true;
    }

    auto load_from_file(const std::filesystem::path& file_path, 
                        BitFlags shader_bits, 
                        bool optimize = false) -> bool {
      namespace fs = std::filesystem;
      
      // example: res/shaders/simple.glsl -> tmp/res/shaders/simple/
      fs::path tmp_shader_dir{ "tmp" / file_path.parent_path() / file_path.stem() };
      
      if (fs::exists(tmp_shader_dir)) {
        fs::path out_file_stem{ tmp_shader_dir / name_ };
        for (u32 i{ 0 }; i < Kind::Max; ++i) {
          Kind kind{ static_cast<Kind>(i) };
          if (shader_bits.test(kind)) {
            FOXY_TRACE << "Looking for " << kind_names_.at(kind) << ": " << name_;
            fs::path tmp_name{ out_file_stem.string() + out_file_endings_.at(kind) };
            if (fs::exists(tmp_name)) {
              FOXY_TRACE << "Found " << kind_names_.at(kind) << ": " << tmp_name;
              auto code{ read_file(tmp_name, std::ios::binary).value() };
              bytecode_[kind] = { code.begin(), code.end() };
            } else {
              FOXY_TRACE << "Didn't find " << kind_names_.at(kind) << " in cache: " << tmp_name;
              if (!fs::exists(tmp_shader_dir)) {
                fs::create_directories(tmp_shader_dir);
              }
              compile_combo_file(tmp_shader_dir, file_path, optimize);
            }
          }
        }
      } else {
        if (!fs::exists(tmp_shader_dir)) {
          fs::create_directories(tmp_shader_dir);
        }
        compile_combo_file(tmp_shader_dir, file_path, optimize);
      } 

      return true;   
    }

    void compile_combo_file(const std::filesystem::path& tmp_shader_dir,
                              const std::filesystem::path& file_path, 
                              bool optimize = false) {
      namespace fs = std::filesystem;

      auto code_map = parse_file(file_path);
      if (code_map.empty()) {
        FOXY_FATAL << "Attempted to parse empty/non-existant shader: " << file_path.filename();
      }

      fs::path out_file_stem{ tmp_shader_dir / name_ };

      for (u32 i{ 0 }; i < Kind::Max; ++i) {
        Kind kind{ static_cast<Kind>(i) };
        if (code_map.contains(kind)) {
          compile_shader_type(out_file_stem, code_map.at(kind), kind, optimize);
        }
      }
    }

    std::unordered_map<Kind, std::string> parse_file(const std::filesystem::path& file_path) {
      FOXY_TRACE << "Parsing shader: " << name_ << "...";

      std::unordered_map<Kind, std::string> srcs;

      std::ifstream file{ file_path };
      if (!file.is_open()) {
        FOXY_ERROR << "File " << file_path << " does not exist.";
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
          DCHECK(kinds_.find(kind) != kinds_.end()) << "Shader type " << kind << " is not supported.";

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

    [[nodiscard]] void compile_shader_type(const std::filesystem::path& out_file_stem, 
                                           const std::string& code_str, 
                                           Kind kind, 
                                           bool optimize = false) {
      namespace fs = std::filesystem;
      fs::path out_file{ out_file_stem.string() + out_file_endings_.at(kind) };
      FOXY_TRACE << "Compiling " << kind_names_.at(kind) << ": " << name_ << "...";

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
        FOXY_ERROR << "Shaderc Compile: " << result.GetErrorMessage();
        return; 
      }

      std::string assembly{ result.begin(), result.end() };
      std::ofstream spv_file{ out_file };
      spv_file << assembly;

      bytecode_[kind] = { assembly.begin(), assembly.end() };
    }

    [[nodiscard]] auto preprocess(const std::string& code_str, Kind kind) -> std::string {
      shaderc::Compiler compiler{};
      shaderc::CompileOptions options{};

      auto result{ compiler.PreprocessGlsl(code_str, kind_to_kind_.at(kind), name_.c_str(), options) };
      if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        FOXY_ERROR << "Shaderc Preprocess: " << result.GetErrorMessage();
        return ""; 
      }

      return { result.begin(), result.end() };
    }

    void create_shader_modules(const vk::raii::Device& device) {
      FOXY_TRACE << "Building shader modules: " << name_ << "...";

      for (u32 i{ 0 }; i < Kind::Max; ++i) {
        Kind kind{ static_cast<Kind>(i) };
        if (shader_modules_.contains(kind)) {
          vk::ShaderModuleCreateInfo create_info{
            .codeSize = bytecode_.at(kind).size(),
            .pCode = reinterpret_cast<const u32*>(bytecode_.at(kind).data())
          };

          try {
            shader_modules_.insert(std::make_pair(kind, device.createShaderModule(create_info)));
          } catch (const std::exception& e) {
            FOXY_ERROR << e.what();
          }
        }
      }

      FOXY_TRACE << "Created shader modules: " << name_;
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