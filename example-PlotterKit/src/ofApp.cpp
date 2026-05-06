#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(60);
    ofBackground(34);

    prefs_.load(ofToDataPath(grbl::MachinePrefs::defaultRelativePath(), true));

    // Synthetic OK stream until you connect real hardware or disable in the UI.
    sender_.setSimulationMode(true);

    serialWindow_.setSender(&sender_);
    serialWindow_.setPrefs(&prefs_);
    serialWindow_.refreshDeviceList();
    serialWindow_.syncSelectionFromPrefs();

    grbl::kit::registerWindow(ofkitty::runtime(), serialWindow_);
}

void ofApp::update() {
    sender_.update();

    if (sender_.isConnected()) {
        float now = ofGetElapsedTimef();
        if (now - lastStatusTime_ > kStatusPollIntervalSec) {
            sender_.sendRealtimeStatusQuery();
            lastStatusTime_ = now;
        }
    }
}

void ofApp::draw() {
    const float pad = 18.f;
    ofSetColor(210);
    std::string line1 = "ofxGrblKit — example-PlotterKit";
    std::string line2 = "Toggle Edit overlay: Cmd-E or Ctrl-E  |  View menu: Serial / Machine";
    std::string line3 = "Simulation: "
                        + std::string(sender_.isSimulationMode() ? "on" : "off")
                        + "  |  Connected: "
                        + std::string(sender_.isConnected() ? "yes" : "no");
    ofDrawBitmapString(line1, pad, pad + 12);
    ofDrawBitmapString(line2, pad, pad + 28);
    ofDrawBitmapString(line3, pad, pad + 44);
}
