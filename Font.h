// Module:			Gameplay Programming
// Assignment 1:	Pixl

#pragma once
#ifndef _FONT_H_
#define _FONT_H_

#include "entity.h"

class Font : public Entity {
public:
	Font();

	bool loadTextData(std::string fileName);		// text data (widths) is stored in dat files
	void Print(int x, int y, std::string text);		// prints the text on screen give the coordinates
	void Print_(int x, int y, std::string text);		// prints the text on screen give the coordinates
	virtual void draw();
	virtual bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void update(float deltaTime);

	int getTotalWidth(std::string text);
	int getTotalWidth_(std::string text);

private:

	int				widths[256];


};

#endif