// System
#include <cstddef>
#include <fstream>
// User-Defined
#include "shaders.hpp"

namespace mini_engine {
//Public
Shader_Comp::Shader_Comp(const path &path_to_shaderc, const path &path_to_input,
                         const path &path_to_output)
    : m_shaderc{path_to_shaderc}, m_in_dir{path_to_input},
      m_out_dir{path_to_output} {
        m_vert_count = compile_vert();
        m_frag_count = compile_frag();
}

auto Shader_Comp::vert_compile_amt() const -> const int {
    return m_vert_count;
}
auto Shader_Comp::frag_compile_amt() const -> const int {
    return m_frag_count;
}

// Static
auto Shader_Comp::read_file(const path &p) -> vector<char> {
    std::ifstream file(p, std::ios::ate | std::ios::binary);
    if (file.fail()) {
        fprintf(stderr, "Shader Compilation: file in read_file failure");
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
auto Shader_Comp::compile_vert() -> int {
    
}

} // namespace mini_engine