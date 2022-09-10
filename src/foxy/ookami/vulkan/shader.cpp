//
// Created by galex on 9/8/2022.
//

#include "shader.hpp"

#include "foxy/ookami/vulkan/vulkan.hpp"
#include <shaderc/shaderc.hpp>

namespace foxy::vulkan {
  class Shader::Impl {
  public:
    Impl(const vk::raii::Device& device, const std::filesystem::path& file_path, bool optimize = false)
      : name_{ file_path.stem().string() } {
      load_shader_code(file_path, optimize);
      create_shader_modules(device);

      FOXY_TRACE << "Shader \"" << name_ << "\" ready.";
    }

    ~Impl() = default;

    [[nodiscard]] auto module(Kind kind) const -> const vk::raii::ShaderModule& { return shader_modules_.at(kind); }
  private:
    std::string name_;
    std::unordered_map<Kind, std::vector<char>> bytecode_;
    std::unordered_map<Kind, vk::raii::ShaderModule> shader_modules_;

    static inline const std::string preproc_token_type_{ "#type" };
    static inline std::unordered_map<Kind, std::string> file_endings_{
        {Kind::Vertex, ".vert.spv"},
        {Kind::Fragment, ".frag.spv"},
    };
    static inline std::unordered_map<std::string, Kind> kinds_{
        {"vertex", Kind::Vertex},
        {"fragment", Kind::Fragment},
    };
    static inline std::unordered_map<Kind, shaderc_shader_kind> kind_to_kind_{
        {Kind::Vertex, shaderc_shader_kind::shaderc_glsl_vertex_shader},
        {Kind::Fragment, shaderc_shader_kind::shaderc_glsl_fragment_shader},
    };

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

    [[nodiscard]] auto compile_single_shader(const std::string& code_str, Kind kind, bool optimize = false) -> std::string {
      shaderc::Compiler compiler{};
      shaderc::CompileOptions options{};

      if (optimize) {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
      }

      auto result{ compiler.CompileGlslToSpvAssembly(code_str, kind_to_kind_.at(kind), name_.c_str(), options) };
      if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        FOXY_ERROR << "Shaderc Compile: " << result.GetErrorMessage();
        return ""; 
      }

      return { result.begin(), result.end() };
    }

    void compile_from_scratch(const std::filesystem::path& tmp_shader_dir,
                                            const std::filesystem::path& file_path, 
                                            bool optimize = false) {
      namespace fs = std::filesystem;
      FOXY_TRACE << "Compiling shader from scratch: " << name_;

      auto code_map = parse_file(file_path);
      if (code_map.empty()) {
        FOXY_FATAL << "Attempted to compile empty/non-existant shader: " << file_path.filename();
      }

      FOXY_TRACE << "Shader cached location: " << tmp_shader_dir;
      fs::create_directories(tmp_shader_dir);

      if (code_map.contains(Kind::Vertex)) {
        std::string out_file_name{ tmp_shader_dir.string() + '/' + name_ + file_endings_.at(Kind::Vertex) };
        FOXY_TRACE << "Compiling vertex: " << name_ << "...";
        std::string code_str{ preprocess(code_map.at(Kind::Vertex), Kind::Vertex) };
        //FOXY_DEBUG << code_str;
        std::string assembly{ compile_single_shader(code_str, Kind::Vertex, optimize) };
        std::ofstream spv_file{ out_file_name };
        spv_file << assembly;
        bytecode_[Kind::Vertex] = { assembly.begin(), assembly.end() };
      }

      if (code_map.contains(Kind::Fragment)) {
        std::string out_file_name{ tmp_shader_dir.string() + '/' + name_ + file_endings_.at(Kind::Fragment) };
        FOXY_TRACE << "Compiling fragment: " << name_ << "...";
        std::string code_str{ preprocess(code_map.at(Kind::Fragment), Kind::Fragment) };
        //FOXY_DEBUG << code_str;
        std::string assembly{ compile_single_shader(code_str, Kind::Fragment, optimize) };
        std::ofstream spv_file{ out_file_name };
        spv_file << assembly;
        bytecode_[Kind::Fragment] = { assembly.begin(), assembly.end() };
      }
    }

    void load_shader_code(const std::filesystem::path& file_path, 
                          bool optimize = false) {                  
      FOXY_TRACE << "Loading shader: " << name_;

      namespace fs = std::filesystem;
      fs::path filename{ name_ };
      fs::path tmp_shader_dir{ "tmp" };
      // example: res/shaders/simple.glsl -> tmp/res/shaders/simple/
      tmp_shader_dir /= file_path.parent_path() /= file_path.stem();
      
      if (fs::exists(tmp_shader_dir)) {
        FOXY_TRACE << "Grabbing shader from cache: " << name_;
        std::string partial_name{ (tmp_shader_dir /= filename).string() };

        fs::path tmp_vert{ std::string{ partial_name + file_endings_.at(Kind::Vertex) } };
        FOXY_TRACE << "Looking for vertex: " << name_;
        if (fs::exists(tmp_vert)) {
          auto code{ read_file(tmp_vert, std::ios::binary).value() };
          bytecode_[Kind::Vertex] = { code.begin(), code.end() };
          FOXY_TRACE << "Found vertex: " << tmp_vert;
        } else {
          compile_from_scratch(tmp_shader_dir, file_path, optimize);
        }

        fs::path tmp_frag{ std::string{ partial_name + file_endings_.at(Kind::Fragment) } };
        FOXY_TRACE << "Looking for fragment: " << name_;
        if (fs::exists(tmp_frag)) {
          auto code{ read_file(tmp_frag, std::ios::binary).value() };
          bytecode_[Kind::Fragment] = { code.begin(), code.end() };
          FOXY_TRACE << "Found fragment: " << tmp_frag;
        } else {
          compile_from_scratch(tmp_shader_dir, file_path, optimize);
        }

      } else {
        compile_from_scratch(tmp_shader_dir, file_path, optimize);
      }       

      FOXY_TRACE << "Loaded shader: " << name_;
    }

    void create_shader_modules(const vk::raii::Device& device) {
      FOXY_TRACE << "Building shader modules: " << name_ << "...";

      if (shader_modules_.contains(Kind::Vertex)) {
        vk::ShaderModuleCreateInfo create_info{
          .codeSize = bytecode_.at(Kind::Vertex).size(),
          .pCode = reinterpret_cast<const u32*>(bytecode_.at(Kind::Vertex).data())
        };

        try {
          shader_modules_.insert(std::make_pair(Kind::Vertex, device.createShaderModule(create_info)));
        } catch (const std::exception& e) {
          FOXY_ERROR << e.what();
        }
      }

      if (shader_modules_.contains(Kind::Fragment)) {
        vk::ShaderModuleCreateInfo create_info{
          .codeSize = bytecode_.at(Kind::Fragment).size(),
          .pCode = reinterpret_cast<const u32*>(bytecode_.at(Kind::Vertex).data())
        };

        try {
          shader_modules_.insert(std::make_pair(Kind::Fragment, device.createShaderModule(create_info)));
        } catch (const std::exception& e) {
          FOXY_ERROR << e.what();
        }
      }

      FOXY_TRACE << "Shader modules ready: " << name_;
    }
  };

  //
  //  Shader
  //

  Shader::Shader(const vk::raii::Device& device, const std::filesystem::path& file_path, bool optimize)
    : pImpl_{std::make_unique<Impl>(device, file_path, optimize)} {}

  Shader::~Shader() = default;

  auto Shader::module(Shader::Kind kind) const -> const vk::raii::ShaderModule& { return pImpl_->module(kind); }
}