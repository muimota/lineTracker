#include "warpeditor.h"

void warpEditor::mousePressed(int x, int y, int button){
    ofPoint mousePosition(x,y);
    mousePosition-=displayRect.getPosition();
    for (vector<warpWindow>::iterator it = windows.begin();it != windows.end();it++){
        for(int i=0;i<4;i++){
            ofPoint pos;
            pos.x=ofMap(it->srcPoints[i].x,0,it->getImage().getWidth(),0,displayRect.getWidth());
            pos.y=ofMap(it->srcPoints[i].y,0,it->getImage().getHeight(),0,displayRect.getHeight());
            if(pos.distance(mousePosition)<handleRadius){
                cout<<"click!"<<endl;
                //http://stackoverflow.com/a/2160319
                selectedWindow = &(*it);
                selectedHandle = &it->srcPoints[i];
                handleOffset = pos-mousePosition;
            }
        }
    }
}
void warpEditor::mouseDragged(int x, int y, int button){
    if(selectedHandle!=NULL){
        ofPoint mousePosition(x,y);
        mousePosition-=displayRect.getPosition();
        selectedHandle->x=ofMap(mousePosition.x,0,displayRect.getWidth(),0,selectedWindow->getImage().getWidth()  );
        selectedHandle->y=ofMap(mousePosition.y,0,displayRect.getHeight(),0,selectedWindow->getImage().getHeight());
    }
}
void warpEditor::mouseReleased(int x, int y, int button){
     cout<<"released!"<<endl;
    selectedHandle = NULL;
}

void warpEditor::draw()
{
    if(!windows.empty()){
        ofPushStyle();
        ofColor editorColor(255,0,0);
        ofxCvImage &img = windows.front().getImage();
        img.draw(displayRect);

        for (vector<warpWindow>::iterator it = windows.begin();it != windows.end();it++){
            ofSetColor(editorColor);
            ofPolyline line;
            for(int i=0;i<4;i++){
                ofPoint pos;
                pos.x=ofMap(it->srcPoints[i].x,0,img.getWidth(),0,displayRect.getWidth());
                pos.y=ofMap(it->srcPoints[i].y,0,img.getHeight(),0,displayRect.getHeight());
                pos += displayRect.getPosition() ;
                line.addVertex( pos);
                ofCircle(pos,handleRadius);
            }
            line.close();
            line.draw();
        }
        ofPopStyle();
    }

}
