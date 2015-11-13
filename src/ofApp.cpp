#include "ofApp.h"

#define kVolHistorySize (400)
#define kNumberOfBands (32)

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0, 0, 0);

    loudest_band_ = 0;

    sample_rate_ = 44100;

    // 0 output channels,
    // 2 input channels
    // 44100 samples per second
    // N samples per buffer
    // 4 num buffers (latency)
    ofSoundStreamSetup(0, 2, this, sample_rate_, beat_.getBufferSize(), 4);

    // configure Gist.
    gist_.setUseForOnsetDetection(GIST_PEAK_ENERGY);
    gist_.setThreshold(GIST_PEAK_ENERGY, .05);

    // detect all Gist features
    gist_.setDetect(GIST_PITCH);
    gist_.setDetect(GIST_NOTE);
    gist_.setDetect(GIST_ROOT_MEAN_SQUARE);
    gist_.setDetect(GIST_PEAK_ENERGY);
    gist_.setDetect(GIST_SPECTRAL_CREST);
    gist_.setDetect(GIST_ZERO_CROSSING_RATE);
    gist_.setDetect(GIST_SPECTRAL_CENTROID);
    gist_.setDetect(GIST_SPECTRAL_FLATNESS);
    gist_.setDetect(GIST_SPECTRAL_DIFFERENCE);
    gist_.setDetect(GIST_SPECTRAL_DIFFERENCE_COMPLEX);
    gist_.setDetect(GIST_SPECTRAL_DIFFERENCE_HALFWAY);
    gist_.setDetect(GIST_HIGH_FREQUENCY_CONTENT);

    // Volume
    left.assign(beat_.getBufferSize(), 0.0);
    right.assign(beat_.getBufferSize(), 0.0);
    vol_history_.assign(kVolHistorySize, 0.0);
    current_vol_ = 0;
    smoothed_vol_     = 0.0;
    scaled_vol_		= 0.0;

    // Enable beat detection
    beat_.enableBeatDetect();

    // Listen to Gist events
    ofAddListener(GistEvent::ON,this,&ofApp::onNoteOn);
    ofAddListener(GistEvent::OFF,this,&ofApp::onNoteOff);

    // onset detection
    onset_decay_rate_ = 0.05;
    onset_minimum_threshold_ = 0.1;
    onset_threshold_ = onset_minimum_threshold_;

    // setup fonts
    ofTrueTypeFont::setGlobalDpi(72);

    hudFont.loadFont("verdana.ttf", 12, true, true);
    //hudFont.setLineHeight(14.0f);
    //hudFont.setLetterSpacing(1.037);

    setup_done_ = true;
}

//--------------------------------------------------------------
void ofApp::update(){
    beat_.update(ofGetElapsedTimeMillis());

    //lets scale the vol up to a 0-1 range
    scaled_vol_ = ofMap(smoothed_vol_, 0.0, 0.17, 0.0, 1.0, true);

    //lets record the volume into an array
    vol_history_.push_back(scaled_vol_);

    //if we are bigger the the size we want to record - lets drop the oldest value
    if( vol_history_.size() >= kVolHistorySize ){
        vol_history_.erase(vol_history_.begin(), vol_history_.begin()+1);
    }

    // find loudest band
    float max = 0;
    int new_loudest = -1;
    for (int i = 0; i < kNumberOfBands; i++) {
        float selectedBand = beat_.getBand(i);
        if (selectedBand > max) {
            max = selectedBand;
            new_loudest = i;
        }
    }
    loudest_band_ = new_loudest;
}

//--------------------------------------------------------------
void ofApp::draw(){
    // Draw black background
    ofSetColor(255, 255, 255);
    ofFill();

    if (draw_hud_) {
        drawHUD();
    }
}

void ofApp::drawHUD() {
    // FIXME: show beat ranges too (have couple of predefined ranges or
    // something, for example 0-10, 20-32 etc.

    // draw debug variables
    hudFont.drawString("onset threshold: "+ofToString(onset_threshold_), 10, 20);
    hudFont.drawString("kick: "+ofToString(beat_.kick()), 10, 40);
    hudFont.drawString("snare: "+ofToString(beat_.snare()), 10, 60);
    hudFont.drawString("hihat: "+ofToString(beat_.hihat()), 10, 80);
    hudFont.drawString("current vol: "+ofToString(current_vol_), 10, 100);
    hudFont.drawString("smoothed vol: "+ofToString(smoothed_vol_), 10, 120);

    // kick bar
    ofSetColor(102, 102, 255);
    ofNoFill();
    ofRect(150, 30, 100, 10);
    ofFill();
    ofRect(150, 30, ofMap(beat_.kick(), 0, 1, 0, 100), 10);

    // snare bar
    ofSetColor(102, 102, 255);
    ofNoFill();
    ofRect(150, 50, 100, 10);
    ofFill();
    ofRect(150, 50, ofMap(beat_.snare(), 0, 1, 0, 100), 10);

    // hihat bar
    ofSetColor(102, 102, 255);
    ofNoFill();
    ofRect(150, 70, 100, 10);
    ofFill();
    ofRect(150, 70, ofMap(beat_.hihat(), 0, 1, 0, 100), 10);

    // Draw Gist onset event info
    ofSetColor(255, 255, 255);
    hudFont.drawString("GIST ONSET EVENT", 300, 20);
    hudFont.drawString("energy: "+ofToString(gist_event_energy_), 300, 40);
    hudFont.drawString("frequency: "+ofToString(gist_event_frequency_), 300, 60);
    hudFont.drawString("onset amount: "+ofToString(gist_event_onset_amount_), 300, 80);
    hudFont.drawString("note on: "+ofToString(gist_event_note_on_), 300, 100);

    // Key hints
    hudFont.drawString("press 'h' to toggle HUD", 300, 140);

    // Draw Gist info
    ofSetColor(255, 255, 255);
    hudFont.drawString("GIST FEATURES", 300, 180);
    hudFont.drawString("pitch: "+ofToString(gist_.getValue(GIST_PITCH)), 300, 200);
    hudFont.drawString("note: "+ofToString(gist_.getValue(GIST_NOTE)), 300, 220);
    hudFont.drawString("root mean square: "+ofToString(gist_.getValue(GIST_ROOT_MEAN_SQUARE)), 300, 240);
    hudFont.drawString("peak energy: "+ofToString(gist_.getValue(GIST_PEAK_ENERGY)), 300, 260);
    hudFont.drawString("special crest: "+ofToString(gist_.getValue(GIST_SPECTRAL_CREST)), 300, 280);
    hudFont.drawString("zero crossing rate: "+ofToString(gist_.getValue(GIST_ZERO_CROSSING_RATE)), 300, 300);
    hudFont.drawString("SPECTRAL FEATURES", 300, 320);
    hudFont.drawString("centroid: "+ofToString(gist_.getValue(GIST_SPECTRAL_CENTROID)), 300, 340);
    hudFont.drawString("flatness: "+ofToString(gist_.getValue(GIST_SPECTRAL_FLATNESS)), 300, 360);
    hudFont.drawString("difference: "+ofToString(gist_.getValue(GIST_SPECTRAL_DIFFERENCE)), 300, 380);
    hudFont.drawString("difference complex: "+ofToString(gist_.getValue(GIST_SPECTRAL_DIFFERENCE_COMPLEX)), 300, 400);
    hudFont.drawString("difference halfway: "+ofToString(gist_.getValue(GIST_SPECTRAL_DIFFERENCE_HALFWAY)), 300, 420);
    hudFont.drawString("high freq. content: "+ofToString(gist_.getValue(GIST_HIGH_FREQUENCY_CONTENT)), 300, 440);

    // Draw individual bands as bars, also show frequencies and values
    // for each band.
    ofPushStyle();
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofSetLineWidth(2);
    const int kVolumeRange = 1;
    for (int i = 0; i < kNumberOfBands; i++) {
        float selectedBand = beat_.getBand(i);

        ofSetColor(255, 255, 255);

        float hz = ((i+1) * sample_rate_) / beat_.getBufferSize();
        std::string text = ofToString(i) + ") " + ofToString(hz) + " hz " + ofToString(selectedBand);
        hudFont.drawString(text, 10, 140 + (20*i));

        if (i== loudest_band_) {
            ofSetColor(255, 0, 10);
        } else {
            ofSetColor(51, 204, 51);
        }
        if (beat_.isBeat(i)) {
            ofFill();
        } else {
            ofNoFill();
        }
        float x = ofGetWidth()*((float)i/kNumberOfBands);
        float y = ofGetHeight()-20;
        float w = ofGetWidth()/kNumberOfBands;
        float h = -ofMap(selectedBand, 0, kVolumeRange, 0, ofGetHeight() / 10);
        ofRect(x, y, w, h);
    }
    ofPopStyle();

    ofNoFill();

    // draw the left channel:
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(565, 20, 0);

    ofSetColor(225);
    hudFont.drawString("Left Channel", 4, 18);

    ofSetLineWidth(1);
    ofRect(0, 0, 440, 200);

    ofSetColor(245, 58, 135);
    ofSetLineWidth(3);

    ofBeginShape();
    for (unsigned int i = 0; i < left.size(); i++){
        ofVertex(i*2, 100 -left[i]*180.0f);
    }
    ofEndShape(false);

    ofPopMatrix();
    ofPopStyle();

    // draw the right channel:
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(565, 220, 0);

    ofSetColor(225);
    hudFont.drawString("Right Channel", 4, 18);

    ofSetLineWidth(1);
    ofRect(0, 0, 440, 200);

    ofSetColor(245, 58, 135);
    ofSetLineWidth(3);

    ofBeginShape();
    for (unsigned int i = 0; i < right.size(); i++){
        ofVertex(i*2, 100 -right[i]*180.0f);
    }
    ofEndShape(false);

    ofPopMatrix();
    ofPopStyle();

    // draw the average volume:
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(565, 420, 0);

    ofSetColor(225);
    hudFont.drawString("Scaled average vol (0-100): " + ofToString(scaled_vol_ * 100.0, 0), 4, 18);

    ofSetColor(245, 58, 135);
    ofFill();
    ofCircle(200, 100, scaled_vol_ * 100.0f);

    //lets draw the volume history as a graph
    ofNoFill();
    ofBeginShape();
    for (unsigned int i = 0; i < vol_history_.size(); i++){
        if( i == 0 ) ofVertex(i, 300);

        ofVertex(i, 300 - vol_history_[i] * 70);

        if( i == vol_history_.size() -1 ) ofVertex(i, 300);
    }
    ofEndShape(false);

    ofPopMatrix();
    ofPopStyle();
}

void ofApp::audioReceived(float* input, int bufferSize, int nChannels) {
    if (!setup_done_) {
        return;
    }

    beat_.audioReceived(input, bufferSize, channel_count_);

    channel_count_ = nChannels;

    //convert float array to vector
    vector<float>buffer;
    buffer.assign(&input[0],&input[bufferSize]);
    gist_.processAudio(buffer, bufferSize, nChannels, sample_rate_);

    calculateVolume(input, bufferSize);

    // detect onset
    onset_threshold_ = ofLerp(onset_threshold_, onset_minimum_threshold_, onset_decay_rate_);
    if (current_vol_ > onset_threshold_) {
        // onset detected!
        onset_threshold_ = current_vol_;
    }
}

void ofApp::calculateVolume(float *input, int bufferSize) {
    // samples are "interleaved"
    int numCounted = 0;

    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (int i = 0; i < bufferSize; i++){
        left[i]		= input[i*2]*0.5;
        right[i]	= input[i*2+1]*0.5;

        current_vol_ += left[i] * left[i];
        current_vol_ += right[i] * right[i];
        numCounted+=2;
    }

    //this is how we get the mean of rms :)
    current_vol_ /= (float)numCounted;

    // this is how we get the root of rms :)
    current_vol_ = sqrt( current_vol_ );

    smoothed_vol_ *= 0.93;
    smoothed_vol_ += 0.07 * current_vol_;
}

void ofApp::onNoteOn(GistEvent &e){
    gist_event_energy_ = e.energy;
    gist_event_frequency_ = e.frequency;
    gist_event_note_ = e.note;
    gist_event_onset_amount_ = e.onsetAmount;

    gist_event_note_on_ = true;
};

void ofApp::onNoteOff(GistEvent &e){
    gist_event_energy_ = e.energy;
    gist_event_frequency_ = e.frequency;
    gist_event_note_ = e.note;
    gist_event_onset_amount_ = e.onsetAmount;

    gist_event_note_on_ = false;
};

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if ('h' == key) {
        draw_hud_ = !draw_hud_;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}