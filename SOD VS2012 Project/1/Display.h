/*
This source file is part of Sword of Destiny Game
Copyright (c) 2014 Pawel Podzus
File:        Display.h
*/

#ifndef DISPLAY_H
#define DISPLAY_H

#include "Global.h"

class ConsoleDisplay
{
	int width,height;
	std::vector<std::vector<char>> display;
	std::vector<std::vector<Global::colorType>> colours;
public:
	ConsoleDisplay(int displaySizeX, int displaySizeY);
	int getDisplayHeight();
	int getDisplayWidth();
	/*adds a new colored string to the array,
	replaces any contents already there*/
	void addString(std::string s, int positionX, int positionY, Global::colorType color = Global::colorType::cWHITE);
	/*changes the color of a string at position specified,
	takes modulo of 16 from the int to get the Global::colorType,
	does nothing if out of range*/
	void setColorAt(int positionX, int positionY, Global::colorType color = Global::colorType::cWHITE);
	void setLineColor(int positionY, Global::colorType color = Global::colorType::cWHITE);
	/*aligns text to right/left/center at specified y-position*/
	void alignLine(int positionY, Global::textAlign alignment);
	/*clears a ColString at position specified,
	does nothing if out of range*/
	void clearAt(int positionX, int positionY, bool clearColour = true, bool clearChar = true);
	void clearString(int positionY, bool clearColour = true, bool clearChar = true);
	/*empties the array*/
	void clear();
	/*prints all contents of the array*/
	void print();
};

#endif //DISPLAY_H