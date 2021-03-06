#include "warpeditor.h"

WarpEditor::WarpEditor(){
    selectedHandle = NULL;
    handleRadius   = 5;
}

void WarpEditor::mousePressed(int x, int y, int button){
    ofPoint mousePosition(x,y);
    mousePosition-=displayRect.getPosition();
    for (vector<WarpWindow*>::iterator it = windows.begin();it != windows.end();it++){
        for(int i=0;i<4;i++){
            ofPoint pos;
            pos.x=ofMap((*it)->srcPoints[i].x,0,(*it)->getImage().getWidth(),0,displayRect.getWidth());
            pos.y=ofMap((*it)->srcPoints[i].y,0,(*it)->getImage().getHeight(),0,displayRect.getHeight());
            if(pos.distance(mousePosition)<handleRadius){
                //http://stackoverflow.com/a/2160319
                selectedWindow = *it;
                selectedHandle = &(*it)->srcPoints[i];
                handleOffset = pos-mousePosition;
            }
        }
    }
}
void WarpEditor::mouseDragged(int x, int y, int button){
    if(selectedHandle!=NULL){
        ofPoint mousePosition(x,y);
        mousePosition-=displayRect.getPosition();
        mousePosition+=handleOffset;
        selectedHandle->x=floor(ofMap(mousePosition.x,0, displayRect.getWidth(),0,selectedWindow->getImage().getWidth() ));
        selectedHandle->y=floor(ofMap(mousePosition.y,0,displayRect.getHeight(),0,selectedWindow->getImage().getHeight()));
    }
}
void WarpEditor::mouseReleased(int x, int y, int button){
    if(selectedHandle!=NULL){
        for(int i=0;i<4;i++){
            cout<<selectedWindow->srcPoints[i]<<endl;
        }
        ofPoint *srcPoints = selectedWindow->srcPoints;
        int imageWidth  = max(srcPoints[0].distance(srcPoints[1]),srcPoints[2].distance(srcPoints[3]));
        int imageHeight = max(srcPoints[0].distance(srcPoints[3]),srcPoints[1].distance(srcPoints[2]));
        //resize image
        selectedWindow->allocate(imageWidth,imageHeight);
    }
    selectedHandle = NULL;
}

void WarpEditor::setImage(ofxCvImage& image){
    origImage = &image;
    for (vector<WarpWindow*>::iterator it = windows.begin();it != windows.end();it++){
        (*it)->setImage(image);
    }
}
/*
 Draws editor inside the displayRect

*/
void WarpEditor::draw()
{

    ofPushStyle();
    ofColor editorColor(255,0,0);
    origImage->draw(displayRect);

    for (vector<WarpWindow*>::iterator it = windows.begin();it != windows.end();it++){
        ofSetColor(editorColor);
        ofPolyline line;
        for(int i=0;i<4;i++){
            ofPoint pos;
            pos.x=ofMap((*it)->srcPoints[i].x,0,origImage->getWidth(),0,displayRect.getWidth());
            pos.y=ofMap((*it)->srcPoints[i].y,0,origImage->getHeight(),0,displayRect.getHeight());
            pos += displayRect.getPosition() ;
            line.addVertex( pos);
            ofCircle(pos,handleRadius);
        }
        line.close();
        line.draw();
    }
    ofPopStyle();


}

void WarpEditor::addWindow(WarpWindow& ww){
    windows.push_back(&ww);
}
void WarpEditor::loadFile(string filename){

    ofXml windowsXml;
    windowsXml.load(filename);
    //TODO:check if there are instances
    windows.clear();
    if(windowsXml.getNumChildren()>0){
        windowsXml.setToChild(0);
        do{
            int wwidth,wheight;
            string name = windowsXml.getAttribute("name");
            istringstream (windowsXml.getAttribute("width")) >>wwidth;
            istringstream (windowsXml.getAttribute("height"))>>wheight;


            WarpWindow *ww = new WarpWindow();
            ww->name=name;
            windows.push_back(ww);
            ww->allocate(wwidth,wheight);
            ww->setImage(*origImage);

           if(windowsXml.getNumChildren()>0){
                windowsXml.setToChild(0);
                int vertexIndex = 0;
                do{
                    ofPoint v;
                    //float to String
                    istringstream (windowsXml.getAttribute("x"))>>v.x;
                    istringstream (windowsXml.getAttribute("y"))>>v.y;
                    ww->srcPoints[vertexIndex]=v;
                    cout<<"["<<vertexIndex<<"] "<<v<<endl;
                    vertexIndex ++ ;

                }while(windowsXml.setToSibling() && vertexIndex<4);
                windowsXml.setToParent();
            }
        }while(windowsXml.setToSibling());
    }

}
void WarpEditor::saveFile(string filename){

    ofXml windowsXml;
    windowsXml.addChild("windows");
    for (vector<WarpWindow*>::iterator it = windows.begin();it != windows.end();it++){
        ofXml windowXml;
        windowXml.addChild("window");
        windowXml.setAttribute("name",(*it)->name);

        stringstream ss;
        ss  << (*it)->getWidth();
        windowXml.setAttribute("width",ss.str());
        ss.str("");
        ss << (*it)->getHeight();
        windowXml.setAttribute("height",ss.str());

        for(int i=0;i<4;i++){
            ofXml vertexXml;
            vertexXml.addChild("vertex");
            ofPoint v = (*it)->srcPoints[i];
            ss.str("");
            ss << v.x;
            vertexXml.setAttribute("x",ss.str());
            ss.str("");
            ss << v.y;
            vertexXml.setAttribute("y",ss.str());
            windowXml.addXml(vertexXml);
        }
        windowsXml.addXml(windowXml);

    }
     windowsXml.save(filename);
}

