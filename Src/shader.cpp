// System
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
// User-Defined
#include "colorIO.hpp"
#include "shaders.hpp"

namespace mini_engine {
// Public
Shader_Comp::Shader_Comp(const path &path_to_shaderc, const path &path_to_input,
                         const path &path_to_output, bool debug)
    : m_shaderc{path_to_shaderc}, m_in_dir{path_to_input},
      m_out_dir{path_to_output}, m_debug{debug} {
  // TODO: check if directory exist
  compile_shader();
}

auto Shader_Comp::vert_compile_amt() const -> int { return m_vert_count; }
auto Shader_Comp::frag_compile_amt() const -> int { return m_frag_count; }

// Static
auto Shader_Comp::read_file(const path &p) -> vector<char> {
  std::ifstream file(p, std::ios::ate | std::ios::binary);
  if (file.fail()) {
    fprintf(stderr, "%s file in read_file failure\n",
            CRED("Shader Compilation: "));
    exit(2);
  }
  auto file_size = static_cast<size_t>(file.tellg());
  vector<char> data(file_size);
  file.seekg(0);
  file.read(data.data(), file_size);
  file.close();
  return data;
}

// Private
auto Shader_Comp::compile_shader() -> void {
  using recursive_dir_iter = std::filesystem::recursive_directory_iterator;
  for (auto const &file : recursive_dir_iter{m_in_dir}) {
    // auto f = file.path().extension().string().c_str();
    if (file.path().extension().string() == ".vert") {
      std::string exe = m_shaderc.string() + " " + file.path().string() +
                        " -o " + m_out_dir.string() + "/" +
                        file.path().filename().string() + ".spv";
      if (std::system(static_cast<const char *>(exe.c_str())) != 0) {
        fprintf(stderr, "Shader: failed to compile %s\n", file.path().c_str());
        continue;
      }
      if (m_debug) {
        std::cout << CGRN("[Shader] ") << exe << " was executed\n";
      }
      ++m_vert_count;
    } else if (file.path().extension().string() == ".frag") {
      std::string exe = m_shaderc.string() + " " + file.path().string() +
                        " -o " + m_out_dir.string() + "/" +
                        file.path().filename().string() + ".spv";
      if (std::system(static_cast<const char *>(exe.c_str())) != 0) {
        fprintf(stderr, "%s failed to compile %s\n", CRED("Shader: "),
                file.path().string().c_str());
        continue;
      }
      if (m_debug) {
        std::cout << CGRN("[Shader] ") << exe << " was executed\n";
      }
      ++m_frag_count;
    } else {
      if (m_debug) {
        fprintf(stderr, "%s Shader: file not a vert or frag file: %s\n",
                CYEL("[Warning]"), file.path().c_str());
      }
      continue;
    }
  }
}

} // namespace mini_engine