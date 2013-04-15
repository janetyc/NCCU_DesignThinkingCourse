#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxOpenALSoundPlayer.h"

class testApp : public ofxiPhoneApp {
	
	public:
		void setup();
		void update();
		void draw();

		void touchDown(ofTouchEventArgs & touch);
		void touchMoved(ofTouchEventArgs & touch);
		void touchUp(ofTouchEventArgs & touch);
		void touchDoubleTap(ofTouchEventArgs & touch);
		void touchCancelled(ofTouchEventArgs &touch);
		
		void exit();
		
		void lostFocus();
		void gotFocus();
		void gotMemoryWarning();
		void deviceOrientationChanged(int newOrientation);
		
		
        int touchUPNum;
        int touchCount;
        int sensorDist;
        int sensorNum;
		ofPoint touchLoc[20]; // one for each possible touch ID
        ofPoint sensorLoc[2];
        int sensorID[2];
    
        ofImage IDimage[6];
    
	
};
