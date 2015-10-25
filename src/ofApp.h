#pragma once

#include "ofMain.h"

#include "ofxBeat.h"
#include "ofxGist.h"

class ofApp : public ofBaseApp{

public:
    ofApp()
    : gist_event_energy_(0)
    , gist_event_frequency_(0)
    , gist_event_note_(0)
    , gist_event_onset_amount_(0)
    , gist_event_note_on_(false)
    , setup_done_(false) {}

    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void audioReceived(float*, int, int);

    void onNoteOn(GistEvent &e);
    void onNoteOff(GistEvent &e);

private:

    ofxBeat beat_;

    ofTrueTypeFont font_;

    ofxGist gist_;

    int sample_rate_;
    int channel_count_;

    void calculateVolume(float *input, int bufferSize);
    float current_vol_;
    float smoothed_vol_;
    float scaled_vol_;

    vector <float> left;
    vector <float> right;
    vector <float> vol_history_;

    float onset_threshold_;
    float onset_minimum_threshold_;
    float onset_decay_rate_;

    // gist event
    float gist_event_energy_;
    float gist_event_frequency_;
    float gist_event_note_;
    float gist_event_onset_amount_;
    bool gist_event_note_on_;

    bool setup_done_;
};
