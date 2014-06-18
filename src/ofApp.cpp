#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){




    player.loadMovie("test04.mp4");
    player.play();

    //if using camera v4l2-ctl -d 1 -c exposure_auto=1 to disable autoexposure
    devices = vidGrabber.listDevices();
    vidGrabber.setDeviceID(devices[deviceIndex].id);
	vidGrabber.setDesiredFrameRate(60);
	vidGrabber.initGrabber(1280,720);

    videoSource = &vidGrabber;


    videoImage.allocate(1280,720);
    videoGrayImage.allocate(1280,720);

    we.setImage(videoGrayImage);
    we.loadFile("test.xml");
    we.displayRect.set(200,10,600,338);

    //init contourFinders
    for(int i=0;i<we.windows.size();i++){
        ofxCvContourFinder *contour = new ofxCvContourFinder();
        contourFinders.push_back(contour);
    }

	parameters.setName("Parameters");
	useBackground.set("Background Substraction",false);
	threshold.set("Threshold",10,0,255);

	erode.set("Erode",0,0,10);
    dilate.set("Dilate",0,0,10);

	minBlobArea.set("Minimum Blob Area",30,0,(int)BLOB_SCALE);
	maxBlobArea.set("Maximum Blob Area",90,0,(int)BLOB_SCALE);

    parameters.add(threshold);
    parameters.add(erode);
	parameters.add(dilate);
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
        videoSource->update();
        if(videoSource->isFrameNew()){

            videoImage.setFromPixels(videoSource->getPixels(), videoSource->getWidth(),videoSource->getHeight());
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
                for(int i=0;i<dilate;i++){
                    diffImages[i].dilate();
                }
                for(int i=0;i<erode;i++){
                    diffImages[i].erode();
                }
                contourFinders[i]->findContours(diffImages[i],videoArea*minBlobArea/BLOB_SCALE, videoArea*maxBlobArea/BLOB_SCALE,10,false);
            }
        }
}

//--------------------------------------------------------------
void ofApp::draw(){
    videoSource->draw(0,0,133,100);
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

    if(key=='c'){
        vidGrabber.close();
        deviceIndex = (deviceIndex+1)%devices.size();
        vidGrabber.setDeviceID(devices[deviceIndex].id);

        vidGrabber.setDesiredFrameRate(30);
        vidGrabber.initGrabber(1280,720);
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
