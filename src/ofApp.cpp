#include "ofApp.h"
#include <time.h>
#include <algorithm>

void LiveArt::snapshot() {
	savex = index;
	index = -1;

	string filename = "data" + ofToString(savecount++);
	filename += ".dat";
	toFile(filename, savedcolors, true);
	savedcolors.clear();
}


void LiveArt::toFile(string path, vector<std::pair<ofColor, int>>&dat) {

	ofFile file(path);
	file.open(path, ofFile::WriteOnly);
	time_t rawtime;
	time(&rawtime);

	file << ctime(&rawtime) << "\n";

	int i = 1;
	for (auto itr = dat.begin(); itr != dat.end(); ++itr) {
		file << i << ":" << itr->first << ":" << itr->second << "\n";
		++i;
	}
	file.close();
}
void LiveArt::toFile(string path, vector<ofColor>&dat, bool clear) {

	ofFile file(path);
	file.open(path, ofFile::WriteOnly);
	if (clear) {
		file.clear();
	}
	for (auto itr = dat.begin(); itr != dat.end(); ++itr) {
		file << *itr << "\n";
	}
	file.close();
}
void LiveArt::fromFile(string path, vector<ofColor> &dat) {
	ofFile file(path);

	while (file) {
		ofColor cur;
		file >> cur;
		dat.push_back(cur);
	}
}

bool LiveArt::isCool(ofColor&color) {
	float h = color.getHue();
	return (h > 80 && h < 330);
}
bool LiveArt::find(ofColor&color, bool add) {

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
bool LiveArt::test(ofColor&color, int i, int j, int k) {
	ofColor testColor = color;
	testColor.r += i;
	testColor.g += j;
	testColor.b += k;
	return find(testColor, false);
}
// return true if color added
bool LiveArt::dedupe(ofColor&color, int rangeR, int rangeG, int rangeB) {

	bool found = false;

	for (int i = 0; i < rangeR && !found; ++i) {
		for (int j = 0; j < rangeG && !found; ++j) {
			for (int k = 0; k < rangeB && !found; ++k) {
				if (test(color, i, j, k)) {
					found = true;
					break;
				}
				if (test(color, i, j, -k)) {
					found = true;
					break;
				}
				if (test(color, i, -j, k)) {
					found = true;
					break;
				}
				if (test(color, i, -j, -k)) {
					found = true;
					break;
				}
				if (test(color, -i, j, k)) {
					found = true;
					break;
				}
				if (test(color, -i, j, -k)) {
					found = true;
					break;
				}
				if (test(color, -i, -j, k)) {
					found = true;
					break;
				}
				if (test(color, -i, -j, -k)) {
					found = true;
					break;
				}
			}
		}
	}

	if (!found) {
		map<int, colorData>::iterator itr = shapes.find(color.getHex());

		if (itr == shapes.end()) {
			// color is not in the list
			return find(color, true);
		}
	}
	return false;
}

void LiveArt::readColors() {
	string filename = imagePath;
	filename += ".pic.dat";
	ofFile file(filename);
	if (file.exists()) {
		// use it
		while (file) {
			colorData data;
			file >> data.color;
			shapes.insert(make_pair(data.color.getHex(), data));

			if (!isCool(data.color) && data.color.getBrightness() > 200) {
				warm = data.color;// go with most recent
			}
		}
	}
	else {
		// create it	
		vector<ofColor> dat;

		for (int w = 0; w < image.getWidth(); w += 1) {
			for (int h = 0; h < image.getHeight(); h += 1) {
				ofColor color = image.getPixels().getColor(w, h);
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
					found = dedupe(color, 5, 5, 5);
				}
				else {
					found = dedupe(color, 5, 5, 5);
				}
				if (found) {
					dat.push_back(color);
				}
				//bugbug make a mid brightness
			}
		}
		toFile(filename, dat, false);
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
	settings.add(yImage.set("yImage", 500, 20, 4000.0));
	settings.add(d.set("d", 15, 0.0, 255.0));
	settings.add(sigmaColor.set("sigmaColor", 80, 0.0, 255.0));
	settings.add(sigmaSpace.set("sigmaSpace", 80, 0.0, 255.0));
	settings.add(imagePath.set("imagePath"));
	settings.add(imagePath.set("imagePath"));
	settings.add(imagePath.set("imagePath"));
	settings.add(imagePath.set("imagePath"));

}
void LiveArt::setup() {
	bool b = image.load(imagePath);//bugbug menu ize

	image.resize(xImage, yImage);
	warm = ofColor::lightYellow; // default
	cv::Mat img2 = toCv(image);
	cv::bilateralFilter(img2, img, d, sigmaColor, sigmaSpace);
	ofxCv::toOf(img, image);

	image.setImageType(OF_IMAGE_COLOR); // should not need this? TODO any over-head / conversion?

										// bugbug drop light and dark, use "if light then set lightthresh hold" cleans it all up
										// bugbug support warm, cools?

	readColors();

	count = shapes.size();

	//ofTranslate(300, 0);
	// less colors, do not draw on top of each other, find holes
	ContourFinder finder;
	finder.setMinAreaRadius(minRadius);

	finder.setMaxAreaRadius(maxRadius);

	finder.setSimplify(true);
	finder.setAutoThreshold(false);
	finder.setThreshold(threshold);
	finder.setUseTargetColor(true);
	finder.setFindHoles(findHoles);// matters
	for (int i = 0; i < count; ++i) {
		//bugbug move this to a function that can be called at any time to reset things
		//bugbug save getPolylines in a file so redraw is fast
		// install backup software
		ofColor color = shapes[i].color;
		// put all results in a vector of PolyLines, then sort by size, then draw, save polylines in a file
		finder.setTargetColor(color, TRACK_COLOR_RGB);
		finder.findContours(img);
		if (finder.getPolylines().size() > 1) {
			colorData data(color);
			map<int, colorData>::iterator itr = shapes.find(color.getHex());
			if (itr != shapes.end()) {
				itr->second.lines = finder.getPolylines();
				itr->second.threshold = threshold;
			}
			// error if not found
		}
	}
	// vector is used to draw as sort is easy to do
	for (map<int, colorData>::iterator it = shapes.begin(); it != shapes.end(); ++it) {
		drawingData.push_back(it->second);
	}
	sort(drawingData.begin(), drawingData.end(), [=](colorData&  a, colorData&  b) {
		return a.color.getSaturation() > a.color.getSaturation();
		//bugbug need to add in sort by size, count, saturation, brightness, object size etc
	});


}

void LiveArt::update() {
	image.update();
}
void LiveArt::draw() {
	ofSetColor(targetColor);
	ofDrawRectangle(0, 700, 64, 64);

	ofSetColor(ofColor::white);
	ofSetBackgroundColor(warm);//bugbug use lightest found color

	ofSetLineWidth(1);

	image.draw(xImage, 0);// test with 2000,2000 image

						  //ofTranslate(300, 0); keep as a reminder
	if (index >= 0) {
		ofPushStyle();
		// less colors, do not draw on top of each other, find holes
		ofSetColor(drawingData[index].color); // varibles here include only show large, or smalll, to create different pictures
		savedcolors.push_back(drawingData[index].color);
		echo(drawingData[index].lines);
		if (++index >= count) {
			index = -1; // stop
		}
		ofPopStyle();
	}


}
void LiveArt::echo(vector<ofPolyline>&lines) {

	for (int j = 0; j <lines.size(); j++) {
		ofPolyline line = lines[j].getSmoothed(2); //bugbug test this data
		ofTessellator tess;
		ofMesh mesh;
		tess.tessellateToMesh(line, OF_POLY_WINDING_ODD, mesh, true);
		mesh.draw();
		line.draw();
	}
}

void ofApp::setup() {
	cam.setup(640, 480);

	ofSetFrameRate(120);
	
	art.setup();
	art.setMenu(gui);


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
	else if (key == 'g') {
		art.index = 0; // go from start
	}
	else if (key == 'b') {
		art.index -= 20; // hit b a bunch of times to get back to the start
		if (art.index < 0) {
			art.index = 0;
		}
	}
}