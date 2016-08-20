#include "ofApp.h"
#include <time.h>
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
void toFile(string path, vector<ofColor>&dat) {


	ofFile file(path);
	file.open(path, ofFile::WriteOnly);
	
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
bool find(unordered_map<int, int> &finder, ofColor&color, bool add) {
	std::unordered_map<int, int>::iterator got = finder.find(color.getHex());
	if (got == finder.end()) {
		if (add) {
			std::pair<int, int> add(color.getHex(), 0);
			finder.insert(add);
			return true;
		}
		return false;
	}
	else {
		got->second += 1;
		if (!add) {
			int i = 0;
		}
		return true;
	}
}
bool test(unordered_map<int, int>&colors, ofColor&color, int i, int j, int k) {
	ofColor testColor = color;
	testColor.r += i;
	testColor.g += j;
	testColor.b += k;
	return find(colors, testColor, false);
}
<<<<<<< HEAD
// return true if color added
bool dedupe(unordered_map<int, int>&colors, ofColor&color, int rangeR, int rangeG, int rangeB) {

	bool found = false;
=======
void dedupe(unordered_map<int, int>&colors, ofColor&color, int rangeR, int rangeG, int rangeB) {

	bool found = false;

>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
	
	for (int i = 0; i < rangeR && !found; ++i) {
		for (int j = 0; j < rangeG && !found; ++j) {
			for (int k = 0; k < rangeB && !found; ++k) {
				if (test(colors, color, i, j, k)) {
					found = true;
					break;
				}
				if (test(colors, color, i, j, -k)) {
					found = true;
					break;
				}
				if (test(colors, color, i, -j, k)) {
					found = true;
					break;
				}
				if (test(colors, color, i, -j, -k)) {
					found = true;
					break;
				}
				if (test(colors, color, -i, j, k)) {
					found = true;
					break;
				}
				if (test(colors, color, -i, j, -k)) {
					found = true;
					break;
				}
				if (test(colors, color, -i, -j, k)) {
					found = true;
					break;
				}
				if (test(colors, color, -i, -j, -k)) {
					found = true;
					break;
				}
			}
		}
	}

	if (!found) {
<<<<<<< HEAD
		std::unordered_map<int, int>::iterator got = colors.find(color.getHex());
		if (got == colors.end()) {
			// color is not in the list
			return find(colors, color, true);
		}
	}
	return false;
}

void ofApp::readColors(unordered_map<int, int> &findDarkcolors, unordered_map<int, int>& findBrightcolors) {
	string filename = "pic3.dat";
	ofFile file(filename);
	if (file.exists()) {
		// use it
		while (file) {
			ofColor color;
			file >> color;
			if (!isCool(color) && color.getBrightness() > 200) {
				warm = color;// go with most recent
			}

			if (color.getBrightness() > 255) {
				color.setBrightness(230);
				dedupe(findDarkcolors, color, 5, 5, 5);
			}
			else {
				dedupe(findDarkcolors, color, 5, 5, 5);
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
					found = dedupe(findDarkcolors, color, 5, 5, 5);
				}
				else {
					found = dedupe(findDarkcolors, color, 5, 5, 5);
				}
				if (found) {
					dat.push_back(color);
				}
				//bugbug make a mid brightness
			}
		}
		toFile(filename, dat);
	}
=======
		find(colors, color, true);
	}

>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
}
void ofApp::setup() {
    cam.setup(640, 480);
	//https://github.com/frauzufall/ofxGuiExtended
	gui.setup(group, "setup",1000, 0);
<<<<<<< HEAD
	gui.add(threshold.set("Threshold", 5, 0.0, 255.0));
	gui.add(targetColor.set("RGB", 128.0, 0.0, 300.0));
	gui.add(generatecolors.set("genearate", true));
	gui.add(count.set("count", 0));
	gui.add(index.set("current", 0));

	bool b = image.load("photo2.jpg");//bugbug menu ize
	image.resize(500, 500);
	warm = ofColor::lightYellow;
=======
	gui.add(threshold.set("Threshold", 120, 0.0, 255.0));
	gui.add(targetColor.set("RGB", 128.0, 0.0, 300.0));
	gui.add(generatecolors.set("genearate", true));
	
	bool b = image.load("photo2.jpg");//bugbug menu ize
	image.resize(500, 500);
	
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
	cv::Mat img2 = toCv(image);

	cv::bilateralFilter(img2, img, 15, 80, 80);
	
	ofSetFrameRate(120);
	ofxCv::toOf(img, image);
	image.setImageType(OF_IMAGE_COLOR); // should not need this? TODO any over-head / conversion?
<<<<<<< HEAD

	unordered_map<int, int> findDarkcolors;
	unordered_map<int, int> findBrightcolors;//bugbug warm/cool?
	readColors(findDarkcolors, findBrightcolors);
=======
	unordered_map<int, int> findDarkcolors;
	unordered_map<int, int> findBrightcolors;//bugbug warm/cool?
	ofPixels pixels = image.getPixels();
	// variables: filter, w,h and my compression, threshold, pic size, finder.setMinAreaRadius(5);
	//finder.setMaxAreaRadius(150); warm/cool?
	for (int w = 0; w < image.getWidth(); w += 1) {
		for (int h = 0; h < image.getHeight(); h += 1) {
			ofColor c = image.getPixels().getColor(w, h);
			//if (!isCool(c)) {
			if (c.r == 255 && c.g == 255 && c.b == 255) {
				continue; //ignore white bugbug or add it once or something, its too dominate?
			}
			if (c.getBrightness() > 180) {
				dedupe(findBrightcolors, c, 0, 0, 0);
			}
			else {
				dedupe(findDarkcolors, c, 2,2,2);
			}
			continue;
			//make a mid brightness
		}
	}
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
	
	for (auto itr = findDarkcolors.begin(); itr != findDarkcolors.end(); ++itr) {
		ofColor c = ofColor::fromHex(itr->first);
		std::pair<ofColor, int> pair(c, itr->second);
		
		pairsDark.push_back(pair);
		
	}
<<<<<<< HEAD
	// drop light and dark, use "if light then set lightthresh hold" cleans it all up
=======
	
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
	vector< pair<ofColor, int> >::iterator it = pairsLight.begin();
	for (auto itr = findBrightcolors.begin(); itr != findBrightcolors.end(); ++itr) {
		ofColor c = ofColor::fromHex(itr->first);
		std::pair<ofColor, int> pair(c, itr->second);

		pairsLight.push_back(pair);

	}
#ifdef DEBUG1
	it = pairsDark.begin();
	while (it != pairsDark.end() - 1) {
		if (it->second < 5) {
			it = pairsDark.erase(it); // color needs to appear at least 10 times before we use it
		}
		else ++it;
	}

#endif // DEBUG1


	sort(pairsDark.begin(), pairsDark.end(), [=](std::pair<ofColor, int>& a, std::pair<ofColor, int>& b)
	{
<<<<<<< HEAD
		// push less saturated back, then darks
		if (a.first.getSaturation() == b.first.getSaturation()) {
			if (a.first.getBrightness() == b.first.getBrightness()) {
				return a.first.getLightness() > b.first.getLightness();
			}
			return a.first.getBrightness() > b.first.getBrightness();
		}
		return a.first.getSaturation() < b.first.getSaturation();
		/* for debug
=======
		return a.first.getSaturation() < b.first.getSaturation();
		/* for debug 
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
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
	sort(pairsLight.begin(), pairsLight.end(), [=](std::pair<ofColor, int>& a, std::pair<ofColor, int>& b)
	{
<<<<<<< HEAD
		if (a.first.getSaturation() == b.first.getSaturation()) {
			return a.first.getBrightness() > b.first.getBrightness();
		}
		return a.first.getBrightness() > b.first.getBrightness();
=======
		return a.first.getSaturation() < b.first.getSaturation();
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
	}
	);
	
	//for debug more data toFile("lotsofdata2.dat", pairsLight);
<<<<<<< HEAD
	pairsCurrent = &pairsDark;
=======
	pairsCurrent = &pairsLight;
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
	ofSetBackgroundColor(ofColor::white);
	ofSetBackgroundAuto(false);
}// 45shavlik11
//http://www.creativeapplications.net/tutorials/arduino-servo-opencv-tutorial-openframeworks/
//http://www.autobotic.com.my/ds3218-servo-control-angle-180-degree-25t-servo-armv
<<<<<<< HEAD
void ofApp::update() { 
    cam.update();
	image.update();

=======
void ofApp::update() {
    cam.update();
	image.update();
	
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
        //convertColor(cam, gray, CV_RGB2GRAY);
		//GaussianBlur(gray, gray, gray.getWidth());
       // Canny(gray, edge, mouseX, mouseY, 3);
		//Sobel(gray, sobel);
		
       // gray.update();
       // sobel.update();
        //edge.update();

		
	
}
<<<<<<< HEAD
void ofApp::echo(ContourFinder& finder, bool cleanPixelsOnly) {

	for (int j = 0; j < finder.getPolylines().size(); j++) {
		// save the pixels we will set
		if (cleanPixelsOnly) {
			ofPolyline line = finder.getPolylines()[j].getSmoothed(2);
			for (int z = 0; z < line.size(); ++z) {
				std::ostringstream ss;
				ss << (int)line[z].x << ":" << (int)line[z].y;
				std::string s(ss.str());
				unordered_set<string>::iterator points = hitPixels.find(s);
				if (points == hitPixels.end()) {
					hitPixels.insert(s);
					ofDrawCircle(line[z].x, line[z].y, 1);
				}
			}
		}
		else {
			ofPolyline line = finder.getPolylines()[j].getSmoothed(2);
			for (int z = 0; z < line.size(); ++z) {
				std::ostringstream ss;
				ss << (int)line[z].x << ":" << (int)line[z].y;
				std::string s(ss.str());
				unordered_set<string>::iterator points = hitPixels.find(s);
				if (points == hitPixels.end()) {
					hitPixels.insert(s);
				}
			}
=======
void ofApp::echo(ofColor&color, ContourFinder& finder) {
	int sz = finder.getPolylines().size();
	if (finder.getPolylines().size() > 0) {
		//color.a = 100;
		ofSetColor(color);

		if (finder.getPolylines().size() > 1) {
			ofTessellator tess;
			ofMesh mesh;
			//tess.tessellateToMesh(finder.getPolylines(), OF_POLY_WINDING_ODD, mesh, true);
			//mesh.draw();
			//mesh.drawFaces();

		}
		for (int j = 0; j < finder.getPolylines().size(); j++) {
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
			ofTessellator tess;
			ofMesh mesh;
			tess.tessellateToMesh(finder.getPolylines()[j].getSmoothed(2), OF_POLY_WINDING_ODD, mesh, true);
			mesh.draw();
			finder.getPolylines()[j].getSmoothed(2).draw();
<<<<<<< HEAD

		}

	}
}
void ofApp::draw() {
	ofSetColor(targetColor);
	ofDrawRectangle(0, 700, 64, 64);

	ofSetColor(ofColor::white);
	ofSetBackgroundColor(warm);//bugbug use lightest found color
	gui.draw();
	
=======
		}
	}
}
void ofApp::draw() {
	gui.draw();
	
	ofSetColor(targetColor);
	ofDrawRectangle(0, 700, 64, 64);
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509

	//ofFill();
	//cam.draw(0, 0);
	//gray.draw(0, 480);(
	//edge.draw(640, 0);
	//sobel.draw(640, 480);
	ofSetLineWidth(1);
<<<<<<< HEAD

=======
	ofSetColor(255, 255, 255);
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
	if (1) {
		once = true;
		ofxCv::toOf(img, image);
		image.draw(500, 0);
	}
	
	ofPushStyle();
<<<<<<< HEAD
	count = pairsCurrent->size();
	if (index >= pairsCurrent->size() && index != -1) {
		index = 0;
		
		if (pairsCurrent == &pairsDark) {
			pairsCurrent = &pairsLight;// flip to dark if currently light
		}
		else {
			pairsCurrent = &pairsDark;//bugbug flip to dark if currently light
		}
	}
	if (pairsCurrent == &pairsDark) {
		threshold = 10;
	}
	else {
		threshold = 180;
	}
	

	//ofTranslate(300, 0);
=======
	
	if (index >= pairsCurrent->size() && index != -1) {
		index = 0;
		if (pairsCurrent == &pairsDark) {
			threshold = 180;
			pairsCurrent = &pairsLight;// flip to dark if currently light
		}
		else {
			threshold = 10;
			pairsCurrent = &pairsDark;//bugbug flip to dark if currently light

		}

	}
	//ofTranslate(300, 0);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
	// less colors, do not draw on top of each other, find holes
	while (pairsCurrent && index > -1 && index < pairsCurrent->size()) {

		ofColor color;
		color = (*pairsCurrent)[index++].first;
		ContourFinder finder;
<<<<<<< HEAD
		finder.setMinAreaRadius(1);
=======
		finder.setMinAreaRadius(5);
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
		finder.setMaxAreaRadius(150);
		finder.setSimplify(true);
		finder.setAutoThreshold(false);
		
		finder.setUseTargetColor(true);
<<<<<<< HEAD
		finder.setFindHoles(true);// matters
		finder.setSortBySize(false);
		// put all results in a vector of PolyLines, then sort by size, then draw
		// 'b' moves index back by 10
=======
		finder.setFindHoles(true);
		finder.setSortBySize(true);
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
		//color = ofColor::yellow;
		finder.setTargetColor(color, TRACK_COLOR_RGB);
		finder.setThreshold(threshold);
		finder.findContours(img);
		if (finder.getPolylines().size() > 1) {
<<<<<<< HEAD
			ofSetColor(color);
			echo(finder, pairsCurrent != &pairsDark);
=======
			//polys.insert( finder.getPolylines());
			echo(color, finder);
>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
			targetColor = color;
			savedcolors.push_back(color);
			break;
		}
		else {
			int i = 0;
			targetColor = ofColor::black;
		}
	
	}
	
<<<<<<< HEAD
	ofPopStyle();
	return;

=======
	
	ofPopStyle();
	return;

	

	
	ofPopStyle();

>>>>>>> e14c94331bd39f8af161c3af4e12158d13843509
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
			toFile(filename, savedcolors);
			savedcolors.clear();
		}
		
	}
}