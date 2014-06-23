#ifndef WARPEDITOR_H
#define WARPEDITOR_H

#include "warpWindow.h"

class warpEditor
{
    public:
        vector<warpWindow*>  windows;
        ofRectangle displayRect;

        warpEditor();
        void addWindow(warpWindow& ww);
        void loadFile(string filename);
        void saveFile(string filename);
        void setImage(ofxCvImage& image);
        void mousePressed(int x, int y, int button);
        void mouseDragged(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void draw();
    protected:

        ofxCvImage *origImage;
        int handleRadius;
        warpWindow  *selectedWindow;
        ofPoint     *selectedHandle;
        ofPoint handleOffset;
};

#endif // WARPEDITOR_H
