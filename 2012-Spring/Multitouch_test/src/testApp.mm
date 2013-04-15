#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// touch events will be sent to myTouchListener
	ofRegisterTouchEvents(this);
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	ofSetFrameRate(60);
    ofEnableAlphaBlending();
	
    touchCount = 0;
    touchUPNum = 20;
    sensorDist = 200;
    sensorNum = 2;
	for(int i=0;i<touchUPNum;i++)
	{
		touchLoc[i].set(-200,-200);
	}
    for(int i=0; i<sensorNum; i++) 
    {
        sensorID[i] = 0;
    }
    for(int i=0; i<6; i++) 
    {
        IDimage[i].loadImage(ofToString(i, 0)+".png");
    }
    sensorLoc[0].set(ofGetWidth()*0.3, ofGetHeight()*0.3);
    sensorLoc[1].set(ofGetWidth()*0.7, ofGetHeight()*0.6);

}


//--------------------------------------------------------------
void testApp::update(){
	sensorID[0] = 0;
    sensorID[1] = 0;
    
    // see how many points in 2 sensor zones
    for(int i=0;i<touchUPNum;i++)
	{
        if(ofDist(touchLoc[i].x, touchLoc[i].y, sensorLoc[0].x, sensorLoc[0].y) < sensorDist)
            sensorID[0] = sensorID[0]+1;
        if(ofDist(touchLoc[i].x, touchLoc[i].y, sensorLoc[1].x, sensorLoc[1].y) < sensorDist)
            sensorID[1] = sensorID[1]+1;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0);
    
    // display Info
    ofSetColor(255,255,255);
    ofDrawBitmapString("Touch Points: " + ofToString(touchCount, 0), 50, 20);
    ofDrawBitmapString("sensorZone 1: " + ofToString(sensorID[0], 0), 50, 40);
    ofDrawBitmapString("sensorZone 2: " + ofToString(sensorID[1], 0), 50, 60);
    
    // draw ID image
    if(sensorID[0]<6) {
        IDimage[sensorID[0]].draw(sensorLoc[0].x-IDimage[sensorID[0]].width/2, sensorLoc[0].y-IDimage[sensorID[0]].height/2);
        IDimage[sensorID[0]].draw(sensorLoc[0].x+80, sensorLoc[0].y-250, IDimage[sensorID[0]].width/4, IDimage[sensorID[0]].height/4);
    }
    if(sensorID[1]<6) {
        IDimage[sensorID[1]].draw(sensorLoc[1].x-IDimage[sensorID[1]].width/2, sensorLoc[1].y-IDimage[sensorID[1]].height/2);
        IDimage[sensorID[1]].draw(sensorLoc[1].x+80, sensorLoc[1].y-250, IDimage[sensorID[1]].width/4, IDimage[sensorID[1]].height/4);
    }
    
    // draw sensor Zone
    ofSetColor(220, 220, 50, 100);
    ofCircle(sensorLoc[0].x, sensorLoc[0].y, sensorDist);
    ofCircle(sensorLoc[1].x, sensorLoc[1].y, sensorDist);
    
	// draw touch points
    ofSetColor(200,200,200,100);
	for(int i=0;i<touchCount;i++)
	{
		ofCircle(touchLoc[i].x, touchLoc[i].y, 50);
	}
    
}

//--------------------------------------------------------------
void testApp::exit() {

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs & touch){
	touchLoc[touch.id].set(touch.x,touch.y);
    
    touchCount = touchCount+1;
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs & touch){
    touchLoc[touch.id].set(touch.x,touch.y);
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs & touch){
    touchLoc[touch.id].set(-200,-200);
    touchCount = touchCount-1;
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs & touch){

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

