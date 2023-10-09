#pragma once
// Library
#include <X11/Xlib.h>

// System
#include <optional>
#include <string>

namespace mini_engine {

class Window_T {
public:
  explicit Window_T(int w, int h, std::string title, bool debug = false);

  Window_T(const Window_T &) = delete;

  Window_T &operator=(const Window_T &) = delete;

  // mask given to have x11 to report events ex
  //
  // KeyPressMask, KeyReleaseMask, ButtonPressMask, ButtonReleaseMask,
  // EnterWindowMask LeaveWindowMask, PointerMotionMask, PointerMotionHintMask,
  // Button1MotionMask Button2MotionMask, Button3MotionMask, Button4MotionMask,
  // Button5MotionMask, KeyMapState Mask, ExposureMask, VisibilityChangeMask,
  // StructureNotifiyMask, ResizeRedirectMask, SubstructureNotifyMask,
  // SubstructureRedirectMask, FocusChangeMask, PropertyChangeMask,
  // ColormapChangeMask, OwnerGrabButtonMask
  [[nodiscard("possible return value is BadWindow: A value for a Window "
              "argument does not name a defined window")]] 
  auto set_attributes(long attr) const -> bool;

  // handling event should be for another class to handle. Might change later
  [[nodiscard("returns an optional event")]] 
  auto get_event(void) -> std::optional<XEvent>;

  // check if window manager destroyed window
  [[nodiscard("returns bool to see if wm destroyed window")]]
  auto check_destroy_win(const XEvent &event) const -> bool;

  ~Window_T();

private:
  /// Member variables ----------------------------------------------- //

  /// width of window
  int m_width;
  /// height of window
  int m_height;
  /// title of window
  std::string m_win_name;
  /// X11 window main
  Window m_window;
  /// X11display
  Display *m_display;
  /// Monitor
  Screen *m_screen;
  /// number of screens
  int m_num_screens;
  /// Close Window message
  Atom m_wm_delete;
  /// Graphic Context
  GC m_gc; // may remove this when vulcan is set up
  /// if debugging output to console
  bool m_debug;
  // TODO: Add icon for window

  // Member functions ------------------------------------------------ //
};
} // namespace mini_engine