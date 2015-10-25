#include "ofApp.h"

#define kVolHistorySize (400)

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0, 0, 0);

    sample_rate_ = 44100;

    // 0 output channels,
    // 2 input channels
    // 44100 samples per second
    // N samples per buffer
    // 4 num buffers (latency)
    ofSoundStreamSetup(0, 2, this, sample_rate_, beat_.getBufferSize(), 4);

    font_.loadFont("Batang.ttf", 160, true, true, true);

    gist_.setUseForOnsetDetection(GIST_PEAK_ENERGY);
    gist_.setThreshold(GIST_PEAK_ENERGY, .05);

    // Volume
    left.assign(beat_.getBufferSize(), 0.0);
    right.assign(beat_.getBufferSize(), 0.0);
    vol_history_.assign(kVolHistorySize, 0.0);
    current_vol_ = 0;
    smoothed_vol_     = 0.0;
    scaled_vol_		= 0.0;

    ofAddListener(GistEvent::ON,this,&ofApp::onNoteOn);
    ofAddListener(GistEvent::OFF,this,&ofApp::onNoteOff);

    // onset detection
    onset_decay_rate_ = 0.05;
    onset_minimum_threshold_ = 0.1;
    onset_threshold_ = onset_minimum_threshold_;

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
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255, 255, 255);
    ofFill();

    ofDrawBitmapString("onset threshold: "+ofToString(onset_threshold_), 10, 20);
    ofDrawBitmapString("kick: "+ofToString(beat_.kick()), 10, 40);
    ofDrawBitmapString("snare: "+ofToString(beat_.snare()), 10, 60);
    ofDrawBitmapString("hihat: "+ofToString(beat_.hihat()), 10, 80);
    ofDrawBitmapString("current vol: "+ofToString(current_vol_), 10, 100);
    ofDrawBitmapString("smoothed vol: "+ofToString(smoothed_vol_), 10, 120);

    const int kNumberOfBands = 32;

    for (int i = 0; i < kNumberOfBands; i++) {
        float selectedBand = beat_.getBand(i);
        float hz = ((i+1) * sample_rate_) / beat_.getBufferSize();
        std::string text = ofToString(i) + ") " + ofToString(hz) + " hz " + ofToString(selectedBand);
        ofDrawBitmapString(text, 10, 140 + (20*i));
    }

    // draw the average volume:
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(565, 170, 0);

    ofSetColor(225);
    ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(scaled_vol_ * 100.0, 0), 4, 18);
    ofRect(0, 0, 400, 400);

    ofSetColor(245, 58, 135);
    ofFill();
    ofCircle(200, 200, scaled_vol_ * 190.0f);

    //lets draw the volume history as a graph
    ofBeginShape();
    for (unsigned int i = 0; i < vol_history_.size(); i++){
        if( i == 0 ) ofVertex(i, 400);

        ofVertex(i, 400 - vol_history_[i] * 70);

        if( i == vol_history_.size() -1 ) ofVertex(i, 400);
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
    //noteOnRadius = 100;
};

void ofApp::onNoteOff(GistEvent &e){
    //noteOnRadius = 0;
};

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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