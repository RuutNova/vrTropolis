#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

class assimpThread : public ofThread {

	public:

	bool					modelIsLoaded;
	ofxAssimpModelLoader	modelLoader;
	string					fileName;

	private:
	    
    void assimpThread::threadedFunction() {
		
		modelIsLoaded = false;
		
		if(fileName.compare("") != 0){

			while(isThreadRunning()){

				if(modelLoader.loadModel("PHOTOSCAN_OUTPUT/" + fileName + ".obj") && modelIsLoaded == false){
					modelIsLoaded = true;
				}
				else{
					modelIsLoaded = false;
				}

			}

		}
		else{
			printf("filename is empty!");
		}
        
	}
 
};