#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){


    videoWidth  = 640;
    videoHeight = 480;

    //videoSource = &player;
    
    eventMessage[0] = "init";
    eventMessage[1] = "init";
    eventMessage[2] = "init";
    eventMessage[3] = "init";
    
    
    
    //videos
    deviceIndex = 0;
    devices = vidGrabber.listDevices();
    vidGrabber.setDeviceID(devices[deviceIndex].id);
	vidGrabber.setDesiredFrameRate(30);
	vidGrabber.initGrabber(videoWidth,videoHeight);
	videoSource = &vidGrabber;

    videoImage.allocate(videoWidth,videoHeight);
    videoGrayImage.allocate(videoWidth,videoHeight);

    //windows
    we.setImage(videoGrayImage);
    we.loadFile("test.xml");
    
    //set where the rectangle is going to be drawn
    int windowEditorWidth = 600;
    //where is going to be displayed so it can be edited with the mouse
    we.displayRect.set(200,10,windowEditorWidth,windowEditorWidth*videoHeight/float(videoWidth));
    
    //windows' events listener
    for(int i=0;i<4;i++){
        WarpWindow *ww = we.windows[i];
        ofAddListener(ww->lineEvent, this, &ofApp::lineHandler);
        tw[i] = new ThreadedWindow(ww);
    }

    //GUI
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
    maxWindowMovement.set("Max Window Movement",0.1,0,1);
    maxLineMovement.set("Max Line Movement",0.06,0,1);


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
	parameters.add(maxWindowMovement);
	parameters.add(maxLineMovement);


    displayDebug = true;
    font.loadFont("type/verdana.ttf", 50);

	gui.setup(parameters);
    gui.loadFromFile("settings.xml");
    gui.setPosition(420,360);

    //osc up
    sender.setup("127.0.0.1",12000);
}

//--------------------------------------------------------------
void ofApp::update(){
        videoSource->update();
        if(videoSource->isFrameNew()){

            
            videoImage.setFromPixels(videoSource->getPixels(), videoSource->getWidth(),videoSource->getHeight());
            videoImage.mirror(true,false);
            videoGrayImage = videoImage;

            //image process
            //this should be threaded
            for(int i=0;i<4;i++){
                
                tw[i]->gamma = gamma[i];
                tw[i]->brightness = brightness[i];
                tw[i]->contrast = contrast[i];
                tw[i]->threshold = threshold[i];
                tw[i]->thresholdParam = thresholdParam[i];
                tw[i]->erode = erode[i];
                tw[i]->dilate = dilate[i];
                tw[i]->median = median[i];
                tw[i]->minBlobArea = minBlobArea;
                tw[i]->maxBlobArea = maxBlobArea;
                tw[i]->maxWindowMovement = maxWindowMovement;
                tw[i]->maxLineMovement = maxLineMovement;
                
                tw[i]->startThread();
            }
           
            for(int i=0;i<4;i++){
                
                tw[i]->waitForThread();
        
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

    for(int i=0;i<4;i++){
        ofPushStyle();
        if(i%2==0){
            ofSetColor(0,255,0);
        }else{
            ofSetColor(255,0,0);
        }
        font.drawString(eventMessage[i],100,100+i*(font.getSize()+5));
        ofPopStyle();
    }
}

void ofApp::drawDebug(){
    videoSource->draw(0,0,133,100);
    //windows editor , position define by displayRect
    we.draw();

    //draw windows
    ofPoint windowOffset(0,400);
    float windowHeight = 200;
    for(int i=0;i<we.windows.size();i++){
       WarpWindow& ww = *we.windows[i];
       float windowWidth = ww.getWidth()*windowHeight/float(ww.getHeight());
       ofRectangle displayRect(windowOffset.x,windowOffset.y,windowWidth,windowHeight);
       ww.draw(displayRect);
       if(ww.status>LineEventArgs::READY ){
            ww.setROI(ww.startLineBox);
            ww.drawROI(windowOffset.x,windowOffset.y+200);
            ww.setROI(0,0,ww.width,ww.height);
            //ww.resetROI();
       }

       windowOffset.x+=windowWidth;
    }
    //display info
    stringstream ss;
    for(int i=0;i<we.windows.size();i++){
        WarpWindow& ww = *we.windows[i];
        ss<<"window: "<<ww.name<<endl;
        ss<<"Window movement:"<<ww.windowMovement<<endl;
        ss<<"Line movement:"<<ww.windowMovement<<endl;
        ss<<"energy:"<<ww.lineEnergy<<endl;
    }
    ofDrawBitmapString(ss.str(),0,200);
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='s'){
        we.saveFile("test.xml");
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

//handles the line events from the warpwindows
void ofApp::lineHandler(LineEventArgs &le){
     int i;
     stringstream ss;
     WarpWindow *ww;

     //index of the

     for(i=0;i<we.windows.size();i++){
        ww =we.windows[i];
        if(ww == le.sender){
            break;
        }
     }

     ofxOscMessage m;
     m.setAddress("/MNS");
     m.addIntArg(le.status);
     m.addIntArg(i);
     //ratio between window height, line height
     float lineHeight = ww->startLineBox.height/ww->height;
     m.addFloatArg(lineHeight);
     //ratio of the line line consumed
     float lineRatio;
     if(le.status==LineEventArgs::DETECTED || le.status==LineEventArgs::READY){
        lineRatio = 0;
     }else{
        lineRatio = ww->lineBox.height/ww->startLineBox.height;
     }
     m.addFloatArg(lineRatio);
     sender.sendMessage(m);

     if(le.status==LineEventArgs::DETECTED){
        int money  = (ww->startLineBox.height/ww->height)*10000000;
        ss<<"DETECTED!"<<money;
        eventMessage[i] = ss.str();
     }

     if(le.status==LineEventArgs::READY){
        ss<<"READY";
        eventMessage[i] = ss.str();
     }
     if(le.status==LineEventArgs::UP){
        int percent = (1-ww->lineBox.height/ww->startLineBox.height)*100;
        cout<<percent<<endl;
        ss<<"BUY:"<<percent<<"%";
        eventMessage[i] = ss.str();
     }
     if(le.status==LineEventArgs::DOWN){
        //int money   = (ww->startLineBox.height/ww->height)*10000000;
        //int buy     = (1-ww->lineBox.height/ww->startLineBox.height)*money;
        int percent = (1-ww->lineBox.height/ww->startLineBox.height)*100;
        ss<<"SELL:"<<percent;
        eventMessage[i] = ss.str();
     }
     if(le.status==LineEventArgs::FINISH){
        ss<<"FINISH!!";
        eventMessage[i] = ss.str();
     }

    cout<<le<<endl;
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
    //sends the event to windowsEditor
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
