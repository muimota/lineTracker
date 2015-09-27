//
//  ThreadedWindow.cpp
//  linetracker
//
//  Created by Martin Nadal Berlinches on 23/09/15.
//
//

#include "ThreadedWindow.h"

ThreadedWindow::ThreadedWindow( WarpWindow *_ww){
    ww = _ww;
}

void ThreadedWindow::threadedFunction(){
    
    //warp and apply effects
    //ww->warp();
    ww->gamma(gamma);
    ww->brightnessContrast(brightness,contrast);
    if(threshold){
        //normal threshold
        ww->threshold(thresholdParam);
    }
    
    //erode
    for(int j=0;j<erode;j++){
        ww->erode();
    }
    //dilate
    for(int j=0;j<dilate;j++){
        ww->dilate();
    }
    ww->median(median);
       //
    ww->findContours(minBlobArea,maxBlobArea,maxWindowMovement,maxLineMovement);
    
}