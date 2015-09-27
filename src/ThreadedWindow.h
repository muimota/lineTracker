//
//  ThreadedWindow.hpp
//  linetracker
//
//  Created by Martin Nadal Berlinches on 23/09/15.
//
//

#ifndef ThreadedWindow_hpp
#define ThreadedWindow_hpp

#include "ofMain.h"
#include "WarpWindow.h"

class ThreadedWindow:public ofThread{
    
public:
    
    bool    threshold;
    float   gamma, contrast, brightness;
    int     thresholdParam, dilate, erode, median;
    
    float   maxWindowMovement, maxLineMovement;
    int     minBlobArea, maxBlobArea;
    
    ThreadedWindow(WarpWindow *ww);
    void threadedFunction();
    
private:
    WarpWindow *ww;
};

#endif /* ThreadedWindow_hpp */
