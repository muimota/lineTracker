#ifndef WARPWINDOW_H
#define WARPWINDOW_H
#include "ofxOpenCv.h"

class warpWindow : public ofxCvGrayscaleImage{

    protected:
        ofxCvImage *origImage;
    public:
        ofPoint   srcPoints[4];
        ofPoint   dstPoints[4];

        void allocate(int w, int h);
        void setImage(ofxCvImage &_image);
        ofxCvImage& getImage();
        void warp();
};

#endif // WARPWINDOW_H
