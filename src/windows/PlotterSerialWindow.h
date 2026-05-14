#pragma once

#include "ofSerial.h"
#include "ofxGrbl.h"

#include <string>
#include <vector>

namespace grbl::kit {

/// COM/baud connect + machine work envelope UI for shared GrblSender + MachinePrefs.
class PlotterSerialWindow {
public:
    PlotterSerialWindow();

    const std::string& name() const { return name_; }
    bool isVisible() const { return visible_; }
    void setVisible(bool visible) { visible_ = visible; }
    bool& visible() { return visible_; }

    void setSender(grbl::GrblSender* sender) { sender_ = sender; }
    void setPrefs(grbl::MachinePrefs* prefs) { prefs_ = prefs; }

    void draw();
    void draw(bool& visible);

    /// Refresh device list. Call once at startup after prefs are loaded.
    void refreshDeviceList();

    /// Match Combo index to prefs serialDevicePath after refresh.
    void syncSelectionFromPrefs();

    /// ImGui window title passed to ImGui::Begin, including any `###id` suffix
    /// required for docking (must match ofxKit registerWindow id). Empty uses name().
    void setImguiWindowTitle(std::string title) { imguiWindowTitle_ = std::move(title); }

private:
    void savePrefs();
    void ensureSelectedPortInRange();

    std::string name_ {"Serial / Machine"};
    std::string imguiWindowTitle_;
    bool visible_ {true};

    grbl::GrblSender* sender_ {nullptr};
    grbl::MachinePrefs* prefs_ {nullptr};

    std::vector<ofSerialDeviceInfo> deviceList_;
    int selectedPort_ {0};

    static constexpr int kNumBaudPresets = 6;
    static const int kBaudPresets[kNumBaudPresets];

    bool prefsDirty_ {false};
};

} // namespace grbl::kit
