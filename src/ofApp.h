#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include <unordered_map>

using namespace ofxCv;
using namespace cv;

class Drawing {
public:
	ContourFinder contourFinder;
	ofColor color;
	
	
};
class ofApp : public ofBaseApp {
public:
	void echo(ofColor&color, ContourFinder& finder);
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
	std::vector<std::pair<ofColor, int>> *pairsCurrent=nullptr;
	int index = 0;
	vector<ofPolyline> polys;
	int savecount = 0;
	cv::Mat img;
	bool once = false;
	
	int savex;

	ofxPanel gui;
	ofParameterGroup group;
	ofParameter<string> imagePath;
	ofParameter<float> threshold;
	ofParameter<ofColor>targetColor;
	
	ofParameter<bool> generatecolors;
	vector<ofColor> savedcolors; //bugbug make unique, right now will store dupes

};

