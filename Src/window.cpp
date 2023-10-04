// System
#include <cstdio>
// User-Defined
#include "window.hpp"

namespace mini_engine {
// Public
Window::Window(int w, int h, std::string name, bool vulkan, GLFWmonitor *mon,
               GLFWwindow *win)
    : m_width{w}, m_height{h}, m_win_name{name} {
  if (vulkan)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  init_window(mon, win);
  init_monitors();
}

auto Window::get_monitor_sz(GLFWmonitor *mon, int *width, int *height) const -> void {
    glfwGetMonitorPhysicalSize(mon, width, height);
}

Window::~Window() {
    glfwDestroyWindow(m_win);
    glfwTerminate();
}

// Private
auto Window::init_window(GLFWmonitor *mon, GLFWwindow *win) -> void {
  m_win = glfwCreateWindow(m_width, m_height, m_win_name.data(), mon, win);
  if (m_win == nullptr) {
    fprintf(stderr,
            "GLFW: window creation error: width %d, height %d, title %s\n",
            m_width, m_height, m_win_name.data());
    exit(2);
  }
}

auto Window::init_monitors() -> void {
    m_monitors.primary = glfwGetPrimaryMonitor();
    if (m_monitors.primary == nullptr) {
        fprintf(stderr, "GLFW: getting primary monitor failed");
        exit(2);
    }
    auto arr_monitors = glfwGetMonitors((int *)(&m_monitors.num_of_moniters));
    if (arr_monitors == nullptr) {
        fprintf(stderr, "GLFW: no monitors were found");
        exit(2);
    }
    for (auto i = 0; i < m_monitors.num_of_moniters; ++i) {
        m_monitors.secondaries.emplace_back(arr_monitors[i]);
    }

}

} // namespace mini_engine