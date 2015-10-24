#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(255,255,255);

    sampleRate = 44100;

    ofSoundStreamSetup(0, 1, this, sampleRate, beat.getBufferSize(), 4);

    font.loadFont("Batang.ttf", 160, true, true, true);

    gist.setUseForOnsetDetection(GIST_PEAK_ENERGY);
    gist.setThreshold(GIST_PEAK_ENERGY, .05);

    ofAddListener(GistEvent::ON,this,&ofApp::onNoteOn);
    ofAddListener(GistEvent::OFF,this,&ofApp::onNoteOff);
}

//--------------------------------------------------------------
void ofApp::update(){
    beat.update(ofGetElapsedTimeMillis());
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(0, 90, 60);
    ofFill();

    ofDrawBitmapString("fps: "+ofToString(ofGetFrameRate()), 10, 20);
    ofDrawBitmapString("kick: "+ofToString(beat.kick()), 10, 40);
    ofDrawBitmapString("snare: "+ofToString(beat.snare()), 10, 60);
    ofDrawBitmapString("hihat: "+ofToString(beat.hihat()), 10, 80);
    ofDrawBitmapString("volume: FIXME:", 10, 100);
    ofDrawBitmapString("bands: FIXME:", 10, 120);
}

void ofApp::audioReceived(float* input, int bufferSize, int nChannels) {
    beat.audioReceived(input, bufferSize, nChannels);

    //convert float array to vector
    vector<float>buffer;
    buffer.assign(&input[0],&input[bufferSize]);
    gist.processAudio(buffer, bufferSize, nChannels, sampleRate);
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