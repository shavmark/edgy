#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include <unordered_map>
#include <unordered_set>
using namespace ofxCv;
using namespace cv;
class LiveArt {
public:
	class colorData {
	public:
		colorData() {}
		colorData(const ofColor& data) { color = data; } //bugbut not everything is copied
		int operator==(const colorData& data) { return data.color == color; }
		ofColor color;
		float threshold = 0;
		vector<ofPolyline> lines;
	};

	void setup();
	void update();
	void draw();

	void setMenu(ofxPanel &gui);
	void echo(vector<ofPolyline>&lines);
	bool find(ofColor&color, bool add);
	bool test(ofColor&color, int i, int j, int k);
	bool dedupe(ofColor&color, int rangeR, int rangeG, int rangeB);
	void readColors();
	
	void toFile(string path, vector<std::pair<ofColor, int>>&dat);
	void toFile(string path, vector<ofColor>&dat, bool clear);
	void fromFile(string path, vector<ofColor> &dat);
	static bool isCool(ofColor&color);
	vector<colorData> drawingData;

	// read from xml file, 'r' key will refresh data? 

	ofParameter<float> minRadius = 1;
	ofParameter<float> maxRadius = 150;
	ofParameter<bool> findHoles = true;
	ofParameter<int> smoothingSize = 2;//learn
	ofParameter<float> smoothingShape = 0.0;//bugbug learn
	ofParameter<float> xImage = 500;// make bigger when presenting, smaller when getting colors
	ofParameter<float> yImage = 500;
	ofParameter<int> d = 15; //bugbug learn for bilateralfilter
	ofParameter<double> sigmaColor = 80; //bugbug learn for bilateralfilter
	ofParameter<double> sigmaSpace = 80; //bugbug learn for bilateralfilter
	ofParameter<string> imagePath = "photo2.jpg";

	ofParameter<float> threshold;
	ofParameter<int> count;
	ofParameter<int> index;
	ofParameter<ofColor>targetColor;
	ofParameter<ofColor>warm;

	ofImage image;
	cv::Mat img;
	
	void snapshot();
	void restore(){ index = savex; }

private:
	map<int, colorData> shapes; //optimization
	vector<ofColor> savedcolors; //bugbug make unique, right now will store dupes
	int savex = 0;
	int savecount = 0;

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

