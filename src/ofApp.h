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
	colorData(const ofColor& data) { color = data;  } //bugbut not everything is copied
	int operator==(const colorData& data) { return data.color == color; }
	ofColor color;
	float threshold = 0;
	vector<ofPolyline> lines;
};
class ofApp : public ofBaseApp {
public:
	void echo(vector<ofPolyline>&lines);
	bool find(ofColor&color, bool add);
	bool test(ofColor&color, int i, int j, int k);
	bool dedupe(ofColor&color, int rangeR, int rangeG, int rangeB);
	void readColors();
	void keyPressed(int key);

	void setup();
	void update();
	void draw();

	ofVideoGrabber cam;
	ofImage gray, edge, sobel;
	ofImage image;
	vector<colorData> shapes;
	
	cv::Mat img;
	ofxPanel gui;
	ofParameterGroup group;
	ofParameter<string> imagePath;
	ofParameter<float> threshold;
	ofParameter<int> count;
	ofParameter<int> index;
	ofParameter<ofColor>targetColor;
	ofParameter<bool> generatecolors;
	vector<ofColor> savedcolors; //bugbug make unique, right now will store dupes
	ofColor warm;

private:
	map<int, int> colorhash; //optimization
	int savecount = 0;
	int savex;
};

