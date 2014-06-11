#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    player.loadMovie("test04.mp4");
    player.play();

    origImage.allocate(640,480);

    //ww.allocate(50,200);
    //ww.setImage(origImage);
    //we.addWindow(ww);
    we.setImage(origImage);
    we.loadFile("test.xml");
    we.displayRect.set(200,10,400,400);
}

//--------------------------------------------------------------
void ofApp::update(){
        player.update();
        if(player.isFrameNew()){
            origImage.setFromPixels(player.getPixels(), player.getWidth(),player.getHeight());

            for (vector<warpWindow*>::iterator it = we.windows.begin();it != we.windows.end();it++){
                (*it)->warp();
            }
        }
}

//--------------------------------------------------------------
void ofApp::draw(){
    player.draw(0,0,133,100);
    we.draw();
    ofPoint offset(100,400);
    for(int i=0;i<we.windows.size();i++){
       warpWindow& ww = *we.windows[i];
       ww.draw(offset);
       offset.x+=ww.getWidth();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='s'){
        we.saveFile("test.xml");
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
