#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include <unordered_map>
#include <unordered_set>
using namespace ofxCv;
using namespace cv;

class Drawing {
public:
	ContourFinder contourFinder;
	ofColor color;


};
class ofApp : public ofBaseApp {
public:
	void echo(ContourFinder& finder, bool cleanPixelsOnly = false);
	void setup();
	void update();
	void draw();
	ofVideoGrabber cam;
	ofImage gray, edge, sobel;
	ofImage image;
	vector<Drawing> drawings;
	void keyPressed(int key);
	std::vector<std::pair<ofColor, int>> pairsDark;
	std::vector<std::pair<ofColor, int>> pairsLight;
	std::vector<std::pair<ofColor, int>> *pairsCurrent = nullptr;
	vector<ofPolyline> polys;
	int savecount = 0;
	cv::Mat img;
	bool once = false;
	void readColors(unordered_map<int, int> &findDarkcolors, unordered_map<int, int>& findBrightcolors);
	int savex;
	vector<cv::Rect> bounding;
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

