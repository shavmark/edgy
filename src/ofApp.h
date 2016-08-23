#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include <unordered_map>
#include <unordered_set>
using namespace ofxCv;
using namespace cv;

class colorData {
public:
	colorData() {}
	colorData(const ofColor& data) { color = data; } //bugbut not everything is copied
	int operator==(const colorData& data) { return data.color == color; }
	ofColor color;
	float threshold = 0;
	vector<ofPolyline> lines;
	bool isCool();
};
typedef map<int, colorData> Shapes;
class Image {
public:
	Image(const string filename) { name = ofToDataPath(string("\\images\\") + filename, true); 	warm = ofColor::lightYellow; }

	bool findOrAdd(const ofColor&color, bool add);
	bool testForExistance(ofColor color, int i, int j, int k);// test for existance
	bool dedupe(ofColor color, int rangeR, int rangeG, int rangeB);
	void readColors();

	string name;
	Shapes shapes;
	vector<colorData> drawingData;
	ofImage img;//both images stored for Convenience  of the progammer
	cv::Mat mat;
	ofParameter<ofColor>warm;
	ofParameter<int> shrinkby = 3;

};


class LiveArt {
public:
	void setup();
	void update();
	void draw();

	bool LiveArt::loadAndFilter(Image& image);
	void setMenu(ofxPanel &gui);
	void echo(vector<ofPolyline>&lines);
	
	static void toFile(ofFile& resultsfile, vector<std::pair<ofColor, int>>&dat);
	static void toFile(ofFile& resultsfile, vector<ofColor>&dat, bool clear);
	static void fromFile(ofFile& resultsfile, vector<ofColor> &dat);

	// read from xml file, 'r' key will refresh data? 

	ofParameter<float> minRadius;
	ofParameter<float> maxRadius;
	ofParameter<bool> findHoles;
	ofParameter<int> smoothingSize;//learn
	ofParameter<float> smoothingShape;//bugbug learn
	ofParameter<int> xImage;// make bigger when presenting, smaller when getting colors
	ofParameter<int> yImage;
	ofParameter<int> d; //bugbug learn for bilateralfilter and enable other filters
	ofParameter<double> sigmaColor; //bugbug learn for bilateralfilter
	ofParameter<double> sigmaSpace; //bugbug learn for bilateralfilter
	ofParameter<string>currentImageName;
	ofParameter<float> threshold;
	ofParameter<int> count;
	ofParameter<int> index;
	ofParameter<ofColor>targetColor;
	ofParameter<int> currentImage = 0;
	vector<Image> images;
	ofParameter<int> searchBy=1;

	void snapshot();
	void restore(){ index = savex; }

private:
	vector<ofColor> savedcolors; //bugbug make unique, right now will store dupes
	int savex = 0;
	int savecount = 0;
	int getImages();

};
class ofApp : public ofBaseApp {
public:

	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	ofVideoGrabber cam;
	ofImage gray, edge, sobel;
	ofxPanel gui;
	LiveArt art;

private:

};

