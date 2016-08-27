#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>

using namespace ofxCv;
using namespace cv;

class drawingData {
public:
	void draw() {
		mesh.draw();
	}
	ofColor getTargetColor() { return targetColor; }
	ofMesh mesh;
	ofColor targetColor;
private:
};
class myContourFinder  {
public:
	enum TrackingColorMode { TRACK_COLOR_RGB, TRACK_COLOR_HSV, TRACK_COLOR_H, TRACK_COLOR_HS };
	bool findContours(Mat img, shared_ptr<drawingData>);
	void setThreshold(float t) { thresholdValue = t; }
	ofColor& getTargetColor() { return targetColor; }
	void setTargetColor(const ofColor& c) { targetColor = c; }
	bool useTargetColor = true;
	float thresholdValue = 5;
	ofColor targetColor;
	TrackingColorMode trackingColorMode= TRACK_COLOR_RGB;
	
};

class colorData : public myContourFinder {
public:
	colorData() {}
	colorData(const ofColor& color) { setTargetColor(color); } //bugbut not everything is copied
	int operator==(const colorData& data) { return targetColor == data.targetColor; }
	bool isCool();
private:
};
typedef map<int, colorData> Shapes;
class Image;
class MyThread : public ofThread {
public:
	shared_ptr<Image>image;
	shared_ptr<drawingData> get();
	queue<shared_ptr<drawingData>> tracedata;
	bool isDone() { return done; }
	void setDone(bool b = true) { done = b; }
private:
	bool done = false;
	void threadedFunction();
};
class Image  {
public:
	Image(string& filename);
	int index = 0;
	int hits = 0;
	float threshold;
	bool findOrAdd(const ofColor&color, bool add);
	bool testForExistance(ofColor color, int i, int j, int k);// test for existance
	bool dedupe(ofColor color, int rangeR, int rangeG, int rangeB);
	void readColors();
	static void rgbToryb(const ofColor& in, ofParameter<ofColor>& red, ofParameter<ofColor>& yellow, ofParameter<ofColor>& blue);
	void filter(int id, ofParameter<int> a, ofParameter<double> b, ofParameter<double> c);
	string name;
	string shortname;
	vector<shared_ptr<colorData>> drawingData;
	vector<shared_ptr<colorData>> ignoredData;
	int lineThreshHold = 0;
	ofImage img;//both images stored for Convenience  of the progammer
	cv::Mat mat;
	ofParameter<ofColor>warm;
	ofParameter<int> shrinkby = 5; // 5 shaves off some brightness?, 3 is pretty good but slow. 2 is slow and no much better to look at, 4 is ok, not too fast
	string logDir="logs\\";
	bool readIn = false;
	MyThread mythread;
	ofParameter<int>sortby=0;
	int allcolors = 0;
private:
	static void rgb2ryb(unsigned char &r, unsigned char g, unsigned char &b, unsigned char&y);
	Shapes shapes; // hash for quick lookup

};


class LiveArt {
public:
	void setup();
	void update();
	void draw();

	bool LiveArt::loadAndFilter(shared_ptr<Image> image);
	void setMenu(ofxPanel &gui);
	void echo(ofMesh&mesh);
	void setTargetColor(const ofColor&c);
	static void toFile(ofFile& resultsfile, vector<shared_ptr<colorData> > dat, bool clear);
	static void toFileHumanForm(ofFile& resultsfile, vector<shared_ptr<colorData> > dat, bool clear);
	void advanceImage();
	void haveBeenNotifiedFloat(float &f);
	void haveBeenNotifiedInt(int &i);
	void haveBeenNotifiedBool(bool &b);
	void haveBeenNotifiedDouble(double &d);
	void redoButtonPressed();
	bool toscreen(shared_ptr<drawingData>data);
	bool toscreen(drawingData& data);
	// read from xml file, 'r' key will refresh data? 

	ofParameter<float> minRadius;
	ofParameter<float> maxRadius;
	ofParameter<bool> findHoles;
	ofParameter<int> smoothingSize;//learn
	ofParameter<int> allColors;
	ofParameter<float> smoothingShape;//bugbug learn
	ofParameter<int> xImage;// make bigger when presenting, smaller when getting colors
	ofParameter<int> yImage;
	ofParameter<int> d; //bugbug learn for bilateralfilter and enable other filters
	ofParameter<double> sigmaColor; //bugbug learn for bilateralfilter
	ofParameter<double> sigmaSpace; //bugbug learn for bilateralfilter
	ofParameter<string>currentImageName;
	ofParameter<float> threshold;
	ofParameter<int> count;
	ofParameter<ofColor>targetColor;
	ofxButton redo;
	ofxButton cancel;
	ofParameter<ofColor>red;
	ofParameter<ofColor>yellow;
	ofParameter<ofColor>blue;

	ofParameter<int> currentImage = 0;
	vector<shared_ptr<Image>> images;
	ofParameter<int> sortby=0;


private:
	int getImages();
	bool readIn = false;
};
class ofApp : public ofBaseApp {
public:

	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void mousePressed(int x, int y, int button);
	ofVideoGrabber cam;
	ofImage gray, edge, sobel;
	ofxPanel gui;
	LiveArt art;

private:

};

