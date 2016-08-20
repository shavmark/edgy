#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include <unordered_map>
#include <unordered_set>
using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp {
public:
	void echo(vector<ofPolyline>&lines);
	void setup();
	void update();
	void draw();
	ofVideoGrabber cam;
	ofImage gray, edge, sobel;
	ofImage image;
	void keyPressed(int key);
	vector<pair<ofColor, int>> colors;
	vector<pair<ofColor, vector<ofPolyline>>> shapes;
	
	int savecount = 0;
	cv::Mat img;
	bool once = false;
	void readColors(unordered_map<int, int> &colors);
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

