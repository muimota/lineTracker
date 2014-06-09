#ifndef WARPEDITOR_H
#define WARPEDITOR_H

#include "warpWindow.h"

class warpEditor
{
    public:
        vector<warpWindow*>  windows;
        ofRectangle displayRect;

        void addWindow(warpWindow& ww);
        void mousePressed(int x, int y, int button);
        void mouseDragged(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void draw();
    protected:
        const int handleRadius  = 10;
        warpWindow  *selectedWindow;
        ofPoint     *selectedHandle = NULL;
        ofPoint handleOffset;
};

#endif // WARPEDITOR_H
