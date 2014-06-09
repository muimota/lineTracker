#include "warpWindow.h"

void warpWindow::allocate(int w, int h){
    //
    ofxCvColorImage::allocate(w,h);
    dstPoints[0] = ofPoint(0,0);
    dstPoints[1] = ofPoint(w,0);
    dstPoints[2] = ofPoint(w,h);
    dstPoints[3] = ofPoint(0,h);
}
void warpWindow::setImage(ofxCvImage &_image){
    origImage = &_image;
    int imageWidth  = origImage->getWidth();
    int imageHeight = origImage->getHeight();

    srcPoints[0] = ofPoint(0,0);
    srcPoints[1] = ofPoint(imageWidth,0);
    srcPoints[2] = ofPoint(imageWidth,imageHeight);
    srcPoints[3] = ofPoint(0,imageHeight);
}

ofxCvImage& warpWindow::getImage(){
    return *origImage;
}

void warpWindow::warp(){
    warpIntoMe(*origImage,srcPoints,dstPoints);
}
