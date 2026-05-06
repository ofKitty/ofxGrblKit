#include "PlotterSerialWindow.h"

#include "imgui.h"
#include "ofMain.h"

#include <algorithm>

namespace grbl::kit {

const int PlotterSerialWindow::kBaudPresets[] = {
    9600, 19200, 38400, 57600, 115200, 250000,
};

PlotterSerialWindow::PlotterSerialWindow() = default;

void PlotterSerialWindow::refreshDeviceList()
{
    ofSerial tmp;
    deviceList_ = tmp.getDeviceList();

    // Sort so real USB-serial devices come first. On Linux/WSL this puts
    // /dev/ttyUSB* and /dev/ttyACM* (real USB devices) above /dev/ttyS*
    // (virtual legacy COM stubs that almost never connect to anything).
    auto rank = [](const std::string& path) {
        if (path.find("ttyUSB") != std::string::npos) return 0;
        if (path.find("ttyACM") != std::string::npos) return 0;
        if (path.find("cu.usbserial") != std::string::npos) return 0; // macOS
        if (path.find("cu.usbmodem") != std::string::npos) return 0; // macOS
        if (path.find("COM") != std::string::npos) return 0;          // Windows
        if (path.find("ttyS") != std::string::npos) return 2;         // virtual / legacy
        return 1;
    };
    std::sort(deviceList_.begin(), deviceList_.end(),
        [&](ofSerialDeviceInfo a, ofSerialDeviceInfo b) {
            return rank(a.getDevicePath()) < rank(b.getDevicePath());
        });
}

void PlotterSerialWindow::syncSelectionFromPrefs()
{
    if (!prefs_ || prefs_->serialDevicePath.empty()) {
        return;
    }
    for (int i = 0; i < (int)deviceList_.size(); ++i) {
        if (deviceList_[i].getDevicePath() == prefs_->serialDevicePath) {
            selectedPort_ = i;
            break;
        }
    }
}

void PlotterSerialWindow::savePrefs()
{
    if (!prefs_) {
        return;
    }
    std::string path = ofToDataPath(grbl::MachinePrefs::defaultRelativePath(), true);
    prefs_->save(path);
    prefsDirty_ = false;
}

void PlotterSerialWindow::draw()
{
    draw(visible_);
}

void PlotterSerialWindow::draw(bool& visible)
{
    if (!visible) {
        visible_ = false;
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(340, 420), ImGuiCond_FirstUseEver);

    if (ImGui::Begin(name_.c_str(), &visible)) {
        if (ImGui::CollapsingHeader("USB serial", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::SmallButton("Refresh ports")) {
                refreshDeviceList();
            }

            const bool simulation = sender_ && sender_->isSimulationMode();
            const bool usbConnected = sender_ && sender_->isUsbConnected();
            const bool connected = sender_ && sender_->isConnected();

            if (simulation) {
                ImGui::TextColored(ImVec4(1.f, 0.85f, 0.35f, 1.f),
                    "Bench simulation: USB disabled (synthetic ok)");
            }

            if (deviceList_.empty()) {
                ImGui::TextDisabled("No serial ports found");
            } else {
                std::vector<const char*> names;
                names.reserve(deviceList_.size());
                for (auto& d : deviceList_) {
                    names.push_back(d.getDeviceName().c_str());
                }
                ImGui::Combo("Port", &selectedPort_, names.data(), (int)names.size());
            }

            if (prefs_) {
                int fw = prefs_->capabilities.supportsM114Position ? 1 : 0;
                if (ImGui::Combo("Firmware", &fw, "Grbl (plotter / cnc)\0"
                                                  "Marlin (Anycubic & most FDM)\0\0")) {
                    prefs_->capabilities = (fw == 0) ? grbl::MachineCapabilities::grbl()
                                                     : grbl::MachineCapabilities::marlin();
                    prefsDirty_ = true;
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Marlin: G0/G1 jogs; position via M114. Grbl: $?, $$, $J=.");
                }
                if (ImGui::InputInt("Baud rate", &prefs_->baudRate, 100, 1000)) {
                    prefsDirty_ = true;
                }
            }

            ImGui::TextDisabled("Presets:");
            for (int i = 0; i < kNumBaudPresets; ++i) {
                ImGui::SameLine();
                if (ImGui::SmallButton(ofToString(kBaudPresets[i]).c_str()) && prefs_) {
                    prefs_->baudRate = kBaudPresets[i];
                    prefsDirty_ = true;
                }
            }

            ImGui::Separator();

            bool simToggle = simulation;
            if (sender_ && ImGui::Checkbox("Bench simulation (no USB)", &simToggle)) {
                sender_->setSimulationMode(simToggle);
            }

            ImGui::Separator();

            if (!connected && !simulation) {
                if (ImGui::Button("Connect", ImVec2(-1, 0))) {
                    if (sender_ && prefs_ && !deviceList_.empty()
                        && selectedPort_ < (int)deviceList_.size()) {
                        prefs_->serialDevicePath = deviceList_[selectedPort_].getDevicePath();
                        prefs_->baudRate = std::max(9600, prefs_->baudRate);
                        bool ok = sender_->connectSerial(deviceList_[selectedPort_].getDevicePath(),
                            prefs_->baudRate);
                        if (ok) {
                            savePrefs();
                        }
                    }
                }
            } else if (usbConnected) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.25f, 0.25f, 1.f));
                if (ImGui::Button("Disconnect USB", ImVec2(-1, 0))) {
                    if (sender_) {
                        sender_->disconnectSerial();
                    }
                    savePrefs();
                }
                ImGui::PopStyleColor();

                ImGui::TextColored(ImVec4(0.5f, 1.f, 0.55f, 1.f), "USB connected");
            } else if (simulation) {
                ImGui::TextDisabled("Uncheck simulation to use USB");
            }
        }

        if (sender_ && ImGui::CollapsingHeader("Serial console")) {
            auto& lines = sender_->consoleLines();
            float h = std::max(120.f, ImGui::GetContentRegionAvail().y * 0.35f);
            if (ImGui::BeginChild("GrblSerialConsole", ImVec2(0, h), ImGuiChildFlags_Borders)) {
                for (auto& line : lines) {
                    ImGui::TextUnformatted(line.c_str());
                }
                if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 20.f) {
                    ImGui::SetScrollHereY(1.0f);
                }
            }
            ImGui::EndChild();
            if (ImGui::Button("Clear console", ImVec2(-1, 0))) {
                sender_->clearConsole();
            }
        }

        if (prefs_ && ImGui::CollapsingHeader("Work envelope (mm)", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (prefs_->capabilities.supportsM114Position) {
                ImGui::TextWrapped(
                    "Set this to your Anycubic bed + Z travel (slicer max). "
                    "Manual moves in the app are clamped to this box.");
            } else {
                ImGui::TextWrapped(
                    "Match firmware max travel ($130-$132) after you configure GRBL with $$ . "
                    "Manual moves are clamped to this box.");
            }

            if (ImGui::DragFloat3("Min X/Y/Z", &prefs_->envelope.minX, 0.5f, -500.f, 500.f, "%.2f")) {
                prefsDirty_ = true;
            }
            if (ImGui::DragFloat3("Max X/Y/Z", &prefs_->envelope.maxX, 0.5f, -500.f, 500.f, "%.2f")) {
                prefsDirty_ = true;
            }

            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.45f);
            if (ImGui::Button("Typical FDM 220x220x250 (Anycubic / Ender class)", ImVec2(-1, 0))) {
                prefs_->envelope.minX = 0.f;
                prefs_->envelope.minY = 0.f;
                prefs_->envelope.minZ = 0.f;
                prefs_->envelope.maxX = 220.f;
                prefs_->envelope.maxY = 220.f;
                prefs_->envelope.maxZ = 250.f;
                prefsDirty_ = true;
            }
            if (ImGui::Button("Reset A3 plotter (420x297 paper)", ImVec2(-1, 0))) {
                prefs_->envelope.minX = 0.f;
                prefs_->envelope.minY = 0.f;
                prefs_->envelope.minZ = -10.f;
                prefs_->envelope.maxX = 420.f;
                prefs_->envelope.maxY = 297.f;
                prefs_->envelope.maxZ = 40.f;
                prefsDirty_ = true;
            }
            ImGui::PopItemWidth();

            if (prefsDirty_ && ImGui::Button("Save settings to disk")) {
                savePrefs();
            }
        }
    }
    ImGui::End();

    visible_ = visible;
}

} // namespace grbl::kit
