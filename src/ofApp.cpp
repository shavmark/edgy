#include "ofApp.h"
#include <time.h>
#include <algorithm>

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

void LiveArt::toFile(ofFile& resultsfile, vector<shared_ptr<colorData>>dat, bool clear) {

	if (clear) {
		resultsfile.clear();
	}
	ofBuffer buffer;
	buffer.allocate(dat.size());
	for (auto& itr = dat.begin(); itr != dat.end(); ++itr) {
		buffer.append(ofToString((*itr)->getTargetColor().getHex()) + "\n");
	}
	resultsfile.writeFromBuffer(buffer);
	resultsfile.close();
}
void LiveArt::toFileHumanForm(ofFile& resultsfile, vector<shared_ptr<colorData> >dat, bool clear) {

	if (clear) {
		resultsfile.clear();
	}
	ofBuffer buffer;
	buffer.allocate(dat.size());
	for (auto itr = dat.begin(); itr != dat.end(); ++itr) {
		buffer.append(ofToString((int)(*itr)->getTargetColor().r) + ofToString(",") + ofToString((int)(*itr)->getTargetColor().g) + ofToString(",") + ofToString((int)(*itr)->getTargetColor().b) + ofToString("\n"));
	}
	resultsfile.writeFromBuffer(buffer);
	resultsfile.close();
}

bool colorData::isCool() {
	float h = getTargetColor().getHue();
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
			data.setTargetColor(color);
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
	drawingData.clear();
	if (resultsfile.exists()) {
		// using a file, hash not needed as all data is just loaded in
		ofLog() << "use data" << logDir << endl;
		ofBuffer buffer = ofBufferFromFile(logDir);
		for (auto line : buffer.getLines()) {
			int hex = ofToInt(line);
			if (!hex) {
				continue;
			}
			shared_ptr<colorData> data= make_shared<colorData>(); 
			if (data) {
				data->getTargetColor().setHex(ofToInt(line));//bugbug alpha ignored
				data->setThreshold(threshold);
				drawingData.push_back(data);// will be sorted later in this function //bugbug go to shared pointer?
											//(0.299*R + 0.587*G + 0.114*B)
				if (warm.get().getBrightness() < data->getTargetColor().getBrightness() && !data->isCool()) {
					warm = data->getTargetColor();// go with most recent
				}
			}
		}
	}
	else {
		// create it	
		ofLog() << "create data" << logDir << endl;
		vector<ofColor> dat;
		for (int w = 0; w < img.getWidth(); w += 1) {
			for (int h = 0; h < img.getHeight(); h += 1) {
				shared_ptr<colorData> data = make_shared<colorData>();
				if (data == nullptr) {
					return; // we are in a bad place
				}
				data->setTargetColor(img.getPixels().getColor(w, h));
				// bugbug ? save all colors in a file so its easier to tweak data later? maybe a different file?

				bool found;
				//if (color.r == 255 && color.g == 255 && color.b == 255) {
				//continue; //ignore white need this to be the back ground color
				//}
				if (warm.get().getBrightness() < data->getTargetColor().getBrightness()) {
					warm = data->getTargetColor();// go with most recent
				}
				found = dedupe(data->getTargetColor(), shrinkby, shrinkby, shrinkby);
				if (found) {
					data->setThreshold(threshold);
					drawingData.push_back(data);
				}
				//bugbug make a mid brightness
			}
		}
		resultsfile.open(logDir, ofFile::WriteOnly);
		LiveArt::toFile(resultsfile, drawingData, true);
	}

	allcolors = drawingData.size(); // save size before empty items are removed

	sort(drawingData.begin(), drawingData.end(), [=](shared_ptr<colorData>  a, shared_ptr<colorData>  b) {
		switch (sortby) {
		case 0:
			if (a->getTargetColor().getSaturation() == b->getTargetColor().getSaturation()) {
				return a->getTargetColor().getLightness() < b->getTargetColor().getLightness();
			}
		}
		return a->getTargetColor().getSaturation() < b->getTargetColor().getSaturation();
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
	realtime.add(allColors.set("all colors", 0, 0, 100000));
	
	realtime.add(count.set("used count", 0));
	gui.setup(realtime, "setup", 700 *2, 0);

	ofParameterGroup settings;
	settings.setName("settings");

	settings.add(threshold.set("Threshold", 10, 1.0, 255.0)); // 8 works great, 5 not as much, 10?

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
shared_ptr<colorData> Image::MyThread::get() {
	shared_ptr<colorData> data = nullptr;
	lock();
	if (!tracedata.empty()) {
		int i = tracedata.front();
		data = image->drawingData[i];
		tracedata.pop();
	}
	unlock();
	return data;
}
bool myContourFinder::findContours(Mat img) {
	// threshold the image using a tracked color or just binary grayscale
	if (useTargetColor) {
		Scalar offset(thresholdValue, thresholdValue, thresholdValue);
		Scalar base = toCv(targetColor);
		if (trackingColorMode == TRACK_COLOR_RGB) {
			inRange(img, base - offset, base + offset, thresh);
		}
	}
	else {
		copyGray(img, thresh);
	}

	// run the contour finder
	vector<vector<cv::Point> > allContours;
	std::vector<std::vector<cv::Point> > contours;
	vector<size_t> allIndices;
	cv::findContours(thresh, allContours, CV_CHAIN_APPROX_SIMPLE, true);

	for (size_t i = 0; i < allContours.size(); i++) {
		allIndices.push_back(i);
	}

	// generate polylines from the contours
	std::vector<ofPolyline> polylines;
	for (size_t i = 0; i < allIndices.size(); i++) {
		contours.push_back(allContours[allIndices[i]]);
		polylines.push_back(toOf(contours[i]).getSmoothed(2));//getSmoothed bugbug study this
	}
	if (polylines.size() > 0) {
		ofTessellator tess;
		tess.tessellateToMesh(polylines, OF_POLY_WINDING_ODD, mesh, true);
	}
	return polylines.size() > 0;

}
void Image::MyThread::threadedFunction() {

	if (image) {
		setDone(false);
		ofLog() << image->name << "start" << endl;
		image->readColors(); // bugbug read all in, in the future only read in what is shown
		image->readIn = true;
		image->ignoredData.clear();
		image->hits = 0;
		//   contourFinder.setMinAreaNorm(ofMap(mouseY, 0, ofGetHeight(), 0.0, 1.0));
		// less colors, do not draw on top of each other, find holes
		for (int i = 0; i <  image->drawingData.size(); ++i) {
			// put all results in a vector of PolyLines, then sort by size, then draw, save polylines in a file
			if (image->drawingData[i]->findContours(image->mat)) {
				lock();
				tracedata.push(i);
				unlock();
				++image->hits;
			}
			else {
				image->ignoredData.push_back(image->drawingData[i]);
				image->drawingData.erase(image->drawingData.begin() + i); // assume past indexes remain correct after delete
			}
		}
		// only do this once somethings is found
		if (image->hits > 0) {
			ofFile resultsfile; // save in a file, too much data show -- but the data is key as it shows what is ignored per given rules
			sort(image->ignoredData.begin(), image->ignoredData.end(), [=](shared_ptr<colorData>  a, shared_ptr<colorData> b) {
				if (a->getTargetColor().r == b->getTargetColor().r) {
					if (a->getTargetColor().g == b->getTargetColor().g) {
						return a->getTargetColor().b > b->getTargetColor().b;
					}
					return a->getTargetColor().g > b->getTargetColor().g;
				}
				return a->getTargetColor().r > b->getTargetColor().r;

			});
			resultsfile.open(image->logDir + ofToString(".notused.dat"), ofFile::WriteOnly);
			LiveArt::toFileHumanForm(resultsfile, image->ignoredData, true);
		}
		setDone();
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
		images[i]->mythread.setDone(false);
		images[i]->index = -1; // no need to show data until the user asks for a scan again
		//images[i]->mythread.finder.setMinAreaRadius(minRadius);
		//images[i]->mythread.finder.setMaxAreaRadius(maxRadius);
		images[i]->sortby = sortby;
		images[i]->threshold = threshold;
		images[i]->mythread.image = images[i];
		images[i]->mythread.startThread();
	}


	// bugbug drop light and dark, use "if light then set lightthresh hold" cleans it all up
	// bugbug support warm, cools?

}

void LiveArt::update() {
	// remove unused data
	if (images[currentImage]->mythread.isDone()) {
		count = images[currentImage]->drawingData.size();
	}
	else {
		count = images[currentImage]->hits;
	}
	currentImageName = images[currentImage]->shortname;
	allColors = images[currentImage]->allcolors;
}
// true if draw occured
bool LiveArt::toscreen(colorData& data) {
	ofSetBackgroundColor(images[currentImage]->warm);
	setTargetColor(data.getTargetColor());
	ofSetColor(data.getTargetColor());
	echo(data.mesh);
	return true;
}
// true if draw occured
bool LiveArt::toscreen(shared_ptr<colorData> data) {
	if (data) {
		return toscreen(*data);
	}
	return false;
}
void LiveArt::draw() {
	ofPushStyle();
	ofSetColor(ofColor::white);
	images[currentImage]->img.draw(0, 0);

	ofSetLineWidth(1);

	if (!images[currentImage]->mythread.isDone()) {
		toscreen(images[currentImage]->mythread.get());
	}
	else {
		// clean out any remaining data
		do {
			if (!toscreen(images[currentImage]->mythread.get())) {
				break;
			}
		} while (1);
	}

	if (images[currentImage]->index < 0) {
		count = -1;
		ofPopStyle();
		return;
	}

	//ofTranslate(300, 0); keep as a reminder

	// thread is building data so only show current item
	
	// show one item at a time

	int i = images[currentImage]->index;// for debug
	int s = images[currentImage]->drawingData.size();
	if (images[currentImage]->index >= images[currentImage]->drawingData.size()) {
		images[currentImage]->index = -1; // stop
	}
	else if (images[currentImage]->index < images[currentImage]->drawingData.size()) {
		toscreen(images[currentImage]->drawingData[images[currentImage]->index]);
		++images[currentImage]->index;
	}
	ofPopStyle();
}
void LiveArt::echo(ofMesh&mesh) {

	// use this? fillPoly in wrappers.h

	ofTranslate(xImage, 0);
	//for (auto& itr = lines.begin(); itr != lines.end(); ++itr) {
		// cv::Mat dest fillPoly(line, dest); draw dest
		//ofTessellator tess;
		//ofMesh mesh;
		//tess.tessellateToMesh(lines, OF_POLY_WINDING_ODD, mesh, true);
		mesh.draw();
		//line.draw();
	//}
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
		name += ofToString("save.")+art.images[art.currentImage]->shortname;
		name += ".png";
		ofImage img;
		img.grabScreen(art.xImage, 0, art.xImage, art.yImage);
		img.save(name);
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