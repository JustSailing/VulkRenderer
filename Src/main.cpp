#include "colorIO.hpp"
#include "shaders.hpp"
#include "window.hpp"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <cstdio>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <vulkan/vulkan.hpp>

auto main(int argc, char **argv) -> int {
  mini_engine::Window_T window(800, 600, "Vulkan Window");
  if (!window.set_attributes(KeyPressMask | KeyReleaseMask |
                             StructureNotifyMask))
    std::cout << "setting attributes for X11 window failed\n";
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::cout << extensionCount << " Extensions supported\n";

  glm::mat4 matrix;
  glm::vec4 vec;
  auto test = matrix * vec;
  mini_engine::Shader_Comp comp{"../Vendor/Executable/glslc", "../Shader/Input",
                                "../Shader/Output"};

  while (1) {
    std::optional<XEvent> event = window.get_event();
    if (event.has_value()) {
      if (event->type == ClientMessage) {
        if (window.check_destroy_win(*event)) {
          fprintf(stderr, "%sDestroyed by WM\n", CGRN("WINDOW: "));
          break;
        }
      }
      fprintf(stderr, "%s%d\nKeyPressEvent type %d\n", CGRN("EVENT_Type: "),
              event->type, event->xkey.keycode);
    }
  }

  return 0;
}