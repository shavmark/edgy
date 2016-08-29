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

void LiveArt::toFile(ofFile& resultsfile, vector<shared_ptr<ofColor>>&dat, bool clear) {

	if (clear) {
		resultsfile.clear();
	}
	ofBuffer buffer;
	buffer.allocate(dat.size());
	for (auto itr = dat.begin(); itr != dat.end(); ++itr) {
		buffer.append(ofToString(itr->get()->getHex()) + "\n");
	}
	resultsfile.writeFromBuffer(buffer);
	resultsfile.close();
}
void LiveArt::toFileHumanForm(ofFile& resultsfile, vector<shared_ptr<ofColor>>&dat, bool clear) {

	if (clear) {
		resultsfile.clear();
	}
	ofBuffer buffer;
	buffer.allocate(dat.size());
	for (auto itr = dat.begin(); itr != dat.end(); ++itr) {
		buffer.append(ofToString((int)itr->get()->r) + ofToString(",") + ofToString((int)itr->get()->g) + ofToString(",") + ofToString((int)itr->get()->b) + ofToString("\n"));
	}
	resultsfile.writeFromBuffer(buffer);
	resultsfile.close();
}

bool isCool(const ofColor&c) {
	float h = c.getHue();
	return (h > 80 && h < 330);
}
bool Image::findOrAdd(const ofColor&color, bool add) {

	unordered_set<int>::iterator itr = shapes.find(color.getHex());
	if (itr != shapes.end()) {
		return true;
	}
	else {
		if (add) {
			shapes.insert(color.getHex());
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
	colorsList.clear();

	if (resultsfile.exists()) {
		// using a file, hash not needed as all data is just loaded in
		ofLog() << "use data" << logDir << endl;
		ofBuffer buffer = ofBufferFromFile(logDir);
		for (auto line : buffer.getLines()) {
			int hex = ofToInt(line);
			if (!hex) {
				continue;
			}
			shared_ptr<ofColor> color= make_shared<ofColor>();
			if (color) {
				color->setHex(hex);//bugbug alpha ignored
				colorsList.push_back(color);// will be sorted later in this function //bugbug go to shared pointer?
											//(0.299*R + 0.587*G + 0.114*B)
				if (warm.get().getBrightness() < color->getBrightness() && !isCool(*color)) {
					warm = *color;// go with most recent
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
				shared_ptr<ofColor> color = make_shared<ofColor>();
				if (color == nullptr) {
					return; // we are in a bad place
				}
				*color = img.getPixels().getColor(w, h);
				// bugbug ? save all colors in a file so its easier to tweak data later? maybe a different file?

				if (warm.get().getBrightness() < color->getBrightness()) {
					warm = *color;// go with most recent
				}
				if (dedupe(*color, shrinkby, shrinkby, shrinkby)) {
					colorsList.push_back(color);
				}
				//bugbug make a mid brightness
			}
		}
		resultsfile.open(logDir, ofFile::WriteOnly);
		LiveArt::toFile(resultsfile, colorsList, true);
	}

	sort(colorsList.begin(), colorsList.end(), [=](shared_ptr<ofColor>  a, shared_ptr<ofColor>  b) {
		return a->getLightness() < b->getLightness();
		if (a->getSaturation() == b->getSaturation()) {
			return a->getLightness() < b->getLightness();
		}
		return a->getSaturation() < b->getSaturation();
		//bugbug need to add in sort by size, count, saturation, brightness, object size etc
	});

}
void LiveArt::haveBeenNotifiedFloat(float &f) {
}
void LiveArt::haveBeenNotifiedInt(int &i) {
}
void LiveArt::haveBeenNotifiedBool(bool &b) {
}
void LiveArt::haveBeenNotifiedDouble(double &d) {
}
void LiveArt::redoButtonPressed() {

	ofLog() << " event at redo " << endl;

	// find the current image and start its thread to compile contours
	for (int i = 0; i < images.size(); ++i) {
		// if current thread is already running, or the current image is not this one, skip this scan
		if (images[i]->mythread.isThreadRunning() || images[i]->shortname != currentImageName.get()) {
			ofLogNotice() << "ignore " << images[i]->name << endl;
			continue;
		}

		images[i]->smoothAmount = smoothAmount;
		images[i]->pictureType = pictureType;
		images[i]->threshold = threshold;

		images[i]->filter(pictureType);
		if (!images[i]->getCountours) {
			ofLogNotice() << "getCountours false " << endl;
			continue;
		}

		//images[i]->mythread.finder.setMinAreaRadius(minRadius);
		//images[i]->mythread.finder.setMaxAreaRadius(maxRadius);
		images[i]->mythread.stop = false;
		images[i]->mythread.image = images[i];
		images[i]->mythread.startThread();
	}

}

void LiveArt::setMenu(ofxPanel &gui) {

	//consider https://github.com/frauzufall/ofxGuiExtended
	ofxGuiSetTextPadding(4);

	ofParameterGroup realtime;
	realtime.add(targetColor.set("RGB", targetColor, 0.0, 255.0));
	realtime.add(allColors.set("all colors", 0, 0, 100000));
	
	realtime.add(count.set("used count", 0));
	gui.setup(realtime, "setup", xImage *2, 0);

	ofParameterGroup settings;
	settings.setName("settings");

	settings.add(threshold.set("Threshold", 10, 1.0, 255.0)); // 8 works great, 5 not as much, 10?

	settings.add(minRadius.set("minRadius", 1, 0.0, 255.0));
	settings.add(maxRadius.set("maxRadius", 150, 0.0, 255.0));
	settings.add(smoothAmount.set("smoothAmount", 5.0, 1.0, 200.0));
	
	settings.add(currentImageName.set("currentImageName"));
	settings.add(pictureType.set("pictureType", 0, 0, 11));
	redo.setup("draw");
	gui.add(&redo);
	redo.addListener(this, &LiveArt::redoButtonPressed);

	gui.add(settings);
	ofParameterGroup ryb;
	ryb.setName("RYB");
	ryb.add(red.set("r", red, 0.0, 255.0));
	ryb.add(yellow.set("y", yellow, 0.0, 255.0));
	ryb.add(blue.set("b", blue, 0.0, 255.0));
	
	gui.add(ryb);

}
void Image::filter(int id){
#define MAX_KERNEL_LENGTH 31

	//http://docs.opencv.org/2.4/doc/tutorials/imgproc/gausian_median_blur_bilateral_filter/gausian_median_blur_bilateral_filter.html
	//bugbug get the canny ones here too
	if (!img.load(name)) {
		ofLogError() << "cannot read " << name;
		return;
	}
	img.resize(xImage, yImage);
	ofLogNotice("LiveArt::filter") << shortname;
	
	// gui.add(radius.set("Radius", 50, 0, 100));
	Mat src;
	Mat dst;
	ofImage gray;
	getCountours = true;

	switch (id) {
	case 0:
	case 11:
		ofLogNotice("Image::filter") << "no filter";
		break; // no mod
	case 1:
		ofLogNotice("Image::filter") << "dilate and erode " << smoothAmount;
		ofxCv::GaussianBlur(img, 3);
		ofxCv::dilate(img, (int)smoothAmount);
		ofxCv::erode(img, (int)smoothAmount);
		//ofLogNotice("Image::filter") << "equalizeHist";
		//GaussianBlur(img, 3);
		//this changes color, to colors we may not have ofxCv::equalizeHist(img);
		break;
	case 2:
		ofLogNotice("Image::filter") << "GaussianBlur " << smoothAmount;
		ofxCv::GaussianBlur(img, (int)smoothAmount);
		break;
	case 3:
		ofLogNotice("Image::filter") << "blur " << smoothAmount;
		ofxCv::blur(img, (int)smoothAmount);
		break;
	case 4:
		ofLogNotice("Image::filter") << "Canny " << smoothAmount;
		ofxCv::convertColor(img, gray, CV_RGB2GRAY);
		ofxCv::Canny(img, gray, (int)smoothAmount*10, (int)smoothAmount * 40, 3);
		getCountours = false;
		break;
	case 5:
		ofLogNotice("Image::filter") << "bilateralFilter ";
		src = toCv(img).clone();
		cv::bilateralFilter(src, dst, (int)smoothAmount, smoothAmount*10, smoothAmount*2);
		ofxCv::copy(dst, img);
		break;
	case 6:
		ofLogNotice("Image::filter") << "applyColorMap ";
		src = toCv(img).clone();
		cv::applyColorMap(src, dst, COLORMAP_OCEAN);
		ofxCv::copy(dst, img);
		getCountours = false;
		break;
	case 7:
		ofLogNotice("Image::filter") << "Sobel ";
		ofxCv::GaussianBlur(img, 3);
		ofxCv::convertColor(img, gray, CV_RGB2GRAY);
		ofxCv::Sobel(gray, img);
		src = toCv(gray).clone();
		cv::applyColorMap(src, dst, COLORMAP_OCEAN);
		ofxCv::copy(dst, img);
		getCountours = false;
		break;
	case 8:
		ofLogNotice("Image::filter") << "erode " << smoothAmount;
		ofxCv::GaussianBlur(img, 3);
		ofxCv::erode(img, (int)smoothAmount);
		break;
	case 9:
		ofLogNotice("Image::filter") << "dilate " << smoothAmount;
		ofxCv::GaussianBlur(img, 3);
		ofxCv::dilate(img, (int)smoothAmount);
		break;
	case 10:
		ofLogNotice("Image::filter") << "Laplacian " << smoothAmount;
		ofxCv::GaussianBlur(img, 3);
		src = toCv(img).clone();
		cv::Laplacian(src, dst, (int)smoothAmount);
		ofxCv::copy(dst, img);
		getCountours = false;
		break;
	}
}
int LiveArt::getImages() {
	ofDirectory dir("images");
	dir.listDir();
	for (auto& itr : dir) {
		bool found = false;
		for (auto& itr2 : images) {
			if (itr.getFileName() == itr2->shortname) {
				found = true;
				break;
			}
		}
		if (!found) {
			ofLogNotice("LiveArt::getImages") << "add " << itr.getFileName();
			shared_ptr<Image> image = make_shared<Image>(itr.getFileName());
			if (image == nullptr) {
				ofLogError() << itr.getFileName() << " not loaded";
				continue;
			}
			image->filter(pictureType);
			images.push_back(image);
		}
	}

	currentImageName = images[currentImage]->shortname;

	return images.size();
}
shared_ptr<Contours> MyThread::get() {
	shared_ptr<Contours> data = nullptr;
	lock();
	if (!tracedata.empty()) {
		data = tracedata.front();
		tracedata.pop();
	}
	unlock();
	return data;
}
Contours::Contours(const ofColor &color, float threshold) : ContourFinder() { 
	setTargetColor(color); 
	setThreshold(threshold); 
	setAutoThreshold(false); 
	setFindHoles(true);
}
void Contours::draw(float x, float y, bool fill) {
	ofTranslate(x, 0);
	//ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
	ofSetColor(targetColor);
	for (int i = 0; i < getPolylines().size(); ++i) {
		ofPolyline line = getPolyline(i);
		line.draw();
		if (fill) {
			ofTessellator tess;
			tess.tessellateToMesh(line, OF_POLY_WINDING_ODD, mesh, true);
			mesh.draw();
		}
	} 
	//ofDisableBlendMode();
}

void MyThread::threadedFunction() {

	if (image) {
		ofLog() << image->name << "start" << endl;
		image->readColors(); // bugbug read all in, in the future only read in what is shown
		image->readIn = true;
		image->ignoredData.clear();
		image->hits = 0;
		//   contourFinder.setMinAreaNorm(ofMap(mouseY, 0, ofGetHeight(), 0.0, 1.0));
		// less colors, do not draw on top of each other, find holes
		for (int i = 0; !stop && i < image->colorsList.size(); ++i) {
			// put all results in a vector of PolyLines, then sort by size, then draw, save polylines in a file
			shared_ptr<Contours> contours = make_shared<Contours>(*image->colorsList[i], image->threshold);
			if (contours != nullptr) {
				contours->setUseTargetColor(image->img.getImageType() != OF_IMAGE_GRAYSCALE);
				contours->findContours(image->img);
				if (contours->getPolylines().size() > 0) {
					lock();
					tracedata.push(contours);
					unlock();
					++image->hits;
				}
				else {
					image->ignoredData.push_back(image->colorsList[i]);
					image->colorsList.erase(image->colorsList.begin() + i); // assume past indexes remain correct after delete
				}
			}
		}
		// only do this once somethings is found
		if (image->hits > 0) {
			ofFile resultsfile; // save in a file, too much data show -- but the data is key as it shows what is ignored per given rules
			sort(image->ignoredData.begin(), image->ignoredData.end(), [=](shared_ptr<ofColor>  a, shared_ptr<ofColor> b) {
				if (a->r == b->r) {
					if (a->g == b->g) {
						return a->b > b->b;
					}
					return a->g > b->g;
				}
				return a->r > b->r;

			});
			resultsfile.open(image->logDir + ofToString(".notused.dat"), ofFile::WriteOnly);
			LiveArt::toFileHumanForm(resultsfile, image->ignoredData, true);
		}
	}
}
void MyThread::shutItDown() {
	stop = true;
	std::queue<shared_ptr<Contours>> empty;
	std::swap(tracedata, empty);
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

	// bugbug drop light and dark, use "if light then set lightthresh hold" cleans it all up
	// bugbug support warm, cools?

}

void LiveArt::update() {
	// remove unused data
	if (images[currentImage]->mythread.isThreadRunning()) {
		count = images[currentImage]->hits;
	}
	else {
		count = images[currentImage]->colorsList.size();
	}
	currentImageName = images[currentImage]->shortname;
	allColors = images[currentImage]->colorsList.size();
	images[currentImage]->img.update();
}
void LiveArt::draw() {

	ofPushStyle();
	ofSetColor(ofColor::white);
	ofImage i = images[currentImage]->img;
	images[currentImage]->img.draw(0, 0);

	ofSetLineWidth(1);
	shared_ptr<Contours> p;
	if (p = images[currentImage]->mythread.get()) {
		setTargetColor(p->getTargetColor());
		ofSetBackgroundColor(images[currentImage]->warm);
		p->draw(xImage, 0, pictureType!=11);
	}

	ofPopStyle();
}
void LiveArt::advanceImage()
{
	if (images.size() <= 1)
		return;// ignore

	images[currentImage]->mythread.shutItDown();

	currentImage++;
	if (currentImage >= images.size()) {
		currentImage = 0;
	}
	
	currentImageName = images[currentImage]->shortname;
	readIn = false;
	setup();

}
void ofApp::setup() {
	art.setMenu(gui);

	ofSetFrameRate(120);
	
	art.setup();

	ofSetBackgroundAuto(false);
}// 45shavlik11
 //http://www.creativeapplications.net/tutorials/arduino-servo-opencv-tutorial-openframeworks/
 //http://www.autobotic.com.my/ds3218-servo-control-angle-180-degree-25t-servo-armv
void ofApp::update() {
	cam.update();
	art.update();
}
void ofApp::draw() {
	gui.draw();
	art.draw();
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
	else if (key == 'x') {
		string name = "save\\";
		name += ofToString("save.")+art.images[art.currentImage]->shortname;
		ofImage img;
		img.grabScreen(art.xImage, 0, art.xImage, art.yImage);
		img.saveImage(name, OF_IMAGE_QUALITY_BEST);
	}
}
void ofApp::mousePressed(int x, int y, int button) {

	if (x < art.images[art.currentImage]->img.getWidth() && y < art.images[art.currentImage]->img.getHeight()) {
		art.setTargetColor(art.images[art.currentImage]->img.getColor(x, y)); //bugbug make it scan just for this in the future
	}

}