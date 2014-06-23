#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){



/*
    player.loadMovie("test04.mp4");
    player.setLoopState(OF_LOOP_NORMAL);
    player.play();
*/

    videoWidth  = 1280;
    videoHeight = 800;

    //videoSource = &player;

    //if using camera v4l2-ctl -d 1 -c exposure_auto=1 to disable autoexposure
    devices = vidGrabber.listDevices();
    vidGrabber.setDeviceID(devices[deviceIndex].id);
	vidGrabber.setDesiredFrameRate(30);
	vidGrabber.initGrabber(videoWidth,videoHeight);
    videoSource = &vidGrabber;

    videoImage.allocate(videoWidth,videoHeight);
    videoGrayImage.allocate(videoWidth,videoHeight);

    we.setImage(videoGrayImage);
    we.loadFile("test.xml");
    //set where the rectangle is going to be drawn
    int windowEditorWidth = 600;
    we.displayRect.set(200,10,windowEditorWidth,windowEditorWidth*videoHeight/float(videoWidth));

    //init contourFinders
    for(int i=0;i<we.windows.size();i++){
        ofxCvContourFinder  *contour = new ofxCvContourFinder();
        contourFinders.push_back(contour);
        amountsOfMovement.push_back(0);
    }

	parameters.setName("Parameters");
	useBackground.set("Background Substraction",false);
	threshold.set("Threshold",10,0,255);

    adaptiveThreshold.set("Adaptive Threshold",false);
    adaptiveBlockSize.set("Blowck Size",0,0,100);
    adaptiveOffset.set("Offset",0,0,100);
    adaptiveInvert.set("Invert",false);
    adaptiveGauss.set("Gauss",false);

	erode.set("Erode",0,0,10);
    dilate.set("Dilate",0,0,10);

	minBlobArea.set("Minimum Blob Area",30,0,(int)BLOB_SCALE);
	maxBlobArea.set("Maximum Blob Area",90,0,(int)BLOB_SCALE);

    parameters.add(threshold);
    parameters.add(adaptiveThreshold);
    parameters.add(adaptiveBlockSize);
    parameters.add(adaptiveOffset);
    parameters.add(adaptiveInvert);
    parameters.add(adaptiveGauss);

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
                if(adaptiveThreshold){
                    //adaptive threshold
                    diffImages[i].adaptiveThreshold(adaptiveBlockSize,adaptiveOffset,adaptiveInvert,adaptiveGauss);
                }else{
                    //normal threshold
                    diffImages[i].threshold(threshold);
                }
                //erode
                for(int j=0;j<erode;j++){
                    diffImages[i].erode();
                }
                //dilate
                for(int j=0;j<dilate;j++){
                    diffImages[i].dilate();
                }
                //calculate amount of Movement
                prevDiffImages[i].absDiff(diffImages[i]);
                amountsOfMovement[i] = prevDiffImages[i].countNonZeroInRegion(0,0,prevDiffImages[i].getWidth(),prevDiffImages[i].getHeight());
                prevDiffImages[i] = diffImages[i];

                contourFinders[i]->findContours(diffImages[i],videoArea*minBlobArea/BLOB_SCALE, videoArea*maxBlobArea/BLOB_SCALE,10,false);
            }
        }
}


//--------------------------------------------------------------
void ofApp::draw(){
    videoSource->draw(0,0,133,100);
    //windows editor , position define by displayRect
    we.draw();

    //draw original images
    ofPoint offset(100,400);
    float windowHeight = 200;
    for(int i=0;i<we.windows.size();i++){
       warpWindow& ww = *we.windows[i];
       float windowWidth = ww.getWidth()*windowHeight/float(ww.getHeight());
       ofRectangle displayRect(offset.x,offset.y,windowWidth,windowHeight);
       ww.draw(displayRect);
       offset.x+=windowWidth;
    }

    //draw bg Images
    offset.x+=30;
    for(int i=0;i<bgImages.size();i++){
       ofxCvImage& img = bgImages[i];
       float windowWidth = img.getWidth()*windowHeight/float(img.getHeight());
       ofRectangle displayRect(offset.x,offset.y,windowWidth,windowHeight);
       img.draw(displayRect);
       offset.x+=windowWidth;
    }

    //drawing diffImage
    offset.x+=30;
    for(int i=0;i<diffImages.size();i++){
        ofxCvImage& img = diffImages[i];
        float windowWidth = img.getWidth()*windowHeight/float(img.getHeight());
        ofRectangle displayRect(offset.x,offset.y,windowWidth,windowHeight);
        img.draw(displayRect);
        stringstream ss;
        ss<<amountsOfMovement[i];
        ofDrawBitmapString(ss.str(),offset.x,offset.y);
        contourFinders[i]->draw(displayRect);
        offset.x+=windowWidth;
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
        prevDiffImages.clear();
        for(int i=0;i<we.windows.size();i++){
           warpWindow& ww = *we.windows[i];
           //putting copies in the vectors
           bgImages.push_back(ww);
           diffImages.push_back(ww);
           prevDiffImages.push_back(ww);
        }
    }

    if(key=='c'){
        vidGrabber.close();
        deviceIndex = (deviceIndex+1)%devices.size();
        vidGrabber.setDeviceID(devices[deviceIndex].id);

        vidGrabber.setDesiredFrameRate(30);
        vidGrabber.initGrabber(videoWidth,videoHeight);
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
