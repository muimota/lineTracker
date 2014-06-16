#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){




    player.loadMovie("test04.mp4");
    player.play();


    videoImage.allocate(640,480);
    videoGrayImage.allocate(640,480);

    we.setImage(videoGrayImage);
    we.loadFile("test.xml");
    we.displayRect.set(200,10,400,400);

    //init contourFinders
    for(int i=0;i<we.windows.size();i++){
        ofxCvContourFinder *contour = new ofxCvContourFinder();
        contourFinders.push_back(contour);
    }

	parameters.setName("Parameters");
	useBackground.set("Background Substraction",false);
	threshold.set("Threshold",10,0,255);
	minBlobArea.set("Minimum Blob Area",30,0,(int)BLOB_SCALE);
	maxBlobArea.set("Maximum Blob Area",90,0,(int)BLOB_SCALE);

    parameters.add(threshold);
	parameters.add(minBlobArea);
	parameters.add(maxBlobArea);
    parameters.add(useBackground);

	//gui
	gui.setup(parameters);
	gui.add(grabBackground.setup("grab Background"));
    gui.loadFromFile("settings.xml");
    gui.setPosition(420,360);
}

//--------------------------------------------------------------
void ofApp::update(){
        player.update();
        if(player.isFrameNew()){

            videoImage.setFromPixels(player.getPixels(), player.getWidth(),player.getHeight());
            videoGrayImage = videoImage;
            //warp images
            for (vector<warpWindow*>::iterator it = we.windows.begin();it != we.windows.end();it++){
                (*it)->warp();
            }
            //image substraction
            for(int i=0;i<diffImages.size();i++){
                int videoArea = diffImages[i].getWidth() * diffImages[i].getHeight();

                diffImages[i].absDiff(*we.windows[i],bgImages[i]);
                diffImages[i].threshold(threshold);
                contourFinders[i]->findContours(diffImages[i],videoArea*minBlobArea/BLOB_SCALE, videoArea*maxBlobArea/BLOB_SCALE,10,false);
            }
        }
}

//--------------------------------------------------------------
void ofApp::draw(){
    player.draw(0,0,133,100);
    we.draw();

    //drawing position
    ofPoint offset(100,400);
    for(int i=0;i<we.windows.size();i++){
       warpWindow& ww = *we.windows[i];
       ww.draw(offset);
       offset.x+=ww.getWidth();
    }

    //drawing position
    offset = ofPoint(305,400);
    for(int i=0;i<bgImages.size();i++){
       ofxCvImage& img = bgImages[i];
       img.draw(offset);
       offset.x+=img.getWidth();
    }

    //drawing position
    offset = ofPoint(510,400);
    for(int i=0;i<diffImages.size();i++){
       ofxCvImage& img = diffImages[i];
       img.draw(offset);
       ofRectangle displayRect(offset.x,offset.y,img.getWidth(),img.getHeight());
       contourFinders[i]->draw(displayRect);
       offset.x+=img.getWidth();
    }
    gui.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='s'){
        we.saveFile("test.xml");
    }

    if(key=='b'){
        bgImages.clear();
        diffImages.clear();
        for(int i=0;i<we.windows.size();i++){
           warpWindow& ww = *we.windows[i];
           //putting copies in the vectors
           bgImages.push_back(ww);
           diffImages.push_back(ww);
        }
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
