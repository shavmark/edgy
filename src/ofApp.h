#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include <unordered_map>
<<<<<<< HEAD
#include <unordered_set>
=======

>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
using namespace ofxCv;
using namespace cv;

class Drawing {
public:
	ContourFinder contourFinder;
	ofColor color;
	
	
};
class ofApp : public ofBaseApp {
public:
<<<<<<< HEAD
	void echo(ContourFinder& finder, bool cleanPixelsOnly=false);
=======
	void echo(ofColor&color, ContourFinder& finder);
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
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
<<<<<<< HEAD
=======
	int index = 0;
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
	vector<ofPolyline> polys;
	int savecount = 0;
	cv::Mat img;
	bool once = false;
<<<<<<< HEAD
	void readColors(unordered_map<int, int> &findDarkcolors, unordered_map<int, int>& findBrightcolors);
	int savex;
	vector<cv::Rect> bounding;
=======
	
	int savex;

>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
	ofxPanel gui;
	ofParameterGroup group;
	ofParameter<string> imagePath;
	ofParameter<float> threshold;
<<<<<<< HEAD
	ofParameter<int> count;
	ofParameter<int> index;
	ofParameter<ofColor>targetColor;
	unordered_set<string> hitPixels;
	ofParameter<bool> generatecolors;
	vector<ofColor> savedcolors; //bugbug make unique, right now will store dupes
	ofColor warm;
=======
	ofParameter<ofColor>targetColor;
	
	ofParameter<bool> generatecolors;
	vector<ofColor> savedcolors; //bugbug make unique, right now will store dupes

>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
};

