#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "warpWindow.h"
#include "warpeditor.h"

class ofApp : public ofBaseApp{
    private:
            ofVideoPlayer   player;
            ofxCvColorImage videoImage;
            ofxCvGrayscaleImage videoGrayImage;

            ofPoint         srcPoints[4];
            ofPoint         dstPoints[4];
            warpEditor      we;
            ofxCvGrayscaleImage bgImage;


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
