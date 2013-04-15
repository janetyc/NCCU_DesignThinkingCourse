#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	ofBackground(0,0,0);
    
    //load background image
    backgroundImg.loadImage("images/background.png");
    
    //load button image
    //button 1
    btImg1[0].loadImage("images/bt1.png");
    btImg1[1].loadImage("images/bt1_pressed2.png");
    
    //button 2
    btImg2[0].loadImage("images/bt2.png");
    btImg2[1].loadImage("images/bt2_pressed2.png");
    
    ofSetFrameRate(24); // 24fps
    //load animation image
    for (int i = 0; i < AINMATIONFRAMENUM; i++) {
        char char1[32];
        sprintf(char1, "images/creature%d.png", i+1);  
        animationImg[i].loadImage(char1);
    }
    
    pressed1 = false;
    pressed2 = false;
    
    animationPlay = false;
    currFrame = 0;
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    ofEnableAlphaBlending();
	backgroundImg.draw(0,0);
    
    //play animation
    playAnimation();
    
    //draw button 1
    if (pressed1) {
        btImg1[1].draw(300,200);
        animationPlay = true;
    }else {
        btImg1[0].draw(300,200);
    }

    //draw button 2
    if (pressed2) {
        btImg2[1].draw(300,320);
    }else {
        btImg2[0].draw(300,320);
    }
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    //check if button 1 is pressed or not
    if ((300 <= int(touch.x)) && (int(touch.x) <= (300+btImg1[0].width))) {
        if ((200 <= int(touch.y)) && (int(touch.y) <= (200+btImg1[0].height))) {
            pressed1 = true;
        }
    }
    
    //check if button 2 is pressed or not
    if ((300 <= int(touch.x)) && (int(touch.x) <= (300+btImg1[0].width))) {
        if ((320 <= int(touch.y)) && (int(touch.y) <= (320+btImg1[0].height))) {
            pressed2 = true;
        }
    }
    
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
    pressed1 = false;
    pressed2 = false;
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

//--------------------------------------------------------------
void testApp::playAnimation(){
    if (animationPlay) {
        
        if (currFrame < AINMATIONFRAMENUM) {
            animationImg[currFrame].draw(700,350);
        }else {
            animationPlay = false;
            currFrame = 0;
        }
        currFrame++;
    } 
}