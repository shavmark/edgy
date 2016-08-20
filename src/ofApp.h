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
	int operator==(colorData data) { return data.color == color; }
	ofColor color;
	int count;
	vector<ofPolyline> lines;
};
class ofApp : public ofBaseApp {
public:
	void echo(vector<ofPolyline>&lines);
	bool find(ofColor&color, bool add);
	bool test(ofColor&color, int i, int j, int k);
	bool dedupe(ofColor&color, int rangeR, int rangeG, int rangeB);
	void setup();
	void update();
	void draw();
	ofVideoGrabber cam;
	ofImage gray, edge, sobel;
	ofImage image;
	void keyPressed(int key);
	vector<colorData> shapes;
	
	int savecount = 0;
	cv::Mat img;
	void readColors();
	int savex;
	ofxPanel gui;
	ofParameterGroup group;
	ofParameter<string> imagePath;
	ofParameter<float> threshold;
	ofParameter<int> count;
	ofParameter<int> index;
	ofParameter<ofColor>targetColor;
	unordered_set<string> hitPixels;
	ofParameter<bool> generatecolors;
	vector<ofColor> savedcolors; //bugbug make unique, right now will store dupes
	ofColor warm;
};

