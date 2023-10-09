// System
#include <X11/X.h>
#include <X11/Xlib.h>
#include <cstdio>
#include <optional>
// User-Defined
#include "colorIO.hpp"
#include "window.hpp"

namespace mini_engine {
// Public
Window_T::Window_T(int w, int h, std::string name)
    : m_width{w}, m_height{h}, m_win_name{name} {
  // TODO: handle possible errors of the below initializations
  m_display = XOpenDisplay(nullptr);
  auto screen = DefaultScreen(m_display);
  m_screen = XDefaultScreenOfDisplay(m_display);
  m_window = XCreateSimpleWindow(m_display, RootWindow(m_display, screen), 0, 0,
                                 800, 600, 1, XBlackPixel(m_display, screen),
                                 XWhitePixel(m_display, screen));
  XMapWindow(m_display, m_window);
  // Avoids X connection to :0 broken (explicit kill or server shutdown).
  // Window manager handles closing of window but the window is unaware
  // of the closing
  m_wm_delete = XInternAtom(m_display, "WM_DELETE_WINDOW", false);
  XSetWMProtocols(m_display, m_window, &m_wm_delete, 1);
  m_gc = XCreateGC(m_display, m_window, 0, nullptr);
  XSetForeground(m_display, m_gc, WhitePixel(m_display, screen));
  m_num_screens = XScreenCount(m_display);
  fprintf(stderr,
          "%s (monitor) size width: %d, height: %d, (window) width: %d, "
          "height: %d, Number of screens: %d\n",
          CGRN("[Success]"), m_screen->width, m_screen->height, m_width,
          m_height, m_num_screens);
}

Window_T::~Window_T() {
  if (XDestroyWindow(m_display, m_window) == BadWindow) {
    fprintf(stderr,
            "%sWindow_T: Window Destructor Called. XDestroyWindow return "
            "BadWindow\n",
            CYEL("[Warning] "));
    return;
  }
  fprintf(stderr, "%sWindow Destructor Called\n", CGRN("Window_T: "));
}

auto Window_T::set_attributes(long attr) const -> bool {
  auto check = XSelectInput(m_display, m_window, attr);
  // If error (possible return value is BadWindow: A value for a Window argument
  // does not name a defined window) However this might not happen since the
  // user does not have access to the window directly and if window
  // initialization fails the class should error out. Added todos
  if (check == BadWindow) {
    fprintf(stderr, "%sWindow possibly not defined when setting attributes\n",
            CRED("Warning: Window_T: "));
    return false;
  }
  return true;
}

auto Window_T::get_event() -> std::optional<XEvent> {
  std::optional<XEvent> evt;
  if (XPending(m_display)) {
    XEvent event;
    XNextEvent(m_display, &event);
    evt = event;
    return evt;
  }
  return evt;
}

auto Window_T::check_destroy_win(const XEvent &event) const -> bool {
  if (m_wm_delete ==
      static_cast<const long unsigned int>(event.xclient.data.l[0]))
    return true;
  return false;
}
// Private

} // namespace mini_engine