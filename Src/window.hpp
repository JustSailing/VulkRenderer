#pragma once
// Library 
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// System 
#include <string>
#include <vector>

namespace mini_engine {
struct Monitors {
  GLFWmonitor *primary;
  std::vector<GLFWmonitor *> secondaries;
  uint32_t num_of_moniters;
};

class Window {
public:
  explicit Window(int w, int h, std::string title, bool vulkan,
                  GLFWmonitor *monitor = nullptr, GLFWwindow *window = nullptr);

  Window(const Window &) = delete;

  Window &operator=(const Window &) = delete;

  auto get_monitors() const -> const Monitors & {return m_monitors;};

  /// should only be called after get_monitors
  auto inline get_monitor_sz(GLFWmonitor *mon, int *width, int *height) const -> void;

  auto inline get_window(void) const -> GLFWwindow * { return m_win; }

  ~Window();

private:
  /// Member variables ----------------------------------------------- //

  /// width of window
  int m_width;
  /// height of window
  int m_height;
  /// title of window
  std::string m_win_name;
  /// GLFWwindow created in initWindow
  GLFWwindow *m_win;
  /// Monitors struct that holds details about monitors
  Monitors m_monitors;
  // TODO: Add icon for window

  // Member functions ------------------------------------------------ //

  /*
      Creates glfw window (called from constructor)
      @param mon Pointer to a GLFWmonitor. Default nullptr
      @param win pointer to a GLFWwindow. Default nullptr
      @return void

  */
  auto init_window(GLFWmonitor *mon = nullptr, GLFWwindow *win = nullptr)
      -> void;

  auto init_monitors(void) -> void;
};
} // namespace mini_engine