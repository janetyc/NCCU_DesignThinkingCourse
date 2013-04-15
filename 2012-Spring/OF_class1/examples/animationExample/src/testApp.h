#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

#define BACKFRAMENUM 10
#define FOREFRAMENUM 5

class testApp : public ofxiPhoneApp {
	
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);

	void lostFocus();
	void gotFocus();
	void gotMemoryWarning();
	void deviceOrientationChanged(int newOrientation);

    ofImage myImage;
    ofImage backImage[BACKFRAMENUM];
    ofImage foreImage[FOREFRAMENUM];
    //ofImage myImage;
    
    
    int currentFrame, currentFrame2;
    int pos_x, pos_y;
    int count;
};


