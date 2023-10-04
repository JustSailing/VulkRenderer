#pragma once
// System
#include <filesystem>
#include <vector>

namespace mini_engine {
using std::vector;
using std::filesystem::path;
// NOTE: This is more of a utility class
class Shader_Comp {
public:
  explicit Shader_Comp(const path &path_to_shaderc, const path &path_to_input,
                       const path &path_to_output);
  auto inline vert_compile_amt() const -> const int;
  auto inline frag_compile_amt() const -> const int;

private:
  /// Member functions

  // function below should use the paths provided in constructor to compile all
  // vert and frag shaders into spir-v files and put them in output directory
  auto compile_shader() -> void; // count saved in member variables

  // TODO: Should add overloaded function in order compile specific shaders

  //  called to for vkCreateShaderModule
  static auto read_file(const path &p) -> vector<char>;

  /// Member Variables
  int m_vert_count{0};
  int m_frag_count{0};
  path m_in_dir;
  path m_out_dir;
  path m_shaderc;
  // NOTE: Should probably add booleans to check if directorys are valid as well
  // as if shader compiler is valid if we want to change compilers or I/O
  // directories
};
} // namespace mini_engine