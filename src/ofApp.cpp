#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){



/*
    player.loadMovie("test04.mp4");
    player.setLoopState(OF_LOOP_NORMAL);
    player.play();
*/

    videoWidth  = 640;
    videoHeight = 480;

    //videoSource = &player;

    //if using camera v4l2-ctl -d 1 -c exposure_auto=1 to disable autoexposure
    devices = vidGrabber.listDevices();
    vidGrabber.setDeviceID(devices[deviceIndex].id);
	vidGrabber.setDesiredFrameRate(30);
	vidGrabber.initGrabber(videoWidth,videoHeight);
	//vidGrabber.videoSettings();
    videoSource = &vidGrabber;

    videoImage.allocate(videoWidth,videoHeight);
    videoGrayImage.allocate(videoWidth,videoHeight);

    videoFloatImage.allocate(videoWidth,videoHeight);

    we.setImage(videoGrayImage);
    we.loadFile("test.xml");
    //set where the rectangle is going to be drawn
    int windowEditorWidth = 600;
    //where is going to be displayed so it can be edited with the mouse
    we.displayRect.set(200,10,windowEditorWidth,windowEditorWidth*videoHeight/float(videoWidth));

    //init contourFinders
     for(int i=0;i<we.windows.size();i++){
        ofxCvContourFinder  *contour = new ofxCvContourFinder();
        contourFinders.push_back(contour);
        status[i]=0;
    }

    avgIndex=0;



    for(int i=0;i<we.windows.size();i++){
            contrast[i].set("contrast",0,0,1);
            brightness[i].set("brightness",0,-2,2);
            gamma[i].set("gamma",1,0,5);
            threshold[i].set("Threshold",false);
            thresholdParam[i].set("Threshold Level",10,0,255);
            adaptiveThreshold[i].set("Adaptive Threshold",false);
            adaptiveBlockSize[i].set("Block Size",0,0,100);
            adaptiveOffset[i].set("Offset",0,0,100);
            adaptiveInvert[i].set("Invert",false);
            adaptiveGauss[i].set("Gauss",false);
            erode[i].set("Erode",0,0,10);
            dilate[i].set("Dilate",0,0,10);
            median[i].set("Median",0,0,2);
    }

    //min blob max of 10%
	minBlobArea.set("Minimum Blob Area",30,0,(int)BLOB_SCALE/10);
	maxBlobArea.set("Maximum Blob Area",90,0,(int)BLOB_SCALE);

    parameters.setName("Parameters");


    for(int i=0;i<we.windows.size();i++){
        ostringstream imageName;
        imageName<<"window "<<i;
        windowParameters[i].setName(imageName.str());

        windowParameters[i].add(contrast[i]);
        windowParameters[i].add(brightness[i]);
        windowParameters[i].add(gamma[i]);
        windowParameters[i].add(threshold[i]);
        windowParameters[i].add(thresholdParam[i]);
        windowParameters[i].add(adaptiveThreshold[i]);
        windowParameters[i].add(adaptiveBlockSize[i]);
        windowParameters[i].add(adaptiveOffset[i]);
        windowParameters[i].add(adaptiveInvert[i]);
        windowParameters[i].add(adaptiveGauss[i]);
        windowParameters[i].add(erode[i]);
        windowParameters[i].add(dilate[i]);
        windowParameters[i].add(median[i]);

        parameters.add(windowParameters[i]);
    }

	parameters.add(minBlobArea);
	parameters.add(maxBlobArea);
    parameters.add(useBackgroundSubstraction);

    displayDebug = true;
    font.loadFont("type/verdana.ttf", 60);

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
            videoImage.mirror(true,false);
            videoGrayImage = videoImage;

            //image process
            for(int i=0;i<we.windows.size();i++){
                WarpWindow *ww = we.windows[i];

                ww->warp();
                ww->gamma(gamma[i]);
                ww->brightnessContrast(brightness[i],contrast[i]);
                int videoArea = ww->getWidth() * ww->getHeight();
                if(threshold[i]){
                    //normal threshold
                    ww->threshold(thresholdParam[i]);
                }
                if(adaptiveThreshold[i]){
                    //adaptive threshold
                    ww->adaptiveThreshold(adaptiveBlockSize[i],adaptiveOffset[i],adaptiveInvert[i],adaptiveGauss[i]);
                }

                //erode
                for(int j=0;j<erode[i];j++){
                    ww->erode();
                }
                //dilate
                for(int j=0;j<dilate[i];j++){
                    ww->dilate();
                }
                ww->median(median[i]);
                ww->findContours(minBlobArea,maxBlobArea);
            }
        }
}

//--------------------------------------------------------------

void ofApp::draw(){
    if(displayDebug){
        drawDebug();
    }else{
        drawPresentation();
    }
}

void ofApp::drawPresentation(){

    stringstream ss;
    ss<<int(avgLineHeight[0]*CAPITAL)<<"$";
    ofPushStyle();
    if(status[0]==0){
        ofSetColor(0,255,0);
    }else{
        ofSetColor(255,0,0);
    }
    font.drawString(ss.str(),100,100);
    ofPopStyle();
}

void ofApp::drawDebug(){
    videoSource->draw(0,0,133,100);
    //windows editor , position define by displayRect
    we.draw();

    //draw original images
    ofPoint offset(100,400);
    float windowHeight = 200;
    for(int i=0;i<we.windows.size();i++){
       WarpWindow& ww = *we.windows[i];
       float windowWidth = ww.getWidth()*windowHeight/float(ww.getHeight());
       ofRectangle displayRect(offset.x,offset.y,windowWidth,windowHeight);
       ww.draw(displayRect);

       offset.x+=windowWidth;
       //ww.contourFinder.draw(displayRect);
    }
    //display info
    stringstream ss;
    for(int i=0;i<we.windows.size();i++){
        WarpWindow& ww = *we.windows[i];
        ss<<"window "<<i<<endl;
        ss<<"movement:"<<ww.getAmountOfMovement()<<endl;
        ss<<"energy:"<<ww.lineEnergy<<endl;
    }
    ofDrawBitmapString(ss.str(),0,200);
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
        ss<<"line["<<i<<"] "<<avgLineHeight[i]<<endl;
        ofDrawBitmapString(ss.str(),offset.x,offset.y);
       // contourFinders[i]->draw(displayRect);
        offset.x+=windowWidth;
    }

    gui.draw();

}
//
void ofApp::grabBackGround(){
    bgImages.clear();
    diffImages.clear();
    for(int i=0;i<we.windows.size();i++){
       WarpWindow& ww = *we.windows[i];
       //putting copies in the vectors
       bgImages.push_back(ww);
       diffImages.push_back(ww);
    }
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='s'){
        we.saveFile("test.xml");
    }

    if(key=='b'){
        grabBackGround();
    }
    //change video Input
    if(key=='c'){
        vidGrabber.close();
        deviceIndex = (deviceIndex+1)%devices.size();
        vidGrabber.setDeviceID(devices[deviceIndex].id);

        vidGrabber.setDesiredFrameRate(30);
        vidGrabber.initGrabber(videoWidth,videoHeight);
    }
    if(key=='d'){
        displayDebug = !displayDebug;
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
