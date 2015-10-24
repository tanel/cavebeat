#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(255,255,255);

    sample_rate_ = 44100;

    ofSoundStreamSetup(0, 1, this, sample_rate_, beat_.getBufferSize(), 4);

    font_.loadFont("Batang.ttf", 160, true, true, true);

    gist_.setUseForOnsetDetection(GIST_PEAK_ENERGY);
    gist_.setThreshold(GIST_PEAK_ENERGY, .05);

    ofAddListener(GistEvent::ON,this,&ofApp::onNoteOn);
    ofAddListener(GistEvent::OFF,this,&ofApp::onNoteOff);
}

//--------------------------------------------------------------
void ofApp::update(){
    beat_.update(ofGetElapsedTimeMillis());
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(0, 90, 60);
    ofFill();

    ofDrawBitmapString("buffer size: "+ofToString(buffer_size_), 10, 20);
    ofDrawBitmapString("kick: "+ofToString(beat_.kick()), 10, 40);
    ofDrawBitmapString("snare: "+ofToString(beat_.snare()), 10, 60);
    ofDrawBitmapString("hihat: "+ofToString(beat_.hihat()), 10, 80);
    ofDrawBitmapString("channel count: "+ofToString(channel_count_), 10, 100);

    const int kNumberOfBands = 32;

    for (int i = 0; i < kNumberOfBands; i++) {
        float selectedBand = beat_.getBand(i);
        float hz = (i * sample_rate_) / buffer_size_;
        std::string text = ofToString(i) + ") " + ofToString(hz) + " hz " + ofToString(selectedBand);
        ofDrawBitmapString(text, 10, 120 + (20*i));
    }
}

void ofApp::audioReceived(float* input, int bufferSize, int nChannels) {
    beat_.audioReceived(input, buffer_size_, channel_count_);

    buffer_size_ = bufferSize;
    channel_count_ = nChannels;

    //convert float array to vector
    vector<float>buffer;
    buffer.assign(&input[0],&input[bufferSize]);
    gist_.processAudio(buffer, bufferSize, nChannels, sample_rate_);
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