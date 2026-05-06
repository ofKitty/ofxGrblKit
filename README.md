# ofxGrblKit

> Part of the [ofKitty](https://github.com/ofkitty/openFrameworks) distribution of openFrameworks.

Optional **ofKitty** integration for [**ofxGrbl**](https://github.com/ofxyz/ofxGrbl): ImGui windows that register with [`ofxKit`](https://github.com/ofkitty/ofxKit) so GRBL-related UI appears in the **View** menu beside built-in panels.

Core motion and serial code stays in `ofxGrbl`; this addon only adds UI glue.

---

## What it provides

- **`PlotterSerialWindow`** — COM/baud selection, connect/disconnect, firmware presets, serial console, and work-envelope editing backed by `grbl::MachinePrefs`
- **`grbl::kit::registerWindow`** — registers any window with `ofkitty::Runtime` under the **View** menu

---

## Dependencies

Declared in `addon_config.mk`:

| Addon       | Role                                                |
| ----------- | --------------------------------------------------- |
| `ofxGrbl`   | `GrblSender`, `MachinePrefs`, serial protocol       |
| `ofxKit`    | Edit overlay, window registry, `ofkitty::runtime()` |

> `ofxKit` pulls `ofxEnTTKit`, `ofxEnTTInspector`, and `ofxImGui`

---

## Usage

Include the umbrella header and wire objects in `setup()` after [`ofkitty::Runtime::attach`](https://github.com/ofkitty/ofxKit) (see **main.cpp** below):

```cpp
#include "ofxGrblKit.h"

grbl::GrblSender sender;
grbl::MachinePrefs prefs;
grbl::kit::PlotterSerialWindow serialWindow;

void ofApp::setup() {
    prefs.load(ofToDataPath(grbl::MachinePrefs::defaultRelativePath(), true));

    serialWindow.setSender(&sender);
    serialWindow.setPrefs(&prefs);
    serialWindow.refreshDeviceList();
    serialWindow.syncSelectionFromPrefs();

    grbl::kit::registerWindow(ofkitty::runtime(), serialWindow);
}

void ofApp::update() {
    sender.update();
}
```
