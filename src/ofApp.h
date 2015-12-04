#pragma once

#include "ofMain.h"

#include "ofxBeat.h"
#include "ofxGist.h"
#include "ofxFXObject.h"
#include "ofxBloom.h"
#include "ofxGaussianBlur.h"
#include "ofxBlur.h"
#include "ofxBokeh.h"
#include "ofxGlow.h"
#include "ofxOldTv.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp{

public:
    ofApp()
    : gist_event_energy_(0)
    , gist_event_frequency_(0)
    , gist_event_note_(0)
    , gist_event_onset_amount_(0)
    , gist_event_note_on_(false)
    , draw_hud_(true)
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
    void drawHUD();
    void setupEffects();

    ofxBeat beat_;
    int loudest_band_;

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

    // HUD can be turned off entirely
    bool draw_hud_;
    bool draw_video_;

    // Draw HUD texts using truetype font,
    // so we can blend it with gl or something
    ofTrueTypeFont hudFont;

    // Use FBO to put the picture together
    ofFbo fbo;

    // ofxFX setup
    ofxFXObject sandbox;
    ofxBloom    bloom;
    ofxGaussianBlur gaussianBlur;
    ofxBlur     blur;
    ofxBokeh    bokeh;
    ofxGlow     glow;
    ofxOldTv    oldtv;
    enum        { nTotalFrag = 19 };
    string      frags[ nTotalFrag ];
    string      fragsTitles[ nTotalFrag];
    float       beat;
    int         nFrag, selection;

    // Video grabber
    ofVideoGrabber myVideoGrabber;
    ofTexture myTexture;
    unsigned char* invertedVideoData;
    int camWidth;
    int camHeight;

    //
    ofxCv::ContourFinder contourFinder;

    bool setup_done_;
};
