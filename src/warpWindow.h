#ifndef WARPWINDOW_H
#define WARPWINDOW_H
#include "ofxOpenCv.h"

#define nsamples   20
#define ndivisions 20

class LineEventArgs;

class WarpWindow : public ofxCvGrayscaleImage{

    protected:

        ofxCvImage          *origImage;//pointer to the frame image
        ofxCvGrayscaleImage prevImage; //previous image to do image substract
        ofxCvGrayscaleImage diffImage; //diff image to calculate amount of movement

        ofxCvFloatImage     floatImage;//float image for gamma

        ofRectangle bboxSamples[nsamples];//last bounding boxes
        int bboxIndex,bboxCount;

        int startWhitePixels[ndivisions]; //initial distribution of pixels
        int whitepixels[ndivisions];      //distribution of pixels
        int topIndex; //index to know whick was the last empty portion of line
        int bottomIndex;


        void notifyEvent(); //notify new status

     public:
        WarpWindow();

        string    name;
        int status;
        unsigned long long  statusTimeStamp;//timestamp un ms of last status change
        ofPoint   srcPoints[4];
        ofPoint   dstPoints[4];


        ofRectangle lineBox,startLineBox;
        int lineEnergy;
        float windowMovement = 0; //Window's amount of movement
        float lineMovement   = 0; //line's amount of movement

        ofxCvContourFinder 	contourFinder;

        int videoArea;
        void allocate(int w, int h);
        void setImage(ofxCvImage &_image);
        ofxCvImage& getImage();
        void warp();
        void median(int radius);
        void gamma(float gammaParameter);
        void findContours(float minBlobArea,float maxBlobArea,float maxWindowMovement,float maxLineMovement);
        void draw(const ofRectangle& rectangle);
        void reset();
        ofEvent<LineEventArgs> lineEvent;
};


class LineEventArgs : public ofEventArgs {
public:
    static const int READY      =0;
    static const int DETECTED   =1;
    static const int START      =2;
    static const int UP         =3;
    static const int DOWN       =4;
    static const int CANCELLED  =5;
    static const int FINISH     =6;

    WarpWindow *sender;
    int status; //detected, cancelled, update
	ofRectangle bbox;
	float lineSize;

	LineEventArgs(int _status,ofRectangle _bbox,float _lineSize,WarpWindow *_sender):
	    status(_status),bbox(_bbox),lineSize(_lineSize),sender(_sender){};

    friend ostream& operator<<(ostream& os, const LineEventArgs& le){
        string statusNames[] = {"READY","DETECTED","START","UP","DOWN","CANCELLED","FINISH"};

        os << le.sender->name << ":" << statusNames[le.status] <<": "<<le.bbox.height;
        return os;
    }
};
#endif // WARPWINDOW_H
