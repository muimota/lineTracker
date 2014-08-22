#ifndef WARPWINDOW_H
#define WARPWINDOW_H
#include "ofxOpenCv.h"

#define nsamples 20
class WarpWindow : public ofxCvGrayscaleImage{

    protected:
        ofxCvImage          *origImage;//pointer to the frame image
        ofxCvGrayscaleImage prevImage; //previous image to do image substract
        ofxCvGrayscaleImage diffImage; //diff iamge to calculate amount of movement
        ofxCvFloatImage     floatImage;//float image for gamma

        float       movementSamples[nsamples];  //last amounts of movement
        float       energySamples[nsamples];    //last energy
        ofRectangle bboxSamples[nsamples];//last bounding boxes
        int         sampleIndex;

    public:
        WarpWindow();

        ofPoint   srcPoints[4];
        ofPoint   dstPoints[4];

        ofRectangle lineBox,startLineBox,lastLineBox; //a rect describing what it looks like a line
        int lineEnergy;
        float amountOfMovement = 0;
        ofxCvContourFinder 	contourFinder;
        int videoArea;
        void allocate(int w, int h);
        void setImage(ofxCvImage &_image);
        ofxCvImage& getImage();
        void warp();
        void median(int radius);
        void gamma(float gammaParameter);
        void updateMovement();
        void findContours(float minBlobArea,float maxBlobArea);
        float getAmountOfMovement();
        void draw(const ofRectangle& rectangle);
};

#endif // WARPWINDOW_H
