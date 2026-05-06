#pragma once

#include "ofMain.h"
#include "ofxKit.h"
#include "ofxGrblKit.h"
#include "ofxGrbl.h"

/// Minimal ofKitty + ofxGrblKit demo: serial/machine UI from the View menu (Cmd/Ctrl-E for Edit mode).
class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;

private:
    grbl::GrblSender sender_;
    grbl::MachinePrefs prefs_;
    grbl::kit::PlotterSerialWindow serialWindow_;

    static constexpr float kStatusPollIntervalSec = 0.1f;
    float lastStatusTime_ = 0.f;
};
