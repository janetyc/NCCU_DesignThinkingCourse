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
    
    backImg.loadImage("images/background.png");
    btImg[0].loadImage("images/bt1.png");
    btImg[1].loadImage("images/bt2.png");
    
    pressImg[0].loadImage("images/bt1_pressed2.png");
    pressImg[1].loadImage("images/bt2_pressed2.png");
    
    pressed[0] = false;
    pressed[1] = false;
    
    for (int i=0; i<ANIMATIONNUM; i++) {
        char char1[32];
        sprintf(char1, "images/creature%d.png", i+1);  
        aniImg[i].loadImage(char1);
    }
    
    currFrame = 0;
    
    stage =1;
    backImg2.loadImage("images/background2.png");
}

//--------------------------------------------------------------
void testApp::update(){
    if (stage == 1) {
        
    }else if(stage == 2){
    }else if(stage == 3){
    }else if(stage == 4){
    }  
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofEnableAlphaBlending();
    
    cout << "stage " << stage << endl;
    
    if (stage == 1) {
        backImg.draw(0, 0);
        
        if (pressed[0]) {
            pressImg[0].draw(200,100);
        }else {
            btImg[0].draw(200, 100);
        }
        
        if (pressed[1]) {
            pressImg[1].draw(200,300);
        }else {
            btImg[1].draw(200, 300);
        }
        
        aniImg[currFrame].draw(700,350);
        
        currFrame = (currFrame+1) % ANIMATIONNUM;
        
        /* if (currFrame < ANIMATIONNUM-1) {        
         currFrame++;
         }else {
         currFrame = 0;
         }*/
    }else if(stage == 2){
        backImg2.draw(0,0);
        btImg[1].draw(200, 300);
    }else if(stage == 3){
        
    }else if(stage == 4){
    }
	
   
    
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    if (stage == 1) {
        //check button 1 is pressed or not
        if ((200<=touch.x) && (touch.x<=(200+btImg[0].width))) {
            if ((100<=touch.y) && (touch.y<=(100+btImg[0].height))) {
                cout << "pressed 1" << endl;
                pressed[0] = true;
                
                //change stage 2
                stage = 2;
            }
        }
        
        
        if ((200<=touch.x) && (touch.x<=(200+btImg[1].width))) {
            if ((300<=touch.y) && (touch.y<=(300+btImg[1].height))) {
                cout << "pressed 2" << endl;
                pressed[1] = true;
            }
        }
    }else if(stage == 2){
    }else if(stage == 3){
    }else if(stage == 4){
    }    
    
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
    pressed[0] = false;
    pressed[1] = false;

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

