#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	
	
    // register touch events
	//ofRegisterTouchEvents(this);
	
    ofxRegisterMultitouch(this);
    
    
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
    
	//If you want a landscape oreintation 
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);

	
	ofBackground(127,127,127);
    
    
    ofSetFrameRate(30);
    
    //background
    for (int i = 0; i < BACKFRAMENUM; i++) {
        char char1[32];
        sprintf(char1, "images/background%d.png", i+1);  
        backImage[i].loadImage(char1);
    }
    
    //foreground
    for (int i = 0; i < FOREFRAMENUM; i++) {
        char char1[32];
        sprintf(char1, "images/creature%d.png", i+1);
        foreImage[i].loadImage(char1);
    }
    currentFrame = 0;
    currentFrame2 = 0;
    
    pos_x=500;
    pos_y=350;
    
    count =0;
    
    
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	

    ofEnableAlphaBlending();
    backImage[currentFrame].draw(0, 0);
    foreImage[currentFrame2].draw(pos_x, pos_y);
        
    currentFrame++;
    currentFrame2++;

    if (currentFrame2 > FOREFRAMENUM - 1)   
        currentFrame2 = 0;

    if (currentFrame > BACKFRAMENUM - 1)    
        currentFrame = 0;
    
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------

void testApp::touchDown(ofTouchEventArgs &touch){
    if (pos_x > ofGetWidth()) {
        pos_x=-foreImage[0].width;
    }
    cout << pos_x << endl;
    pos_x=pos_x+10;
    
    
    printf("touch %i down at (%i,%i)\n", touch.id, int(touch.x), int(touch.y));
    
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::lostFocus(){

}

//--------------------------------------------------------------
void testApp::gotFocus(){

}

//--------------------------------------------------------------
void testApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){

}


//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}


