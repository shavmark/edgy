#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>

using namespace ofxCv;
using namespace cv;

class Contours : public ContourFinder {
public:
	Contours(const ofColor &color, float threshold);
	void draw(float x, float y);
	ofMesh mesh;
private:
};


class Image;
class MyThread : public ofThread {
public:
	shared_ptr<Image>image;
	shared_ptr<Contours> get();
	queue<shared_ptr<Contours>> tracedata;
private:
	void threadedFunction();
};
class Image  {
public:
	Image(string& filename);
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
	vector<shared_ptr<ofColor>> colorsList;
	vector<shared_ptr<ofColor>> ignoredData;
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
	std::unordered_set<int>  shapes; // hash for quick lookup

};


class LiveArt {
public:
	void setup();
	void update();
	void draw();

	bool LiveArt::loadAndFilter(shared_ptr<Image> image);
	void setMenu(ofxPanel &gui);
	void setTargetColor(const ofColor&c);
	static void toFile(ofFile& resultsfile, vector<shared_ptr<ofColor> > &dat, bool clear);
	static void toFileHumanForm(ofFile& resultsfile, vector<shared_ptr<ofColor> > &dat, bool clear);
	void advanceImage();
	void haveBeenNotifiedFloat(float &f);
	void haveBeenNotifiedInt(int &i);
	void haveBeenNotifiedBool(bool &b);
	void haveBeenNotifiedDouble(double &d);
	void redoButtonPressed();
	// read from xml file, 'r' key will refresh data? 

	ofParameter<float> minRadius;
	ofParameter<float> maxRadius;
	ofParameter<int> allColors;
	ofParameter<int> xImage=500;// make bigger when presenting, smaller when getting colors
	ofParameter<int> yImage = 500;
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
	ofParameter<int> sortby=2;


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

