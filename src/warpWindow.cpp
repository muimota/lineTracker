#include "warpWindow.h"

WarpWindow::WarpWindow():ofxCvGrayscaleImage(),lineEnergy(0){
    //init averages
    bboxCount=bboxIndex=0;
    for(int i=0;i<nsamples;i++){
        bboxSamples[i].set(0,0,0,0);
    }
    for(int i=0;i<ndivisions;i++){
        whitepixels[i]=0;
    }

    status = LineEventArgs::READY;
    notifyEvent();
};

void WarpWindow::notifyEvent(){
    LineEventArgs lineEventArgs(status,lineBox,lineBox.height/getHeight(),this);
    ofNotifyEvent(lineEvent, lineEventArgs, this);
    statusTimeStamp = ofGetElapsedTimeMillis();
}

void WarpWindow::allocate(int w, int h){
    //
    ofxCvGrayscaleImage::allocate(w,h);
    prevImage.allocate(w,h);
    diffImage.allocate(w,h);
    floatImage.allocate(w,h);

    videoArea = w*h;

    dstPoints[0] = ofPoint(0,0);
    dstPoints[1] = ofPoint(w,0);
    dstPoints[2] = ofPoint(w,h);
    dstPoints[3] = ofPoint(0,h);
}
void WarpWindow::setImage(ofxCvImage &_image){
    origImage = &_image;
    int imageWidth  = origImage->getWidth();
    int imageHeight = origImage->getHeight();

    srcPoints[0] = ofPoint(0,0);
    srcPoints[1] = ofPoint(imageWidth,0);
    srcPoints[2] = ofPoint(imageWidth,imageHeight);
    srcPoints[3] = ofPoint(0,imageHeight);
}

ofxCvImage& WarpWindow::getImage(){
    return *origImage;
}

void WarpWindow::warp(){
    //copy previous image into prevImage
    warpIntoMe(*origImage,srcPoints,dstPoints);
}

//TODO:Should this be in warp?
void WarpWindow::findContours(float minBlobArea,float maxBlobArea){

    //time since las status update
    long long unsigned timeElapsed = ofGetElapsedTimeMillis()-statusTimeStamp;

    //diff image with previus
    diffImage.absDiff(*this,prevImage);
    //update prev image
    prevImage.setFromPixels(getPixels(),getWidth(),getHeight());

    int nonZeroPixels = diffImage.countNonZeroInRegion(0,0,getWidth(),getHeight());
    //amount of movement in whole area
    windowMovement  = (float)nonZeroPixels/videoArea;

    contourFinder.findContours(*this,400,maxBlobArea*videoArea/1000,1,false);
    float prevLineEnergy = lineEnergy;

    //looking for line
    if(status==LineEventArgs::READY && windowMovement<0.1 && contourFinder.blobs.size()>0){
        //grab the biggest blob
        ofxCvBlob blob = contourFinder.blobs[0];
        ofRectangle bbox = blob.boundingRect;

        //check the proportions of the line
        if(bbox.height/bbox.width>2.0){
            lineEnergy = (lineEnergy<10)?lineEnergy+1:10;
            //insert bbox
            bboxCount = MIN(bboxCount+1,nsamples);
            bboxIndex = (bboxIndex+1)%bboxCount;
            bboxSamples[bboxIndex] = bbox;
        }else{
            bboxIndex = bboxCount=lineEnergy = lineMovement= 0;
            return;
        }
    }

    //calcualate avg box
    lineBox.set(0,0,0,0);
    for(int i=0;i<bboxCount;i++){
        lineBox.position += bboxSamples[i].position/bboxCount;
        lineBox.width    += bboxSamples[i].width/bboxCount;
        lineBox.height   += bboxSamples[i].height/bboxCount;
    }

    //we have just found a line
    if(status==LineEventArgs::READY  && lineEnergy==10){
        startLineBox = lastLineBox = lineBox;

        //Detected status
        status=LineEventArgs::DETECTED;
        notifyEvent();
        lineArea = startLineBox.width*startLineBox.height;

        int divisionHeight = startLineBox.height/ndivisions;

        for(int i=0;i<ndivisions;i++){
            startWhitePixels[i]=countNonZeroInRegion(startLineBox.x,startLineBox.y+divisionHeight*i,startLineBox.width,divisionHeight);
        }
    }

    if(status==LineEventArgs::DETECTED){

        //calculate lineMovement
        nonZeroPixels = diffImage.countNonZeroInRegion(startLineBox.x,startLineBox.y,startLineBox.width,startLineBox.height);
        lineMovement = nonZeroPixels/(float)(startLineBox.width*startLineBox.height);

        //calculate distribution
        int divisionHeight = startLineBox.height/ndivisions;
        for(int i=0;i<ndivisions;i++){
            whitepixels[i]=countNonZeroInRegion(startLineBox.x,startLineBox.y+divisionHeight*i,startLineBox.width,divisionHeight);
            int perct = (int)(whitepixels[i]/(float)startWhitePixels[i]*100);
            cout<<i<<"#"<<perct<<"("<<whitepixels[i]<<") #";
        }
        cout<<endl;

        if(lineMovement>0.06){
            bboxIndex = bboxCount=lineEnergy = lineMovement= 0;
            status=LineEventArgs::CANCELLED;
            notifyEvent();
            status=LineEventArgs::READY;
            notifyEvent();
        }

    }

}

void WarpWindow::gamma(float gammaParameter){
    floatImage = *this;
    IplImage *img = floatImage.getCvImage();
    cvPow(img,img,gammaParameter);
    setFromFloatImage(floatImage);
}
void WarpWindow::median(int radius){
    //TODO:gamma & median doing the same float conversion
    floatImage = *this;
    IplImage *img = floatImage.getCvImage();
    radius = MIN(5,MAX(0,1+(radius-1)*2));
    cvSmooth( img, img, CV_MEDIAN, radius, 0 );
    setFromFloatImage(floatImage);
}

void WarpWindow::draw(const ofRectangle& rect){
    float scaleX = rect.width/getWidth();
    float scaleY = rect.height/getHeight();
    ofRectangle rectangle = ofRectangle(rect.x+lineBox.x*scaleX,rect.y+lineBox.y*scaleY,lineBox.width*scaleX,lineBox.height*scaleY);
    ofxCvGrayscaleImage::draw(rect);
    ofPushStyle();
    ofNoFill();
    ofSetColor(255,0,0,(int)ofMap(lineEnergy,0,10,0,255));
    ofRect(rect.x+lineBox.x*scaleX,rect.y+lineBox.y*scaleY,lineBox.width*scaleX,lineBox.height*scaleY);
    ofPopStyle();

}


