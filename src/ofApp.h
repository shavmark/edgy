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
	Image(const string& filename);

	bool findOrAdd(const ofColor&color, bool add);
	bool testForExistance(ofColor color, int i, int j, int k);// test for existance
	bool dedupe(ofColor color, int rangeR, int rangeG, int rangeB);
	void readColors();
	static void rgbToryb(const ofColor& in, ofParameter<ofColor>& red, ofParameter<ofColor>& yellow, ofParameter<ofColor>& blue);
	string name;
	string shortname;
	Shapes shapes;
	vector<colorData> drawingData;
	ofImage img;//both images stored for Convenience  of the progammer
	cv::Mat mat;
	ofParameter<ofColor>warm;
	ofParameter<int> shrinkby = 3;
	string logDir="logs\\";
	bool readIn = false;
private:
	static void rgb2ryb(unsigned char &r, unsigned char g, unsigned char &b, unsigned char&y);

};


class LiveArt {
public:
	void setup();
	void update();
	void draw();

	bool LiveArt::loadAndFilter(Image& image);
	void setMenu(ofxPanel &gui);
	void echo(vector<ofPolyline>&lines);
	void setTargetColor(const ofColor&c);
	static void toFile(ofFile& resultsfile, vector<std::pair<ofColor, int>>&dat);
	static void toFile(ofFile& resultsfile, vector<ofColor>&dat, bool clear);
	static void fromFile(ofFile& resultsfile, vector<ofColor> &dat);
	void advanceImage();
	void haveBeenNotifiedFloat(float &f);
	void haveBeenNotifiedInt(int &i);
	void haveBeenNotifiedBool(bool &b);
	void haveBeenNotifiedDouble(double &d);
	void redoButtonPressed();
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
	ofxButton redo;
	ofParameter<ofColor>red;
	ofParameter<ofColor>yellow;
	ofParameter<ofColor>blue;

	ofParameter<int> currentImage = 0;
	vector<Image> images;
	ofParameter<int> sortby=0;

	void snapshot(const string& name);
	void restore(){ index = savex; }

private:
	vector<ofColor> savedcolors; //bugbug make unique, right now will store dupes
	int savex = 0;
	int savecount = 0;
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

