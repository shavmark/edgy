#include "ofApp.h"
#include <time.h>
#include <algorithm>

void LiveArt::snapshot() {
	savex = index;
	index = -1;

	string filename = "data" + ofToString(savecount++);
	filename += ".dat";
	ofFile resultsfile(filename);
	//resultsfile.open(filename, ofFile::WriteOnly);
	toFile(resultsfile, savedcolors, true);
	//resultsfile.close();
	savedcolors.clear();
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
}
void LiveArt::toFile(ofFile& resultsfile, vector<ofColor>&dat, bool clear) {

	if (clear) {
		resultsfile.clear();
	}
	for (auto itr = dat.begin(); itr != dat.end(); ++itr) {
		resultsfile << *itr << "\n";
	}
}
void LiveArt::fromFile(ofFile& resultsfile, vector<ofColor> &dat) {

	while (resultsfile) {
		ofColor cur;
		resultsfile >> cur;
		dat.push_back(cur);
	}
}

bool LiveArt::isCool(ofColor&color) {
	float h = color.getHue();
	return (h > 80 && h < 330);
}
bool LiveArt::find(Shapes&shapes, ofColor&color, bool add) {

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
bool LiveArt::test(Shapes&shapes, ofColor&color, int i, int j, int k) {
	ofColor testColor = color;
	testColor.r += i;
	testColor.g += j;
	testColor.b += k;
	return find(shapes, testColor, false);
}
// return true if color added
bool LiveArt::dedupe(Shapes&shapes, ofColor&color, int rangeR, int rangeG, int rangeB) {

	bool found = false;

	for (int i = 0; i < rangeR && !found; ++i) {
		for (int j = 0; j < rangeG && !found; ++j) {
			for (int k = 0; k < rangeB && !found; ++k) {
				if (test(shapes, color, i, j, k)) {
					found = true;
					break;
				}
				if (test(shapes, color, i, j, -k)) {
					found = true;
					break;
				}
				if (test(shapes, color, i, -j, k)) {
					found = true;
					break;
				}
				if (test(shapes, color, i, -j, -k)) {
					found = true;
					break;
				}
				if (test(shapes, color, -i, j, k)) {
					found = true;
					break;
				}
				if (test(shapes, color, -i, j, -k)) {
					found = true;
					break;
				}
				if (test(shapes, color, -i, -j, k)) {
					found = true;
					break;
				}
				if (test(shapes, color, -i, -j, -k)) {
					found = true;
					break;
				}
			}
		}
	}

	if (!found) {
		if (shapes.find(color.getHex()) == shapes.end()) {
			// color is not in the list
			return find(shapes, color, true);
		}
	}
	return false;
}

void LiveArt::readColors(Image& image, ofFile& resultsfile) {

	if (resultsfile.exists()) {
		// use it
		while (resultsfile) {
			colorData data;
			resultsfile >> data.color;
			image.shapes.insert(make_pair(data.color.getHex(), data));

			if (!isCool(data.color) && data.color.getBrightness() > 200) {
				warm = data.color;// go with most recent
			}
		}
	}
	else {
		// create it	
		vector<ofColor> dat;
		for (int w = 0; w < image.mat.rows; w += 1) {
			for (int h = 0; h < image.mat.cols; h += 1) {
				Vec3b bgrPixel = image.mat.at<Vec3b>(w, h);
				//BGR not RGB
				ofColor color;
				color.b= bgrPixel[0];
				color.g = bgrPixel[1];
				color.r = bgrPixel[2];
				// bugbug ? save all colors so its easier to tweak data later? maybe a different file?

				bool found;
				//if (color.r == 255 && color.g == 255 && color.b == 255) {
				//continue; //ignore white need this to be the back ground color
				//}
				if (!isCool(color) && color.getBrightness() > 200) {
					warm = color;// go with most recent
				}
				if (color.getBrightness() > 255) { // ignore the super bright stuff
					color.setBrightness(255); // see what else can be done here
					found = dedupe(image.shapes, color, 5, 5, 5);
				}
				else {
					found = dedupe(image.shapes, color, 5, 5, 5);
				}
				if (found) {
					dat.push_back(color);
				}
				//bugbug make a mid brightness
			}
		}
		toFile(resultsfile, dat, false);
	}
}

void LiveArt::setMenu(ofxPanel &gui) {
	//https://github.com/frauzufall/ofxGuiExtended
	ofParameterGroup realtime;
	realtime.add(targetColor.set("RGB", 128.0, 0.0, 300.0));
	realtime.add(count.set("count", 0));
	realtime.add(index.set("current", 0));
	realtime.add(targetColor.set("target color", ofColor::white));
	realtime.add(warm.set("tone", ofColor::black));

	gui.setup(realtime, "setup", 1000, 0);

	ofParameterGroup settings;
	gui.add(settings);

	settings.add(threshold.set("Threshold", 5, 0.0, 255.0));


	settings.add(minRadius.set("minRadius", 1, 0.0, 255.0));
	settings.add(maxRadius.set("maxRadius", 1, 0.0, 255.0));
	settings.add(findHoles.set("findHoles", true));
	settings.add(smoothingSize.set("smoothingSize", 2, 0.0, 255.0));
	settings.add(smoothingShape.set("smoothingShape", 0.0, 0.0, 255.0));
	settings.add(xImage.set("xImage", 500,20, 4000.0));
	settings.add(yImage.set("yImage", 500,20, 4000.0));
	settings.add(d.set("d", 15, 0.0, 255.0));
	settings.add(sigmaColor.set("sigmaColor", 80, 0.0, 255.0));
	settings.add(sigmaSpace.set("sigmaSpace", 80, 0.0, 255.0));
	settings.add(currentImageName.set("currentImageName"));

}
bool LiveArt::loadAndFilter(Image& image) {
	cv::Mat tempMat;
	loadMat(tempMat, image.name);
	if (!tempMat.data) {
		ofLogError() << image.name << " not loaded";
		return false;
	}
	tempMat.create(xImage, yImage, tempMat.type());
	cv::bilateralFilter(tempMat, image.mat, d, sigmaColor, sigmaSpace);
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
		currentImageName = itr->name;
		readColors(*itr, ofFile(itr->name + string("image.data.dat")));
		// less colors, do not draw on top of each other, find holes
		for (auto& itr2 = itr->shapes.begin(); itr2 != itr->shapes.end(); ++itr) {
			// bugbug install backup software
			// put all results in a vector of PolyLines, then sort by size, then draw, save polylines in a file
			finder.setTargetColor(itr2->second.color, TRACK_COLOR_RGB);
			finder.findContours(itr->mat);
			if (finder.getPolylines().size() > 1) {
				itr2->second.lines = finder.getPolylines();
				itr2->second.threshold = threshold;
			}
			itr->drawingData.push_back(itr2->second); // shadow vector for good sorting
		}
		sort(itr->drawingData.begin(), itr->drawingData.end(), [=](colorData&  a, colorData&  b) {
			return a.color.getSaturation() > a.color.getSaturation();
			//bugbug need to add in sort by size, count, saturation, brightness, object size etc
		});
	}

	warm = ofColor::lightYellow; // default

	// bugbug drop light and dark, use "if light then set lightthresh hold" cleans it all up
	// bugbug support warm, cools?

}

void LiveArt::update() {
	count = images[currentImage].shapes.size();

}
void LiveArt::draw() {
	ofSetColor(targetColor);
	ofDrawRectangle(0, 700, 64, 64);

	ofSetColor(ofColor::white);
	ofSetBackgroundColor(warm);//bugbug use lightest found color

	ofSetLineWidth(1);

	//images[currentImage].mat.draw(xImage, 0);// test with 2000,2000 image
	cvShowImage("", &images[currentImage].mat);
						  //ofTranslate(300, 0); keep as a reminder
	if (index >= 0) {
		ofPushStyle();
		// less colors, do not draw on top of each other, find holes
		ofSetColor(images[currentImage].drawingData[index].color); // varibles here include only show large, or smalll, to create different pictures
		savedcolors.push_back(images[currentImage].drawingData[index].color);
		echo(images[currentImage].drawingData[index].lines);
		if (++index >= count) {
			index = -1; // stop
		}
		ofPopStyle();
	}


}
void LiveArt::echo(vector<ofPolyline>&lines) {

	// use this? fillPoly in wrappers.h
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
	art.draw();
	gui.draw();

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
			art.snapshot();
		}
	}
	else if (key == 'i') {
		art.currentImage++;
		if (art.currentImage > art.images.size()) {
			art.currentImage = 0;
		}
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