#include "warpWindow.h"

WarpWindow::WarpWindow():ofxCvGrayscaleImage(),lineEnergy(0){
    //init averages
    for(int i=0;i<nsamples;i++){
        energySamples[i]=0;
        movementSamples[i]=0;
        bboxSamples[i].position = ofPoint(0,0);
        bboxSamples[i].width = bboxSamples[i].height = 0;
    }
    sampleIndex=0;
};

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
    //update sample index
    //TODO:may idependent indexes
    sampleIndex = (sampleIndex+1) % nsamples;
    //calculate amount of movement
    diffImage.absDiff(*this,prevImage);
    int nonZeroPixels = countNonZeroInRegion(0,0,getWidth(),getHeight());
    movementSamples[sampleIndex]=nonZeroPixels/(float)videoArea;
    prevImage.setFromPixels(getPixels(),getWidth(),getHeight());

    //
    contourFinder.findContours(*this,videoArea*minBlobArea/1000, videoArea*maxBlobArea/1000,1,false);
    float prevLineEnergy = lineEnergy;
    //if found update energy

    if(movementSamples[sampleIndex]<0.1 && contourFinder.blobs.size()>0){
        ofxCvBlob blob = contourFinder.blobs[0];
        ofRectangle bbox = blob.boundingRect;
        bboxSamples[sampleIndex] = bbox;
        //check the proportions of the line
        if(bbox.height/bbox.width>2.0){
            lineEnergy = (lineEnergy<10)?lineEnergy+1:10;
        }else{
            lineEnergy = (lineEnergy>0)?lineEnergy-1:0;
        }
    }else{
        lineEnergy = (lineEnergy>0)?lineEnergy-1:0;
    }

    //calcualate avg box
    lineBox.set(0,0,0,0);
    for(int i=0;i<nsamples;i++){
        lineBox.position += bboxSamples[i].position/nsamples;
        lineBox.width    += bboxSamples[i].width/nsamples;
        lineBox.height   += bboxSamples[i].height/nsamples;
    }

    if(prevLineEnergy<10 && lineEnergy==10){
        startLineBox = lastLineBox = lineBox;
        cout<<"new line detected!"<<endl;
    }else{
        if(lineEnergy<10 && prevLineEnergy==10){
            cout<<"line lost"<<endl;
        }
    }

    //line goes shorter
    if(lineEnergy==10 && prevLineEnergy==10){
        int percentage = lineBox.height/startLineBox.height*100;
        int partialPercentage = lastLineBox.height/startLineBox.height*100;
        //variation of 5%
        if(partialPercentage-percentage>5){
            int percentage = lineBox.height/startLineBox.height*100;
            //the variation is very small consider it hasn't changed
            cout<< lineBox.x <<" - "<<lastLineBox.x<<endl;
            if(fabs(lineBox.x-lastLineBox.x)>5){
                cout<<"TOP "<<percentage<<endl;
            }else{
                cout<<"BOTTOM "<<percentage<<endl;
            }


            lastLineBox = lineBox;
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

float WarpWindow::getAmountOfMovement(){
    float movementSum = 0;
    for(int i=0;i<nsamples;i++){
        movementSum+=movementSamples[i];
    }
    return movementSum/nsamples;
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


