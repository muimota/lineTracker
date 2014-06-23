#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "warpWindow.h"
#include "warpeditor.h"

#define BLOB_SCALE 1000.0

class ofApp : public ofBaseApp{
    private:

            ofBaseVideoDraws  *videoSource;

            ofVideoPlayer  player;
            ofVideoGrabber vidGrabber;
            int videoWidth;
            int videoHeight;
            vector<ofVideoDevice> devices;
            int deviceIndex;


            ofxCvColorImage videoImage;
            ofxCvGrayscaleImage videoGrayImage;

            ofPoint         srcPoints[4];
            ofPoint         dstPoints[4];
            warpEditor      we;
            vector<ofxCvGrayscaleImage> bgImages,diffImages,prevDiffImages;
            vector<ofxCvContourFinder*> 	contourFinders;
            vector<float>               amountsOfMovement;

            // parameters
            ofxPanel gui;
            ofParameterGroup parameters;

            ofParameter<int>    threshold;
            ofParameter<bool>   adaptiveThreshold;
            ofParameter<int>    adaptiveBlockSize;
            ofParameter<int>    adaptiveOffset;
            ofParameter<bool>   adaptiveInvert;
            ofParameter<bool>   adaptiveGauss;

            ofParameter<int>    dilate;
            ofParameter<int>    erode;

            ofParameter<int>    minBlobArea;
            ofParameter<int>    maxBlobArea;
            ofParameter<bool>   useBackground;

            ofxButton grabBackground;


	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);



};
