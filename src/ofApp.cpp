#include "ofApp.h"
#include <time.h>
#include <algorithm>

void LiveArt::snapshot(const string& name) {
	savex = index;
	index = -1;

	string filename = "logs\\";
	filename += name+"\\";
	filename += ofToString(savecount++);
	filename += ".dat";
	ofFile resultsfile(filename);
	//resultsfile.open(filename, ofFile::WriteOnly);
	toFile(resultsfile, savedcolors, true);
	//resultsfile.close();
	savedcolors.clear();
}
void LiveArt::setTargetColor(const ofColor&c) {
	targetColor = c;
	Image::rgbToryb(targetColor, red, yellow, blue);
}

void Image::rgbToryb(const ofColor& in, ofParameter<ofColor>& red, ofParameter<ofColor>& yellow, ofParameter<ofColor>& blue) {
	unsigned char r = in.r;
	unsigned char b = in.b;
	unsigned char y = 0;

	rgb2ryb(r, in.g, b, y);

	red.set(ofColor(r, 0, 0));
	yellow.set(ofColor(y, y, 0));
	blue.set(ofColor(0, 0, b));
}

//http://www.deathbysoftware.com/colors/index.html
void Image::rgb2ryb(unsigned char &r, unsigned char g, unsigned char &b, unsigned char&y) {
	// Remove the white from the color
	unsigned char iWhite = min(r, min(g, b));

	r -= iWhite;
	g -= iWhite;
	b -= iWhite;

	unsigned char iMaxGreen = max(r, max(g, b));

	// Get the yellow out of the red+green

	y = min(r, g);

	r -= y;
	g -= y;

	// If this unfortunate conversion combines blue and green, then cut each in half to
	// preserve the value's maximum range.
	if (b > 0 && b > 0) {
		b /= 2;
		g /= 2;
	}

	// Redistribute the remaining green.
	y += g;
	b += g;

	// Normalize to values.
	unsigned char iMaxYellow = max(r, max(y, b));

	if (iMaxYellow > 0) {
		unsigned char iN = iMaxGreen / iMaxYellow;

		r *= iN;
		y *= iN;
		b *= iN;
	}

	// Add the white back in.
	r += iWhite;
	y += iWhite;
	b += iWhite;

	r = floor(r);
	y = floor(y);
	b = floor(b);

}

void LiveArt::toFile(ofFile& resultsfile, vector<std::pair<ofColor, int>>&dat) {

	time_t rawtime;
	time(&rawtime);

	resultsfile << ctime(&rawtime) << "\n";

	int i = 1;
	for (auto itr = dat.begin(); itr != dat.end(); ++itr) {
		resultsfile << i << ":" << itr->first << ":" << itr->second << "\n";
		++i;
	}

	resultsfile.close();
}
void LiveArt::toFile(ofFile& resultsfile, vector<ofColor>&dat, bool clear) {

	if (clear) {
		resultsfile.clear();
	}
	ofBuffer buffer;
	buffer.allocate(dat.size());
	for (auto itr = dat.begin(); itr != dat.end(); ++itr) {
		buffer.append(ofToString(itr->getHex()) + "\n");
	}
	resultsfile.writeFromBuffer(buffer);
	resultsfile.close();
}
void LiveArt::fromFile(ofFile& resultsfile, vector<ofColor> &dat) {

	while (resultsfile) {
		ofColor cur;
		resultsfile >> cur;
		dat.push_back(cur);
	}
}

bool colorData::isCool() {
	float h = color.getHue();
	return (h > 80 && h < 330);
}
bool Image::findOrAdd(const ofColor&color, bool add) {

	map<int, colorData>::iterator itr = shapes.find(color.getHex());
	if (itr != shapes.end()) {
		return true;
	}
	else {
		if (add) {
			colorData data;
			data.color = color;
			shapes.insert(make_pair(color.getHex(), data));
			return true;
		}
		return false;
	}
}
bool Image::testForExistance(ofColor color, int i, int j, int k) {
	color.r += i;
	color.g += j;
	color.b += k;
	return findOrAdd(color, false);
}
// return true if color added
bool Image::dedupe(ofColor color, int rangeR, int rangeG, int rangeB) {

	bool found = false;

	for (int i = 0; i < rangeR && !found; ++i) {
		for (int j = 0; j < rangeG && !found; ++j) {
			for (int k = 0; k < rangeB && !found; ++k) {
				if (testForExistance(color, i, j, k)) {
					found = true;
					break;
				}
				if (testForExistance(color, i, j, -k)) {
					found = true;
					break;
				}
				if (testForExistance(color, i, -j, k)) {
					found = true;
					break;
				}
				if (testForExistance(color, i, -j, -k)) {
					found = true;
					break;
				}
				if (testForExistance(color, -i, j, k)) {
					found = true;
					break;
				}
				if (testForExistance(color, -i, j, -k)) {
					found = true;
					break;
				}
				if (testForExistance(color, -i, -j, k)) {
					found = true;
					break;
				}
				if (testForExistance(color, -i, -j, -k)) {
					found = true;
					break;
				}
			}
		}
	}

	if (!found) {
		// color is not in the list
		return findOrAdd(color, true);
	}
	return false;
}
Image::Image(const string& filename) { 
	logDir += filename + string(".data.dat");
	name = ofToDataPath(string("\\images\\") + filename, true); 	
	warm = ofColor::lightYellow;
	shortname = filename;
}

void Image::readColors() {
	ofFile resultsfile(logDir);

	shapes.clear();

	if (resultsfile.exists()) {
		// use it
		ofLog() << "use data" << logDir << endl;
		ofBuffer buffer = ofBufferFromFile(logDir);
		for (auto line : buffer.getLines()) {
			int hex = ofToInt(line);
			if (!hex) {
				continue;
			}
			colorData data; // each line is a hex
			data.color.setHex(ofToInt(line));//bugbug alpha ignored
			shapes.insert(make_pair(data.color.getHex(), data));

			if (!data.isCool() && data.color.getBrightness() > 200) {
				warm = data.color;// go with most recent
			}
		}
	}
	else {
		// create it	
		ofLog() << "create data" << logDir << endl;
		vector<ofColor> dat;
		for (int w = 0; w < img.getWidth(); w += 1) {
			for (int h = 0; h < img.getHeight(); h += 1) {
				colorData data;
				data.color = img.getPixels().getColor(w, h);
				// bugbug ? save all colors in a file so its easier to tweak data later? maybe a different file?

				bool found;
				//if (color.r == 255 && color.g == 255 && color.b == 255) {
				//continue; //ignore white need this to be the back ground color
				//}
				if (!data.isCool() && data.color.getBrightness() > 200) {
					warm = data.color;// go with most recent
				}
				if (data.color.getBrightness() > 255) { // ignore the super bright stuff
					data.color.setBrightness(255); // see what else can be done here
					found = dedupe(data.color, shrinkby, shrinkby, shrinkby);
				}
				else {
					found = dedupe(data.color, shrinkby, shrinkby, shrinkby);
				}
				if (found) {
					dat.push_back(data.color);
				}
				//bugbug make a mid brightness
			}
		}
		resultsfile.open(logDir, ofFile::WriteOnly);
		LiveArt::toFile(resultsfile, dat, true);
	}
}
void LiveArt::haveBeenNotifiedFloat(float &f) {
	ofLog() << " event at " << f << endl;
}
void LiveArt::haveBeenNotifiedInt(int &i) {
	ofLog() << " event at " << i << endl;
}
void LiveArt::haveBeenNotifiedBool(bool &b) {
	ofLog() << " event at " << b << endl;
}
void LiveArt::haveBeenNotifiedDouble(double &d) {
	ofLog() << " event at " << d << endl;
}
void LiveArt::redoButtonPressed() {
	ofLog() << " event at redo " << endl;
	setup();
}

void LiveArt::setMenu(ofxPanel &gui) {

	//consider https://github.com/frauzufall/ofxGuiExtended
	ofxGuiSetTextPadding(4);

	ofParameterGroup realtime;
	realtime.add(targetColor.set("RGB", targetColor, 0.0, 255.0));
	realtime.add(count.set("count", 0));
	realtime.add(index.set("current", -1, 0, count));
	gui.setup(realtime, "setup", 1000, 0);

	ofParameterGroup settings;
	settings.setName("settings");

	settings.add(threshold.set("Threshold", 5, 0.0, 255.0));

	settings.add(minRadius.set("minRadius", 1, 0.0, 255.0));
	settings.add(maxRadius.set("maxRadius", 150, 0.0, 255.0));
	settings.add(findHoles.set("findHoles", true));
	settings.add(smoothingSize.set("smoothingSize", 2, 0.0, 255.0));
	settings.add(smoothingShape.set("smoothingShape", 0.0, 0.0, 255.0));
	settings.add(xImage.set("xImage", 500,20, 4000.0));
	settings.add(yImage.set("yImage", 500,20, 4000.0));
	settings.add(d.set("d", 15, 0.0, 255.0));
	settings.add(sigmaColor.set("sigmaColor", 80, 0.0, 255.0));
	settings.add(sigmaSpace.set("sigmaSpace", 80, 0.0, 255.0));
	settings.add(currentImageName.set("currentImageName"));
	settings.add(sortby.set("sort", 0, 0, 5));
	redo.setup("run");
	gui.add(&redo);
	redo.addListener(this, &LiveArt::redoButtonPressed);

	// all float changes for a reset
	threshold.addListener(this, &LiveArt::haveBeenNotifiedFloat);
	minRadius.addListener(this, &LiveArt::haveBeenNotifiedFloat);
	maxRadius.addListener(this, &LiveArt::haveBeenNotifiedFloat);
	smoothingShape.addListener(this, &LiveArt::haveBeenNotifiedFloat);
	smoothingSize.addListener(this, &LiveArt::haveBeenNotifiedInt);
	d.addListener(this, &LiveArt::haveBeenNotifiedInt);
	findHoles.addListener(this, &LiveArt::haveBeenNotifiedBool);
	sigmaColor.addListener(this, &LiveArt::haveBeenNotifiedDouble);
	sigmaSpace.addListener(this, &LiveArt::haveBeenNotifiedDouble);

	gui.add(settings);
	ofParameterGroup ryb;
	ryb.setName("RYB");
	ryb.add(red.set("r", red, 0.0, 255.0));
	ryb.add(yellow.set("y", yellow, 0.0, 255.0));
	ryb.add(blue.set("b", blue, 0.0, 255.0));
	
	gui.add(ryb);

}
bool LiveArt::loadAndFilter(Image& image) {
	
	image.img.load(image.name);
	if (!image.img.load(image.name)) {
		ofLogError() << image.name << " not loaded";
		return false;
	}
	image.img.resize(xImage, yImage);
	cv::Mat tempMat = toCv(image.img);
	cv::bilateralFilter(tempMat, image.mat, d, sigmaColor, sigmaSpace);
	toOf(image.mat, image.img);
	return true;
}
int LiveArt::getImages() {
	ofDirectory dir("images");
	dir.listDir();
	images.clear();
	for (auto& itr = dir.begin(); itr != dir.end(); ++itr) {
		Image image(itr->getFileName());
		if (!loadAndFilter(image)) {
			ofLogError() << itr->getFileName() << " not loaded";
			continue;
		} 
		images.push_back(image);
	}
	return images.size();
}
void LiveArt::setup() {
	ofLog() << "setup" << endl;
	SetCursor(LoadCursor(NULL, IDC_WAIT)); // the sin of windows
	index = 0;

	if (!getImages()) {
		ofLogFatalError() << "no images in data\\images directory";
		return;
	}

	ContourFinder finder;
	finder.setMinAreaRadius(minRadius);
	finder.setMaxAreaRadius(maxRadius);
	finder.setSimplify(true);
	finder.setAutoThreshold(false);
	finder.setThreshold(threshold);
	finder.setUseTargetColor(true);
	finder.setFindHoles(findHoles);// matters

	// read in all the images the user may want to see
	for (auto& itr = images.begin(); itr != images.end(); ++itr) {
		itr->readColors();
		itr->drawingData.clear();
		// less colors, do not draw on top of each other, find holes
		for (auto& itr2 = itr->shapes.begin(); itr2 != itr->shapes.end(); ++itr2) {
			// bugbug install backup software
			// put all results in a vector of PolyLines, then sort by size, then draw, save polylines in a file
			finder.setTargetColor(itr2->second.color, TRACK_COLOR_RGB);
			finder.findContours(itr->img);
			if (finder.getPolylines().size() > 1) {
				itr2->second.lines = finder.getPolylines();
				itr2->second.threshold = threshold;
				itr->drawingData.push_back(itr2->second); // shadow vector for good sorting
			}
		}
		sort(itr->drawingData.begin(), itr->drawingData.end(), [=](colorData&  a, colorData&  b) {
			switch (sortby) {
			case 0:
				return a.color.getSaturation() > a.color.getSaturation();//bugbug this bug works!!
			case 1:
				return a.lines.size() > b.lines.size();
			case 2:
				if (a.color.getSaturation() == a.color.getSaturation()) {
					return a.lines.size() > b.lines.size();
				}
				return  a.color.getSaturation() > a.color.getSaturation();
			case 3:
				if (a.lines.size() == a.lines.size()) {
					return a.color.getSaturation() > b.color.getSaturation();
				}
				return  a.lines.size() > a.lines.size();
			case 4:
				return a.color.getSaturation() > b.color.getSaturation();
			default:
				return a.color.getSaturation() > b.color.getSaturation();
			}
			//bugbug need to add in sort by size, count, saturation, brightness, object size etc
		});
	}


	// bugbug drop light and dark, use "if light then set lightthresh hold" cleans it all up
	// bugbug support warm, cools?

}

void LiveArt::update() {
	count = images[currentImage].shapes.size();
}
void LiveArt::draw() {

	ofSetBackgroundColor(ofColor::white);
	images[currentImage].img.draw(0, 0);// test with 2000,2000 image

	ofSetColor(ofColor::white);
	ofSetBackgroundColor(images[currentImage].warm);//bugbug use lightest found color

	ofSetLineWidth(1);
	
	//ofTranslate(300, 0); keep as a reminder

	int s = images[currentImage].drawingData.size();

	if (index > -1 && images[currentImage].drawingData.size() > 0) {
		if (index < images[currentImage].drawingData.size()) {
			currentImageName = images[currentImage].shortname;
			ofPushStyle();
			// less colors, do not draw on top of each other, find holes
			setTargetColor(images[currentImage].drawingData[index].color);
			ofSetColor(targetColor); // varibles here include only show large, or smalll, to create different pictures
			savedcolors.push_back(targetColor);
			echo(images[currentImage].drawingData[index].lines);
			ofPopStyle();
		}
		++index;
		if (index >= images[currentImage].drawingData.size()) {
			index = -1; // stop
		}
	}
}
void LiveArt::echo(vector<ofPolyline>&lines) {

	// use this? fillPoly in wrappers.h

	ofTranslate(xImage, 0);
	for (auto& itr = lines.begin(); itr != lines.end(); ++itr) {
		ofPolyline line = itr->getSmoothed(smoothingSize); //bugbug test this data
		// cv::Mat dest fillPoly(line, dest); draw dest
		ofTessellator tess;
		ofMesh mesh;
		tess.tessellateToMesh(line, OF_POLY_WINDING_ODD, mesh, true);
		mesh.draw();
		line.draw();
	}
}

void ofApp::setup() {
	art.setMenu(gui);

	cam.setup(640, 480);

	ofSetFrameRate(120);
	
	art.setup();

	ofSetBackgroundAuto(false);
}// 45shavlik11
 //http://www.creativeapplications.net/tutorials/arduino-servo-opencv-tutorial-openframeworks/
 //http://www.autobotic.com.my/ds3218-servo-control-angle-180-degree-25t-servo-armv
void ofApp::update() {
	cam.update();
	art.update();

	//convertColor(cam, gray, CV_RGB2GRAY);
	//GaussianBlur(gray, gray, gray.getWidth());
	// Canny(gray, edge, mouseX, mouseY, 3);
	//Sobel(gray, sobel);

	// gray.update();
	// sobel.update();
	//edge.update();



}
void ofApp::draw() {
	gui.draw();
	art.draw();

	//ofFill();
	//cam.draw(0, 0);
	//gray.draw(0, 480);(
	//edge.draw(640, 0);
	//sobel.draw(640, 480);

}
void ofApp::keyPressed(int key) {
	if (key == ' ') {
		if (art.index == -1) {
			art.restore();
		}
		else {
			art.snapshot(art.images[art.currentImage].name);
		}
	}
	else if (key == 'i') {
		art.currentImage++;
		if (art.currentImage >= art.images.size()) {
			art.currentImage = 0;
		}
		art.index = 0;
	}
	else if (key == 'r') {
		art.setup();
	}
	else if (key == 's') {
		art.index = 0; // go from start
	}
	else if (key == 'b') {
		art.index -= 20; // hit b a bunch of times to get back to the start
		if (art.index < 0) {
			art.index = 0;
		}
	}
}
void ofApp::mousePressed(int x, int y, int button) {

	if (x < art.images[art.currentImage].img.getWidth() && y < art.images[art.currentImage].img.getHeight()) {
		art.setTargetColor(art.images[art.currentImage].img.getColor(x, y)); //bugbug make it scan just for this in the future
	}

}