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
	
	cv::Mat img;
	ofxPanel gui;
	ofParameterGroup group;
	ofParameter<float> threshold;
	ofParameter<int> count;
	ofParameter<int> index;
	ofParameter<ofColor>targetColor;
	ofColor warm;
	vector<colorData> drawingData;

	// read from xml file, 'r' key will refresh data? 

	float minRadius = 1;
	float maxRadius = 150;
	bool findHoles = true;
	int smoothingSize=2;//learn
	float smoothingShape = 0.0;//bugbug learn
	float xImage = 500;// make bigger when presenting, smaller when getting colors
	float yImage = 500;
	int d = 15; //bugbug learn for bilateralfilter
	double sigmaColor = 80; //bugbug learn for bilateralfilter
	double sigmaSpace = 80; //bugbug learn for bilateralfilter
	string imagePath= "photo2.jpg";

private:
	vector<ofColor> savedcolors; //bugbug make unique, right now will store dupes
	map<int, colorData> shapes; //optimization
	int savecount = 0;
	int savex;
};

