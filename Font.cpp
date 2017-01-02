// Module:			Gameplay Programming
// Assignment 1:	Pixl
// Student Name:	Elcoms Khang	(S10157558A)
//					Lin Lue			(S10158175E)
//					Amos Tan		(S10158017D)

// Module:			Gameplay Programming
// Assignment 1:	Pixl
// Student Name:	Elcoms Khang	(S10157558A)
//					Lin Lue			(S10158175E)
//					Amos Tan		(S10158017D)

#include <fstream>
#include "Font.h"
#include "spacewar.h"

Font::Font() {

	// set everything in array to 0
	memset(&this->widths, 0, sizeof(widths));

	spriteData.width = 128;
	spriteData.height = 128;
	spriteData.x = 0;
	spriteData.y = 0;
	spriteData.rect.right = spriteData.width;
	spriteData.rect.bottom = spriteData.height;
	spriteData.scale = 0.5;
	velocity.x = 0;
	velocity.y = 0;
	startFrame = 0;
	endFrame = 127;
	currentFrame = 0;

}

bool Font::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM) {
	return (Entity::initialize(gamePtr, width, height, ncols, textureM));
}

bool Font::loadTextData(std::string fileName) {

	char buffer[512];

	std::ifstream infile;
	infile.open(fileName.c_str(), std::ios::binary);
	if (!infile) return false;

	// read 512 bytes (2 bytes per character)

	infile.read((char*)(&buffer), 512);
	if (infile.bad()) return false;
	infile.close();

	for (int i = 0; i < sizeof(this->widths) / sizeof(this->widths[0]); i++) {
		widths[i] = (int)buffer[i * 2];
	}

	return true;
}

void Font::Print(int x, int y, std::string text) {

	int fx = x;
	int fy = y;

	for (int i = 0; i < text.length(); i++) {

		// char can be cast to int
		int frame = (int)text[i] - '!' + 1;
		setCurrentFrame(frame);
		setX(fx);
		setY(fy);
		draw();
		if (widths[frame] == 0)
			widths[frame] = getWidth();
		fx += widths[frame] * getScale();

	}
}

void Font::Print_(int x, int y, std::string text) {
	int fx = x;
	int fy = y;

	for (int i = 0; i < text.length(); i++) {

		// char can be cast to int
		int frame = (int)text[i] - '!' + 1;
		setCurrentFrame(frame);
		setX(fx);
		setY(fy);
		draw();
		fx += getWidth() * getScale();

	}
}

int Font::getTotalWidth(std::string text) {

	int fx = 0;

	for (int i = 0; i < text.length(); i++) {

		int frame = (int)text[i] - '!' + 1;
		fx += widths[frame] * getScale();

	}

	return fx;
}

int Font::getTotalWidth_(std::string text) {

	int fx = 0;

	for (int i = 0; i < text.length(); i++) {
		fx += this->getWidth() * getScale();
	}

	return fx;
}

void Font::draw() {
	Image::draw();
}

void Font::update(float deltaTime) {

}