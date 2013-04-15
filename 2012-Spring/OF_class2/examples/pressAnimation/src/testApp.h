#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

#define AINMATIONFRAMENUM 13

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

    void playAnimation();
    
    
    ofImage backgroundImg;
    ofImage animationImg[AINMATIONFRAMENUM];
    ofImage btImg1[2];
    ofImage btImg2[2];
    
    bool pressed1;
    bool pressed2;
    
    bool animationPlay;
    int currFrame;
};


