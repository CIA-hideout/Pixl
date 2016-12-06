// Module:			Gameplay Programming
// Assignment 1:	Pixl
// Student Name:	Elcoms Khang	(S10157558A)
//					Lin Lue			(S10158175E)
//					Amos Tan		(S10158017D)

#pragma once
#ifndef _FONT_H_
#define _FONT_H_

#include "entity.h"

class Font : public Entity {
public:
	Font();

	bool loadTextData(std::string fileName);
	bool loadTextSprite(TextureManager* texture);
	void Print(int x, int y, std::string text);
	virtual void draw();
	virtual bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void update(float deltaTime);

	int getTotalWidth(std::string text);

private:

	int				widths[256];


};

#endif