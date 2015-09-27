#ifndef WARPEDITOR_H
#define WARPEDITOR_H

#include "WarpWindow.h"

class WarpEditor
{
    public:
        vector<WarpWindow*>  windows;
        ofRectangle displayRect;


        WarpEditor();
        void addWindow(WarpWindow& ww);
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
        WarpWindow  *selectedWindow;
        ofPoint     *selectedHandle;
        ofPoint     handleOffset;
};

#endif // WARPEDITOR_H
