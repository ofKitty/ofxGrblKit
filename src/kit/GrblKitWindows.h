#pragma once

#include "ofxKit.h"

namespace grbl::kit {

/// Register any window object with an ofkitty::Runtime.
///
/// Requirements on Window:
///   std::string  name()      const  — window title
///   bool         isVisible() const  — initial visibility
///   void         draw(bool& visible)— ImGui draw call
///
/// The window is placed in the "View" menu and is always drawn
/// (editModeOnly = false).
template<typename Window>
inline ofkitty::Runtime::RuntimeWindow* registerWindow(
    ofkitty::Runtime& runtime,
    Window&           window)
{
    return runtime.registerWindow({
        window.name(),
        "View",
        window.isVisible(),
        false,
        [&window](bool& visible) {
            window.draw(visible);
        },
    });
}

} // namespace grbl::kit
