#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>

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

private:
};
typedef map<int, colorData> Shapes;
class myContourFinder : public ContourFinder {
public:
};
class Image {
public:
	Image(string& filename);
	class MyThread : public ofThread {
	public:
		myContourFinder finder;
		shared_ptr<Image>image;
		colorData get();
		queue<colorData> tracedata;
	private:
		void threadedFunction();
	};
	int index = 0;
	int hits = 0;
	bool findOrAdd(const ofColor&color, bool add);
	bool testForExistance(ofColor color, int i, int j, int k);// test for existance
	bool dedupe(ofColor color, int rangeR, int rangeG, int rangeB);
	void readColors();
	static void rgbToryb(const ofColor& in, ofParameter<ofColor>& red, ofParameter<ofColor>& yellow, ofParameter<ofColor>& blue);
	void filter(int id, ofParameter<int> a, ofParameter<double> b, ofParameter<double> c);
	string name;
	string shortname;
	vector<colorData> drawingData;
	vector<colorData> ignoredData;
	int lineThreshHold = 0;
	ofImage img;//both images stored for Convenience  of the progammer
	cv::Mat mat;
	ofParameter<ofColor>warm;
	ofParameter<int> shrinkby = 3;
	string logDir="logs\\";
	bool readIn = false;
	MyThread mythread;
	ofParameter<int>sortby=0;
	ofParameter<float> threshold;
	int allcolors = 0;
private:
	static void rgb2ryb(unsigned char &r, unsigned char g, unsigned char &b, unsigned char&y);
	Shapes shapes; // hash for quick lookup

};


class LiveArt {
public:
	void setup();
	void update();
	void draw();

	bool LiveArt::loadAndFilter(shared_ptr<Image> image);
	void setMenu(ofxPanel &gui);
	void echo(const vector<ofPolyline>&lines);
	void setTargetColor(const ofColor&c);
	static void toFile(ofFile& resultsfile, vector<std::pair<ofColor, int>>&dat);
	static void toFile(ofFile& resultsfile, vector<colorData> &dat, bool clear);
	static void toFileHumanForm(ofFile& resultsfile, vector<colorData> &dat, bool clear);
	static void fromFile(ofFile& resultsfile, vector<ofColor> &dat);
	void advanceImage();
	void haveBeenNotifiedFloat(float &f);
	void haveBeenNotifiedInt(int &i);
	void haveBeenNotifiedBool(bool &b);
	void haveBeenNotifiedDouble(double &d);
	void redoButtonPressed();
	void toscreen(const colorData& data);
	// read from xml file, 'r' key will refresh data? 

	ofParameter<float> minRadius;
	ofParameter<float> maxRadius;
	ofParameter<bool> findHoles;
	ofParameter<int> smoothingSize;//learn
	ofParameter<int> allColors;
	ofParameter<float> smoothingShape;//bugbug learn
	ofParameter<int> xImage;// make bigger when presenting, smaller when getting colors
	ofParameter<int> yImage;
	ofParameter<int> d; //bugbug learn for bilateralfilter and enable other filters
	ofParameter<double> sigmaColor; //bugbug learn for bilateralfilter
	ofParameter<double> sigmaSpace; //bugbug learn for bilateralfilter
	ofParameter<string>currentImageName;
	ofParameter<float> threshold;
	ofParameter<int> count;
	ofParameter<ofColor>targetColor;
	ofxButton redo;
	ofxButton cancel;
	ofParameter<ofColor>red;
	ofParameter<ofColor>yellow;
	ofParameter<ofColor>blue;

	ofParameter<int> currentImage = 0;
	vector<shared_ptr<Image>> images;
	ofParameter<int> sortby=0;

	void snapshot(const string& name);

private:
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

