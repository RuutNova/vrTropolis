#pragma once

#include "ofMain.h"
#include "pspThread.h"

#define PICTUREAMOUNT 8

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void exit();
		
		void checkDir();

        void startScan();
        void scanStep();
        void endScan();
        void watchPhotoscan();
        void moveTrain();

		void showStatistics();
    
    
    ofVideoGrabber  cam1, cam2;
    ofSerial        arduino;
    ofImage         camImage1, camImage2, GUITrain, GUIWait;
	pspThread		photoScanThread;
	ofTrueTypeFont	pixelFont, pixelFontLarge;
    
    int     picturesTaken;
    bool    isScanning, photoScanFinish, isReady, isProcessing, showStats;
    string  currentSavePath;
    
};
