#include "WarpWindow.h"

WarpWindow::WarpWindow(){
    windowMovement = 0; //Window's amount of movement
    lineMovement   = 0; //line's amount of movement
    reset();
}

void WarpWindow::reset(){
    
    lineEnergy=0;
    bboxCount=bboxIndex=0;
    //init averages
    for(int i=0;i<nsamples;i++){
        bboxSamples[i].set(0,0,0,0);
    }
    lineBox.set(0,0,0,0);
    startLineBox.set(0,0,0,0);
    status = LineEventArgs::READY;
    notifyEvent();
}

void WarpWindow::notifyEvent(){
    LineEventArgs lineEventArgs(status,lineBox,0,this);
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
void WarpWindow::findContours(float minBlobArea,float maxBlobArea,float maxWindowMovement,float maxLineMovement){

    //time since las status update
    long long unsigned timeElapsed = ofGetElapsedTimeMillis()-statusTimeStamp;

    //if in 20sec nohing has happened reset the line
    if((status != LineEventArgs::READY && timeElapsed>20000) ||
       (status >  LineEventArgs::START && timeElapsed>5000)) {
        reset();
        return;
    }
    //if detected update detected line
    if(status == LineEventArgs::DETECTED && timeElapsed>1000){
        status = LineEventArgs::READY;
    }

    //diff image with previus
    diffImage.absDiff(*this,prevImage);
    //update prev image
    prevImage.setFromPixels(getPixels(),getWidth(),getHeight());

    int nonZeroPixels = diffImage.countNonZeroInRegion(0,0,getWidth(),getHeight());
    //amount of movement in whole area
    windowMovement  = (float)nonZeroPixels/videoArea;

    contourFinder.findContours(*this,minBlobArea*videoArea/1000,maxBlobArea*videoArea/1000,1,false);



    //looking for line
    if(status==LineEventArgs::READY && windowMovement<maxWindowMovement && contourFinder.blobs.size()>0){
        //grab the biggest blob
        ofxCvBlob blob = contourFinder.blobs[0];
        ofRectangle bbox = blob.boundingRect;

        //check the proportions of the line
        if(bbox.height/bbox.width>2.0){
            if(bboxCount==0 || (lineBox.getCenter().distance(bbox.getCenter())<5)){
                lineEnergy = (lineEnergy<10)?lineEnergy+1:10;
                //insert bbox
                bboxCount = MIN(bboxCount+1,nsamples);
                bboxIndex = (bboxIndex+1)%bboxCount;
                bboxSamples[bboxIndex] = bbox;
                //add this box to the lineBox Average
                //calcualate avg box with this new value
                lineBox.set(0,0,0,0);
                for(int i=0;i<bboxCount;i++){
                    lineBox.position += bboxSamples[i].position/bboxCount;
                    lineBox.width    += bboxSamples[i].width/bboxCount;
                    lineBox.height   += bboxSamples[i].height/bboxCount;
                }

            }else{
                bboxIndex = bboxCount=lineEnergy = lineMovement= 0;
                return;
            }
        }
    }

    //we have just found a line
    if(status==LineEventArgs::READY  && lineEnergy==10){
        startLineBox = lineBox;

        //Detected status
        status=LineEventArgs::DETECTED;
        notifyEvent();

        int divisionHeight = startLineBox.height/ndivisions;
        //calculate distribution of white pixels along the line
        for(int i=0;i<ndivisions;i++){
            startWhitePixels[i]=countNonZeroInRegion(startLineBox.x,startLineBox.y+divisionHeight*i,startLineBox.width,divisionHeight);
        }
        //init indexes
        topIndex = 0;
        bottomIndex  = ndivisions-1;
    }

    if(status!=LineEventArgs::READY){

        //calculate lineMovement
        nonZeroPixels = diffImage.countNonZeroInRegion(startLineBox.x,startLineBox.y,startLineBox.width,startLineBox.height);
        lineMovement = nonZeroPixels/(float)(startLineBox.width*startLineBox.height);

        if(lineMovement>maxLineMovement){
            status=LineEventArgs::CANCELLED;
            notifyEvent();
            reset();
            return;
        }

        //calculate distribution
        int divisionHeight = startLineBox.height/ndivisions;
        //count white pixels for distribution
        for(int i=0;i<ndivisions;i++){
            whitepixels[i]=countNonZeroInRegion(startLineBox.x,startLineBox.y+divisionHeight*i,startLineBox.width,divisionHeight);
        }

        float topPortion    = whitepixels[topIndex]/(float)startWhitePixels[0];
        float bottomPortion = whitepixels[bottomIndex]/(float)startWhitePixels[0];

        //TODO: simplify it, if a gesture has started with a direction. don't change it
        bool  downMove = topPortion<0.1;
        bool  upMove   = bottomPortion<0.1;

        if(downMove || upMove ){

            //Alwaysthe line is going to be smaller
            lineBox.height-=divisionHeight;
            bboxIndex = bboxCount= 0;

            if(status==LineEventArgs::DETECTED){
                status=LineEventArgs::START;
                notifyEvent();
            }

            if(downMove && (status==LineEventArgs::START || status==LineEventArgs::DOWN)){
                lineBox.y+=divisionHeight;
                status=LineEventArgs::DOWN;
                topIndex++;
                notifyEvent();
            }

            if(upMove && (status==LineEventArgs::START || status==LineEventArgs::UP)){
                bottomIndex--;
                status=LineEventArgs::UP;
                notifyEvent();
            }
            if(bottomIndex==topIndex){
                lineBox.height=0; //we consider the line finished
                status=LineEventArgs::FINISH;
                notifyEvent();
                reset();
            }
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
    ofSetColor(255,0,0,(int)ofMap(lineEnergy,0,10,50,255));
    ofRect(rect.x+lineBox.x*scaleX,rect.y+lineBox.y*scaleY,lineBox.width*scaleX,lineBox.height*scaleY);
    ofPopStyle();

}


