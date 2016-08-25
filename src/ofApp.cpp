#include "ofApp.h"
#include <time.h>
#include <algorithm>

//bugbug remove this
void LiveArt::snapshot(const string& name) {
	string filename = "logs\\";
	filename += name+"\\";
	filename += ofToString(savecount++);
	filename += ".dat";
	ofFile resultsfile(filename);
	//resultsfile.close();
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
void LiveArt::toFile(ofFile& resultsfile, vector<colorData>&dat, bool clear) {

	if (clear) {
		resultsfile.clear();
	}
	ofBuffer buffer;
	buffer.allocate(dat.size());
	for (auto itr = dat.begin(); itr != dat.end(); ++itr) {
		buffer.append(ofToString(itr->color.getHex()) + "\n");
	}
	resultsfile.writeFromBuffer(buffer);
	resultsfile.close();
}
void LiveArt::toFileHumanForm(ofFile& resultsfile, vector<colorData>&dat, bool clear) {

	if (clear) {
		resultsfile.clear();
	}
	ofBuffer buffer;
	buffer.allocate(dat.size());
	for (auto itr = dat.begin(); itr != dat.end(); ++itr) {
		buffer.append(ofToString((int)itr->color.r) + ofToString(",") + ofToString((int)itr->color.g) + ofToString(",") + ofToString((int)itr->color.b) + ofToString("\n"));
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
Image::Image(string& filename) { 
	logDir += filename + string(".data.dat");
	name = ofToDataPath(string("\\images\\") + filename, true); 	
	warm = ofColor::black;
	shortname = filename;
}

void Image::readColors() {
	ofFile resultsfile(logDir);

	if (resultsfile.exists()) {
		// using a file, hash not needed as all data is just loaded in
		ofLog() << "use data" << logDir << endl;
		ofBuffer buffer = ofBufferFromFile(logDir);
		for (auto line : buffer.getLines()) {
			int hex = ofToInt(line);
			if (!hex) {
				continue;
			}
			colorData data; // each line is a hex
			data.color.setHex(ofToInt(line));//bugbug alpha ignored
			drawingData.push_back(data);// will be sorted later in this function //bugbug go to shared pointer?
			//(0.299*R + 0.587*G + 0.114*B)
			if (warm.get().getBrightness() < data.color.getBrightness() && !data.isCool()) {
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
				if (warm.get().getBrightness() < data.color.getBrightness()) {
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
					drawingData.push_back(data);
				}
				//bugbug make a mid brightness
			}
		}
		resultsfile.open(logDir, ofFile::WriteOnly);
		LiveArt::toFile(resultsfile, drawingData, true);
	}
	sort(drawingData.begin(), drawingData.end(), [=](colorData&  a, colorData&  b) {
		switch (sortby) {
		case 0:
			if (a.color.getSaturation() == b.color.getSaturation()) {
				return a.color.getSaturation() < b.color.getSaturation();
			}
			return a.color.getBrightness() < b.color.getBrightness();
		case 1:
			return a.lines.size() > b.lines.size();
		case 2:
			if (a.color.getSaturation() == a.color.getSaturation()) {
				return a.lines.size() > b.lines.size();
			}
			return  a.color.getSaturation() > b.color.getSaturation();
		case 3:
			if (a.lines.size() == b.lines.size()) {
				return a.color.getSaturation() > b.color.getSaturation();
			}
			return  a.lines.size() > a.lines.size();
		case 4:
			return a.color.getSaturation() > b.color.getSaturation();
		default:
			return a.color.getSaturation() < b.color.getSaturation();
		}
		//bugbug need to add in sort by size, count, saturation, brightness, object size etc
	});

}
void LiveArt::haveBeenNotifiedFloat(float &f) {
	images[currentImage]->readIn = false;// get more data
}
void LiveArt::haveBeenNotifiedInt(int &i) {
	images[currentImage]->readIn = false;// get more data
}
void LiveArt::haveBeenNotifiedBool(bool &b) {
	ofLog() << " event at " << b << endl;
}
void LiveArt::haveBeenNotifiedDouble(double &d) {
	images[currentImage]->readIn = false;// get more data
}
void LiveArt::redoButtonPressed() {
	ofLog() << " event at redo " << endl;
	images[currentImage]->index = 0;
	setup();
}

void LiveArt::setMenu(ofxPanel &gui) {

	//consider https://github.com/frauzufall/ofxGuiExtended
	ofxGuiSetTextPadding(4);

	ofParameterGroup realtime;
	realtime.add(targetColor.set("RGB", targetColor, 0.0, 255.0));
	realtime.add(allColors.set("all colors", 0));
	
	realtime.add(count.set("used count", 0));
	gui.setup(realtime, "setup", 700 *2, 0);

	ofParameterGroup settings;
	settings.setName("settings");

	settings.add(threshold.set("Threshold", 5, 1.0, 255.0));

	settings.add(minRadius.set("minRadius", 1, 0.0, 255.0));
	settings.add(maxRadius.set("maxRadius", 150, 0.0, 255.0));
	settings.add(findHoles.set("findHoles", true));
	settings.add(smoothingSize.set("smoothingSize", 2, 0.0, 255.0));
	settings.add(smoothingShape.set("smoothingShape", 0.0, 0.0, 255.0));
	settings.add(xImage.set("xImage", 700,20, 4000.0));
	settings.add(yImage.set("yImage", 700,20, 4000.0));
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
void Image::filter(int id, ofParameter<int> a, ofParameter<double> b, ofParameter<double> c){
#define MAX_KERNEL_LENGTH 31

	//http://docs.opencv.org/2.4/doc/tutorials/imgproc/gausian_median_blur_bilateral_filter/gausian_median_blur_bilateral_filter.html
	//bugbug get the canny ones here too
	Mat src = toCv(img);
	mat = src.clone();
	switch (id) {
	case 0:
		//cv::bilateralFilter(toCv(img), mat, a, b, c);
		//toOf(mat, img); // keep both around, use where it makes the most sense
		for (int i = 0; i < 2; ++i)	{
			bilateralFilter(src, mat, a.get(), b.get(), c.get());
		}
		toOf(mat, img);
		break;
	case 1:
		for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2) {
			GaussianBlur(src, mat, Size(i, i), 0, 0);
		}
		toOf(mat, img);
		break;
	}
}
bool LiveArt::loadAndFilter(shared_ptr<Image>image) {
	
	if (image) {
		if (image->img.load(image->name)) {
			image->img.resize(xImage, yImage);
			image->filter(0, d, sigmaColor, sigmaSpace);
		}
		return true;
	}
	ofLogError() << image->name << " not loaded";
	return false;
}
int LiveArt::getImages() {
	ofDirectory dir("images");
	dir.listDir();
	images.clear();
	for (auto& itr = dir.begin(); itr != dir.end(); ++itr) {
		shared_ptr<Image> image = make_shared<Image>(itr->getFileName());
		if (image == nullptr || !loadAndFilter(image)) {
			ofLogError() << itr->getFileName() << " not loaded";
			continue;
		} 
		images.push_back(image);
	}
	if (images.size() > 0) {
		currentImageName = images[0]->shortname;
	}

	return images.size();
}
colorData Image::MyThread::get() {
	colorData data;
	lock();
	if (!tracedata.empty()) {
		data = tracedata.front();
		tracedata.pop();
	}
	unlock();
	return data;
}
void Image::MyThread::threadedFunction() {

	if (image && !image->readIn) {
		ofLog() << image->name << "start" << endl;
		image->readColors(); // bugbug read all in, in the future only read in what is shown
		image->readIn = true;
		image->ignoredData.clear();
		if (image->allcolors == 0) {
			image->allcolors = image->drawingData.size(); // save size before empty items are removed
		}
		// less colors, do not draw on top of each other, find holes
		for (auto& itr = image->drawingData.begin(); itr != image->drawingData.end(); ){
			// put all results in a vector of PolyLines, then sort by size, then draw, save polylines in a file
			finder.setTargetColor(itr->color, TRACK_COLOR_RGB);
			finder.findContours(image->mat);
			if (finder.getPolylines().size() > 0) {
				itr->lines = finder.getPolylines();
				itr->threshold = image->threshold;
				lock();
				tracedata.push(*itr);
				unlock();
				++image->hits;
				
			}
			++itr;
		}
	}
}
void LiveArt::setup() {
	ofLog() << "setup" << endl;

	if (!readIn) {
		if (!getImages()) {
			ofLogFatalError() << "no images in data\\images directory";
			return;
		}
		readIn = true;
	}
	// find the current image and start its thread to compile contours
	for (int i = 0; i < images.size(); ++i) {
		// if current thread is already running, or the current image is not this one, skip this scan
		if (images[i]->mythread.isThreadRunning() || images[i]->shortname != currentImageName.get()) {
			ofLog() << "ignore " << images[i]->name << endl;
			continue;
		}
		images[i]->mythread.finder.setMinAreaRadius(minRadius);
		images[i]->mythread.finder.setMaxAreaRadius(maxRadius);
		images[i]->mythread.finder.setSimplify(true);
		images[i]->mythread.finder.setAutoThreshold(false);
		images[i]->mythread.finder.setThreshold(threshold);
		images[i]->mythread.finder.setUseTargetColor(true);
		images[i]->mythread.finder.setFindHoles(findHoles);// matters
		images[i]->threshold = threshold;
		images[i]->sortby = sortby;
		images[i]->mythread.image = images[i];
		images[i]->mythread.startThread();
	}


	// bugbug drop light and dark, use "if light then set lightthresh hold" cleans it all up
	// bugbug support warm, cools?

}

void LiveArt::update() {
	// remove unused data
	if (!images[currentImage]->mythread.isThreadRunning()) {
		// only do this once somethings is found
		if (images[currentImage]->hits > 0) {
			for (auto& itr = images[currentImage]->drawingData.begin(); itr != images[currentImage]->drawingData.end(); ) {
				// put all results in a vector of PolyLines, then sort by size, then draw, save polylines in a file
				if (itr->lines.size() == 0) {
					// remove unused items
					images[currentImage]->ignoredData.push_back(*itr);
					itr = images[currentImage]->drawingData.erase(itr);
				}
				else {
					++itr;
				}
			}
			ofFile resultsfile; // save in a file, too much data show -- but the data is key as it shows what is ignored per given rules
			sort(images[currentImage]->ignoredData.begin(), images[currentImage]->ignoredData.end(), [=](colorData&  a, colorData&  b) {
				if (a.color.r == b.color.r) {
					if (a.color.g == b.color.g) {
						return a.color.b > b.color.b;
					}
					return a.color.g > b.color.g;
				}
				return a.color.r > b.color.r;

			});
			resultsfile.open(images[currentImage]->logDir + ofToString(".notused.dat"), ofFile::WriteOnly);
			LiveArt::toFileHumanForm(resultsfile, images[currentImage]->ignoredData, true);
			images[currentImage]->hits = 0; // data no longer fresh
			images[currentImage]->index = -1;
		}
	}
	else {
		count = images[currentImage]->hits;
	}
}
void LiveArt::draw() {

	if (images[currentImage]->index < 0) {
		return;
	}


	//ofTranslate(300, 0); keep as a reminder

	// thread is building data so only show current item
	
	// show one item at a time
	ofPushStyle();
	ofSetColor(ofColor::white);
	images[currentImage]->img.draw(0, 0);
	ofSetLineWidth(1);
	int i = images[currentImage]->index;// for debug
	if (images[currentImage]->mythread.isThreadRunning()) {
		colorData data = images[currentImage]->mythread.get();
		if (data.lines.size() > 0) {
			// thread that popuplates this data will delete unused items so the list is always changing
			// just echo the first one found to show all data
			ofColor c = images[currentImage]->warm;
			ofSetBackgroundColor(images[currentImage]->warm);
			setTargetColor(data.color);
			ofSetColor(data.color);
			echo(data.lines);
		}
	}
	else {
		// clean out any remaining data
		do {
			colorData data = images[currentImage]->mythread.get();
			if (data.lines.size() > 0) {
				// thread that popuplates this data will delete unused items so the list is always changing
				// just echo the first one found to show all data
				ofSetBackgroundColor(images[currentImage]->warm);
				setTargetColor(data.color);
				ofSetColor(data.color);
				echo(data.lines);
			}
			else {
				break;
			}
		} while (1);

		currentImageName = images[currentImage]->shortname;
		int i = images[currentImage]->index;
		int s = images[currentImage]->drawingData.size();
		if (images[currentImage]->index >= images[currentImage]->drawingData.size()) {
			images[currentImage]->index = -1; // stop
		}
		else if (images[currentImage]->index < images[currentImage]->drawingData.size()) {
			ofSetBackgroundColor(images[currentImage]->warm);//bugbug use lightest found color
																// less colors, do not draw on top of each other, find holes
			setTargetColor(images[currentImage]->drawingData[images[currentImage]->index].color);
			ofSetColor(targetColor); // varibles here include only show large, or smalll, to create different pictures
			echo(images[currentImage]->drawingData[images[currentImage]->index].lines);
			++images[currentImage]->index;
		}
		count = images[currentImage]->index;
	}
	allColors = images[currentImage]->allcolors;
	ofPopStyle();
}
void LiveArt::echo(vector<ofPolyline>&lines) {

	if (lines.size() == 0)
		return;

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
void LiveArt::advanceImage()
{
	currentImage++;
	if (currentImage >= images.size()) {
		currentImage = 0;
	}
	currentImageName = images[currentImage]->shortname;
	if (images[currentImage]->index < 0) {
		images[currentImage]->index = 0; // start again if stopped?
	}
	setup();
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
		// put a start/stop here
	}
	else if (key == 'a') {
		art.advanceImage();
	}
	else if (key == 'r') {
		art.setup();
	}
	else if (key == 's') {
		art.images[art.currentImage]->index = 0; // go from start
	}
	else if (key == 'x') {
		string name = "save\\";
		name += art.images[art.currentImage]->shortname;
		art.images[art.currentImage]->img.save(name);
	}
	else if (key == 'b') {
		art.images[art.currentImage]->index -= 20; // hit b a bunch of times to get back to the start
		if (art.images[art.currentImage]->index < 0) {
			art.images[art.currentImage]->index = 0;
		}
	}
}
void ofApp::mousePressed(int x, int y, int button) {

	if (x < art.images[art.currentImage]->img.getWidth() && y < art.images[art.currentImage]->img.getHeight()) {
		art.setTargetColor(art.images[art.currentImage]->img.getColor(x, y)); //bugbug make it scan just for this in the future
	}

}