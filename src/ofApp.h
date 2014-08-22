#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "warpWindow.h"
#include "warpeditor.h"

#define BLOB_SCALE 1000.0
#define AVG_SIZE  10
#define TRACKS_COUNT 4
#define CAPITAL 10000000

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
            ofxCvFloatImage videoFloatImage;

            ofPoint         srcPoints[4];
            ofPoint         dstPoints[4];
            WarpEditor      we;
            vector<ofxCvGrayscaleImage> bgImages,diffImages;
            vector<ofxCvContourFinder*> 	contourFinders;
            //TODO unos comentarios...

            //aray to calculate an average of the height
            float   lineHeightsArray[TRACKS_COUNT][AVG_SIZE];
            //average Line
            float   avgLineHeight[TRACKS_COUNT];
            //line of the operation accepted
            float   operationHeight[TRACKS_COUNT];
            //if button is pressed in the track
            bool    buttonPressed[TRACKS_COUNT];
            //state of each track
            int status[TRACKS_COUNT];

            int avgIndex;

            ofTrueTypeFont font;

            // parametoers
            ofxPanel gui;
            ofParameterGroup parameters;
            ofParameterGroup windowParameters[4];


            //Images parameters
            ofParameter<float>  contrast[4];
            ofParameter<float>  brightness[4];
            ofParameter<float>  gamma[4];
            ofParameter<bool>   threshold[4];
            ofParameter<int>    thresholdParam[4];
            ofParameter<bool>   adaptiveThreshold[4];
            ofParameter<int>    adaptiveBlockSize[4];
            ofParameter<int>    adaptiveOffset[4];
            ofParameter<bool>   adaptiveInvert[4];
            ofParameter<bool>   adaptiveGauss[4];
            ofParameter<int>    dilate[4];
            ofParameter<int>    erode[4];
            ofParameter<int>    median[4];

            ofParameter<int>    minBlobArea;
            ofParameter<int>    maxBlobArea;
            ofParameter<bool>   useBackgroundSubstraction;

            ofxButton grabBackground;

            bool displayDebug;
	public:
		void setup();
		void update();
		void draw();
		void drawDebug();
		void drawPresentation();
         void grabBackGround();

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
