#include "ofApp.h"

//------------------------------------------------------------
/*
-----------------     TODOLIST -------------------------------

- GUI: Model voorbeeld , laden van voorbeeld in thread, verwerken in GUI
- GUI: Op W7 is OF altijd ontop met fullscreen in W8 niet, workaround?
- GUI: VRtropolis randen langs het scherm, voor de mooi.
- buildscript.py: Markers, werkt wel, nog steeds een beetje scheef, misschien meer?
- buildscript.py: Boundingbox, afsnijden model, scheelt behoorlijk bij laden van het model.

*/
//------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFullscreen(true);
    
    cam1.setVerbose(true);
    cam1.listDevices();
    
    
    cam1.setDeviceID(0);
    cam1.initGrabber(1920,1080,true);
    cam2.setDeviceID(1);
    cam2.initGrabber(1920,1080,true);
    
    camImage1.allocate(cam1.width, cam1.height, OF_IMAGE_COLOR);
    camImage2.allocate(cam2.width, cam2.height, OF_IMAGE_COLOR);
    
    //arduino.setup("/dev/cu.usbmodemfd1211",9600);
    arduino.setup("COM8",9600);
    
	pixelFont.loadFont("FONTS/pixel.ttf",45,true,false,true,0.3,72);
	pixelFontLarge.loadFont("FONTS/pixel.ttf",95,true,false,true,0.3,72);
	
	GUITrain.loadImage("IMAGES/train.gif");
	GUIWait.loadImage("IMAGES/wait.gif");

    isScanning = false;
	isProcessing = false;
    photoScanFinish = true;
    isReady = true;
	showStats=true;
    
    picturesTaken = 0;
    
    currentSavePath = "";

	checkDir();


    
}

//--------------------------------------------------------------
void ofApp::update(){
    cam1.update();
    cam2.update();
	
    if(!isProcessing){ // scan modus -- start scherm

		if(cam1.isFrameNew()){
			camImage1.setFromPixels(cam1.getPixelsRef());
		}
		if(cam2.isFrameNew()){
			camImage2.setFromPixels(cam2.getPixelsRef());
		}
    
		if(arduino.available()>4){
			unsigned char readBuffer[5];
			arduino.readBytes(readBuffer, 5);
        
			if(isScanning){
				scanStep();
			}
        
			printf("Received: %s \n",readBuffer);
        
        
		}

	}

	if(isProcessing){ // photoscanproces
		
		if(photoScanThread.isThreadRunning()){


		}
		else{

			watchPhotoscan();

		}

	}
    
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(10,10,30);
    
	if(isReady){ //Ready for action, welcome screen
		
		ofSetColor(255,0,250);
		string displayString = "Druk op de knop om te scannen";
		pixelFont.drawString(displayString,ofGetWidth()/2-(pixelFont.stringWidth(displayString)/2),ofGetHeight()-100);
		
	}

    if(!isProcessing && isScanning){ // Scanning, display cam output
		
		ofSetColor(255,255,255);

		int camW = 853;
		int camH = 480;

		cam1.draw((ofGetWidth()/2)-(camW/2) , 0, camW, camH);
		cam2.draw((ofGetWidth()/2)-(camW/2), ofGetHeight()-camH, camW, camH);

		//Other GUI-shit here
		GUITrain.draw(0 + ((ofGetWidth()-200)/PICTUREAMOUNT) * picturesTaken,(ofGetHeight()/2)-100,1,200,100);
		
		ofSetColor(255,0,250);
		ofSetLineWidth(3);

		ofLine(0,ofGetHeight()/2,2,ofGetWidth(),ofGetHeight()/2,2);
		for(int i=0; i<ofGetWidth(); i+=20){
			ofLine(i,(ofGetHeight()/2)+2,2,i+5,(ofGetHeight()/2)+2,2);
			ofLine(i,(ofGetHeight()/2)+4,2,i+5,(ofGetHeight()/2)+4,2);
		}

		string displayString = "Scannen";
		pixelFont.drawString(displayString,ofGetWidth()/2-(pixelFont.stringWidth(displayString)/2),(ofGetHeight()/2)+80);

	}

	if(isProcessing){ //waiting with comes with hourglasses 
		
		ofPushMatrix(); //right mid
		ofTranslate(ofGetWidth()-450,(ofGetHeight()/2)-200);
		ofRotateX((ofGetElapsedTimeMillis()/25)%360);
		ofRotateY((ofGetElapsedTimeMillis()/30)%360);
		GUIWait.draw(-(GUIWait.width / 2 ),-(GUIWait.height / 2),-2,147,250);
		ofPopMatrix();

		ofPushMatrix(); //left mid
		ofTranslate(450,(ofGetHeight()/2)-200);
		ofRotateX((ofGetElapsedTimeMillis()/30)%360);
		ofRotateY((ofGetElapsedTimeMillis()/25)%360);
		GUIWait.draw(-(GUIWait.width / 2 ),-(GUIWait.height / 2),-2,147,250);
		ofPopMatrix();

		ofPushMatrix(); //top
		ofTranslate(ofGetWidth()/2,ofGetHeight()/2);
		ofRotateX((ofGetElapsedTimeMillis()/50)%360);
		ofRotateY((ofGetElapsedTimeMillis()/30)%360);
		GUIWait.draw(-(GUIWait.width / 2 ),-(GUIWait.height / 2));
		ofPopMatrix();

		ofPushMatrix(); //left
		ofTranslate(200,ofGetHeight()/2);
		ofRotateX(-(ofGetElapsedTimeMillis()/20)%360);
		ofRotateY(-(ofGetElapsedTimeMillis()/30)%360);
		GUIWait.draw(-(GUIWait.width / 2 ),-(GUIWait.height / 2),2,147,250);
		ofPopMatrix();

		ofPushMatrix(); //right
		ofTranslate(ofGetWidth()-200,ofGetHeight()/2);
		ofRotateX(-(ofGetElapsedTimeMillis()/30)%360);
		ofRotateY(-(ofGetElapsedTimeMillis()/20)%360);
		GUIWait.draw(-(GUIWait.width / 2 ),-(GUIWait.height / 2),2,147,250);
		ofPopMatrix();

		ofSetColor(255);

		string displayString = "EVEN WACHTEN";
		pixelFontLarge.drawString(displayString,ofGetWidth()/2-(pixelFontLarge.stringWidth(displayString)/2),130);

		displayString = "Bouwen wat ik net heb gezien";
		pixelFont.drawString(displayString,ofGetWidth()/2-(pixelFont.stringWidth(displayString)/2),ofGetHeight()-100);

	}


	if(showStats){
		showStatistics();
	}   
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch(key){
        case 'a':
            
            if(arduino.isInitialized()){
				photoScanThread.startThread(true,true);
            }
            
            break;
            
        case 's':
            if(!isScanning){
                startScan();
            }
            
            break;

		case '1':
			cam1.videoSettings();
			break;

		case '2':
			cam2.videoSettings();
			break;
		case '=':
			if(showStats){
				showStats = false;
			}
			else{
				showStats = true;
			}
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
void ofApp::exit(){
    
	
	cam1.close();
    cam2.close();


}

void ofApp::startScan(){ //start scanning
    

	ofFile deleteFiles;
	deleteFiles.removeFile("PHOTOSCAN_RESULT/result.obj");
	deleteFiles.removeFile("PHOTOSCAN_RESULT/result.mtl");
	deleteFiles.removeFile("PHOTOSCAN_RESULT/result.png");

    if(arduino.isInitialized()){
        
        moveTrain();
        
        isScanning = true;
        isReady = false;
        
        currentSavePath = ofGetTimestampString("%d-%e--%m-%s");
        ofDirectory savePath(currentSavePath);
        
        if(!savePath.exists()){
            savePath.create(false);
        }
        
        
    }

    
}
void ofApp::scanStep(){ //drive train, take picture
    

    if(isScanning && picturesTaken < PICTUREAMOUNT){
        
		Sleep(1000);

        picturesTaken++;
        
        camImage1.saveImage(currentSavePath + "/C1-P" + ofToString(picturesTaken) + ".jpg", OF_IMAGE_QUALITY_BEST);
		camImage2.saveImage(currentSavePath + "/C2-P" + ofToString(picturesTaken) + ".jpg", OF_IMAGE_QUALITY_BEST);
		
        
		moveTrain();
        
        
    }
    if(picturesTaken >= PICTUREAMOUNT){
        
        isScanning = false;
        endScan();
        
    }
    

}
void ofApp::endScan(){ //write pictures to directory, start photoscan
    

	picturesTaken = 0; //reset picture counter!!

    printf("Call photoscan \n");
    
	//move files to PSP-workpath
	ofDirectory savePath(currentSavePath);
	savePath.copyTo("PHOTOSCAN_PROCES/",true,true);

	ofDirectory copyPath("PHOTOSCAN_PROCES/"+currentSavePath);
	copyPath.renameTo("PHOTOSCAN_PROCES/CURRENT_SET"); //remove dir after use

	//start psp thread
	photoScanThread.startThread();
	    
    isProcessing = true;


}
void ofApp::watchPhotoscan(){ //watch for photoscan output, moving output files and cleaning up
   

	printf("Photoscan done: copy files");

	ofFile modResult("PHOTOSCAN_RESULT/result.obj");
	modResult.copyTo("PHOTOSCAN_OUTPUT/" + currentSavePath + ".obj",true);
	ofFile texResult("PHOTOSCAN_RESULT/result.png");
	texResult.copyTo("PHOTOSCAN_OUTPUT/" + currentSavePath + ".png",true);
	ofFile mtlResult("PHOTOSCAN_RESULT/result.mtl");
	mtlResult.copyTo("PHOTOSCAN_OUTPUT/" + currentSavePath + ".mtl",true);

	isProcessing = false;
	isReady = true; //temp, still needs to evaluate model


}
void ofApp::moveTrain(){
    

    unsigned char sendString[5] = {'0','7','5','0','F'};
    arduino.writeBytes(sendString, sizeof(sendString));
    

}

void ofApp::checkDir(){


	ofDirectory procesDir("PHOTOSCAN_PROCES");
	ofDirectory resultDir("PHOTOSCAN_RESULT");
	ofDirectory pspOutput("PHOTOSCAN_OUTPUT");

	if(!procesDir.exists()){
		procesDir.create();
		printf("Created process dir");
	}
	
	if(!resultDir.exists()){
		resultDir.create();
		printf("Created result dir");
	}

	if(!pspOutput.exists()){
		pspOutput.create();
		printf("Created result dir");
	}

	
}
void ofApp::showStatistics(){
	ofPushMatrix();
	ofTranslate(10,ofGetHeight()-70);
	ofSetColor(255);

	ofLine(0,-11,ofGetWidth(),-11);

	ofDrawBitmapString("Current Freemreet:  " + ofToString(ofGetFrameRate()),0,0,0);
	ofDrawBitmapString("Arduino Init:       " + ofToString(arduino.isInitialized()),0,10,0);
	ofDrawBitmapString("Current exportname: " + ofToString(currentSavePath),0,20,0);

	ofDrawBitmapString("Thread running:     " + ofToString(photoScanThread.isThreadRunning()),0,40,0);

	ofPopMatrix();
}