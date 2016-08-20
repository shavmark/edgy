#include "ofApp.h"
#include <time.h>
#include <algorithm>

void toFile(string path, vector<std::pair<ofColor, int>>&dat) {

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
void toFile(string path, vector<ofColor>&dat, bool clear=false) {

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
void fromFile(string path, vector<ofColor> &dat) {
	ofFile file(path);

	while (file) {
		ofColor cur;
		file >> cur;
		dat.push_back(cur);
	}
}

bool isCool(ofColor&color) {
	float h = color.getHue();
	return (h > 80 && h < 330);
}
bool ofApp::find(ofColor&color, bool add) {

	map<int, int>::iterator itr = colorhash.find(color.getHex());
	if (itr != colorhash.end()) {
		++itr->second;
		return true;
	}
	else {
		if (add) {
			colorData data;
			data.color = color;
			shapes.push_back(data);
			
			colorhash.insert(make_pair(color.getHex(), 1));
			return true;
		}
		return false;
	}
}
bool ofApp::test(ofColor&color, int i, int j, int k) {
	ofColor testColor = color;
	testColor.r += i;
	testColor.g += j;
	testColor.b += k;
	return find(testColor, false);
}
// return true if color added
bool ofApp::dedupe(ofColor&color, int rangeR, int rangeG, int rangeB) {

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
		map<int, int>::iterator itr = colorhash.find(color.getHex());

		if (itr == colorhash.end()) {
			// color is not in the list
			return find(color, true);
		}
	}
	return false;
}

void ofApp::readColors() {
	string filename = "pic3.dat";
	ofFile file(filename);
	if (file.exists()) {
		// use it
		while (file) {
			colorData data;
			file >> data.color;
			shapes.push_back(data);
			colorhash.insert(make_pair(data.color.getHex(), 1));

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
		toFile(filename, dat);
	}
}
void ofApp::setup() {
	cam.setup(640, 480);
	//https://github.com/frauzufall/ofxGuiExtended
	gui.setup(group, "setup", 1000, 0);
	gui.add(threshold.set("Threshold", 5, 0.0, 255.0));
	gui.add(targetColor.set("RGB", 128.0, 0.0, 300.0));
	gui.add(generatecolors.set("genearate", true));
	gui.add(count.set("count", 0));
	gui.add(index.set("current", 0));

	bool b = image.load("photo2.jpg");//bugbug menu ize
	image.resize(500, 500);
	warm = ofColor::lightYellow;
	cv::Mat img2 = toCv(image);

	cv::bilateralFilter(img2, img, 15, 80, 80);

	ofSetFrameRate(120);
	ofxCv::toOf(img, image);
	image.setImageType(OF_IMAGE_COLOR); // should not need this? TODO any over-head / conversion?

	// bugbug support warm, cools?

	readColors();

	// bugbug drop light and dark, use "if light then set lightthresh hold" cleans it all up


#ifdef DEBUG2
	// sort results, this enables more flexibilty at run time
	sort(colors.begin(), colors.end(), [=](std::pair<ofColor, int>& a, std::pair<ofColor, int>& b) {
		// push less saturated back, then darks
		if (a.first.getSaturation() == b.first.getSaturation()) {
			if (a.first.getBrightness() == b.first.getBrightness()) {
				return a.first.getLightness() > b.first.getLightness();
			}
			return a.first.getBrightness() > b.first.getBrightness();
		}
		return a.first.getSaturation() < b.first.getSaturation();
		/* for debug
		if (a.first.r == b.first.r)
		if (a.first.g == b.first.g)
		return a.first.b > b.first.b;
		else
		return  a.first.g > b.first.g;
		else
		return  a.first.r > b.first.r;

		*/


	}
	);
#endif // DEBUG2

	count = shapes.size();

	//ofTranslate(300, 0);
	// less colors, do not draw on top of each other, find holes
	ContourFinder finder;
	finder.setMinAreaRadius(1);
	finder.setMaxAreaRadius(150);
	finder.setSimplify(true);
	finder.setAutoThreshold(false);
	finder.setThreshold(threshold);
	finder.setUseTargetColor(true);
	finder.setFindHoles(true);// matters
	finder.setSortBySize(false);
	for (int i = 0; i < count; ++i){
		//bugbug move this to a function that can be called at any time to reset things
		//bugbug save getPolylines in a file so redraw is fast
		ofColor color = shapes[i].color;
		// put all results in a vector of PolyLines, then sort by size, then draw, save polylines in a file
		finder.setTargetColor(color, TRACK_COLOR_RGB);
		finder.findContours(img);
		if (finder.getPolylines().size() > 1) {
			colorData data(color);
			vector<colorData>::iterator itr = std::find(shapes.begin(), shapes.end(), data);
			if (itr != shapes.end()) {
				itr->lines = finder.getPolylines();
				itr->threshold = threshold;
			}
		}
	}
	sort(shapes.begin(), shapes.end(), [=](colorData a, colorData b)	{
		return a.color.getSaturation() > a.color.getSaturation();//bugbug need to add in sort by saturation, brightness, object size etc
	});
	ofSetBackgroundAuto(false);
}// 45shavlik11
 //http://www.creativeapplications.net/tutorials/arduino-servo-opencv-tutorial-openframeworks/
 //http://www.autobotic.com.my/ds3218-servo-control-angle-180-degree-25t-servo-armv
void ofApp::update() {
	cam.update();
	image.update();

	//convertColor(cam, gray, CV_RGB2GRAY);
	//GaussianBlur(gray, gray, gray.getWidth());
	// Canny(gray, edge, mouseX, mouseY, 3);
	//Sobel(gray, sobel);

	// gray.update();
	// sobel.update();
	//edge.update();



}
void ofApp::echo(vector<ofPolyline>&lines) {

	for (int j = 0; j <lines.size(); j++) {
		ofPolyline line = lines[j].getSmoothed(2); //bugbug test this data
		ofTessellator tess;
		ofMesh mesh;
		tess.tessellateToMesh(line, OF_POLY_WINDING_ODD, mesh, true);
		mesh.draw();
		line.draw();
	}
}
void ofApp::draw() {
	ofSetColor(targetColor);
	ofDrawRectangle(0, 700, 64, 64);

	ofSetColor(ofColor::white);
	ofSetBackgroundColor(warm);//bugbug use lightest found color
	gui.draw();

	//ofFill();
	//cam.draw(0, 0);
	//gray.draw(0, 480);(
	//edge.draw(640, 0);
	//sobel.draw(640, 480);
	ofSetLineWidth(1);

	ofxCv::toOf(img, image);
	image.draw(500, 0);// test with 2000,2000 image

	//ofTranslate(300, 0); keep as a reminder
	if (index >= 0) {
		ofPushStyle();
		// less colors, do not draw on top of each other, find holes
		ofSetColor(shapes[index].color); // varibles here include only show large, or smalll, to create different pictures
		savedcolors.push_back(shapes[index].color);
		echo(shapes[index].lines);
		if (++index >= count) {
			index = -1; // stop
		}
		ofPopStyle();
	}
	return;

}
void ofApp::keyPressed(int key) {
	if (key == ' ') {
		if (index == -1) {
			index = savex;
		}
		else {
			savex = index;
			index = -1;

			string filename = "data" + ofToString(savecount++);
			filename += ".dat";
			toFile(filename, savedcolors, true);
			savedcolors.clear();
		}
	}
	else if (key == 'g') {
		index = 0; // go from start
	}
	else if (key == 'b') {
		index -= 20; // hit b a bunch of times to get back to the start
		if (index < 0) {
			index = 0;
		}
	}
}