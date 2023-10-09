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

  [[nodiscard("returns the amount of files with .vert that were compiled")]]
  auto inline vert_compile_amt() const -> const int;

  [[nodiscard("returns the amount of files with .frag that were compiled")]]
  auto inline frag_compile_amt() const -> const int;

  // TODO: rather than exiting should return an error. Possibly add a parameter that will
  // fit both an enum type that will give success or some error
  [[nodiscard("returns a vector of chars of the shader file requested")]]
  static auto read_file(const path &p) -> vector<char>;
  // TODO: Should add overloaded function in order compile specific shaders

private:
  /// Member functions

  // function should use the paths provided in constructor to compile all
  // vert and frag shaders into spir-v files and put them in output directory
  auto compile_shader() -> void; // count saved in member variables
  
  /// Member Variables
  int m_vert_count{0};
  int m_frag_count{0};
  path m_shaderc;
  path m_in_dir;
  path m_out_dir;
  // NOTE: Should probably add booleans to check if directorys are valid as well
  // as if shader compiler is valid if we want to change compilers or I/O
  // directories
};
} // namespace mini_engine