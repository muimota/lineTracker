#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    player.loadMovie("test04.mp4");
    player.play();

    origImage.allocate(640,480);


    ww.allocate(200,100);
    ww.setImage(origImage);
    we.windows.push_back(ww);

    we.displayRect.set(200,10,400,400);
}

//--------------------------------------------------------------
void ofApp::update(){
        player.update();
        if(player.isFrameNew()){
            origImage.setFromPixels(player.getPixels(), player.getWidth(),player.getHeight());
            ww.warp();
        }
}

//--------------------------------------------------------------
void ofApp::draw(){
    player.draw(0,0,133,100);
    we.draw();
    ww.draw(0,100);
}

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
    we.mouseDragged(x,y,button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    we.mousePressed(x,y,button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    we.mouseReleased(x,y,button);
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
