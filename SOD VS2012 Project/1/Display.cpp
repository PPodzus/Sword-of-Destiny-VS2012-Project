#include "Display.h"

ConsoleDisplay::ConsoleDisplay(int displaySizeX, int displaySizeY)
{
	width = displaySizeX;
	height = displaySizeY;
	display.resize(displaySizeY);
	colours.resize(displaySizeY);
	for(int i=0;i<displaySizeY;i++){
		display[i].resize(displaySizeX);
		colours[i].resize(displaySizeX);
		for(int j=0;j<displaySizeX;j++)
			colours[i][j] = Global::colorType::cWHITE;
	}
}
int ConsoleDisplay::getDisplayHeight()
{
	return display.size();
}
int ConsoleDisplay::getDisplayWidth()
{
	return display[0].size();
}
void ConsoleDisplay::addString(std::string s, int positionX, int positionY, Global::colorType color)
{
	if(positionY < height && positionX < width && positionY >= 0 && positionX >= 0){
		int curPosX = positionX, curPosY = positionY;
		char c;
		while(s != ""){
			c = s[0];
			if(curPosX+1 > display[0].size() || c == '\n'){
				curPosY++;
				if(s[0] == '\n')
					curPosX = positionX;
				else
					curPosX = 0;
				if(curPosY >= display.size())
					return;
			}
			if(c != '\n'){
				display[curPosY][curPosX] = c;
				colours[curPosY][curPosX] = color;
				curPosX++;
			}
			s = s.substr(1,s.size());
		}
	}
}
void ConsoleDisplay::setColorAt(int positionX, int positionY, Global::colorType color)
{
	if(positionY < height && positionX <= width && positionY >= 0 && positionX >= 0){
		colours[positionY][positionX] = color;
	}
}
void ConsoleDisplay::setLineColor(int positionY, Global::colorType color)
{
	if(positionY < height && positionY >= 0)
		for(int i=0;i<width;i++)
			colours[positionY][i] = color;
}
void ConsoleDisplay::alignLine(int positionY, Global::textAlign alignment)
{
	if(positionY < height  && positionY >= 0 ){
		std::vector<char> newLine;
		std::vector<Global::colorType> newColLine;
		newColLine.resize(width);
		newLine.resize(width);
		int left = 0, right = 0, difference;
		for(int i=0;i<width;i++)
			if(display[positionY][i] != ' '){
				left = i;
				break;
			}

		for(int i=width-1;i>=0;i--)
			if(display[positionY][i] != ' '){
				right = i;
				break;
			}

		switch(alignment){
		case Global::textAlign::tLEFT:
			difference = left;
			for(int i=0;i<width;i++){
				if(i+difference < width-1){
					newLine[i] = display[positionY][i+difference];
					newColLine[i] = colours[positionY][i+difference];
				}
			}
			break;
		case Global::textAlign::tRIGHT:
			difference = width-right;
			for(int i=width-1;i>=0;i--){
				if(i+difference >= 0 && i+difference < width){
					newLine[i] = display[positionY][i+difference];
					newColLine[i] = colours[positionY][i+difference];
				}
			}
			break;
		case Global::textAlign::tCENTER:
			difference =  ((right - left)/2 + left) - width/2;
			if(difference > 0)
				for(int i=0;i<width;i++){
					if(i+difference < width-1){
						newLine[i] = display[positionY][i+difference];
						newColLine[i] = colours[positionY][i+difference];
					}
				}
			else
				for(int i=width-1;i>=0;i--){
					if(i+difference >= 0 && i+difference < width){
						newLine[i] = display[positionY][i+difference];
						newColLine[i] = colours[positionY][i+difference];
					}
				}
				break;
		}
		display[positionY] = newLine;
		colours[positionY] = newColLine;
	}
}
void ConsoleDisplay::clearAt(int positionX, int positionY, bool clearColour, bool clearChar)
{
	if(positionY < height  && positionX < width && positionY >= 0 && positionX >= 0){
		if(clearChar)
			colours[positionY][positionX] = Global::colorType::cWHITE;
		if(clearChar)
			display[positionY][positionX] = ' ';
	}
}
void ConsoleDisplay::clearString(int positionY, bool clearColour, bool clearChar)
{
	if(positionY < height && positionY >= 0){
		for(int i=0;i<display[positionY].size();i++){
			if(clearChar)
				colours[positionY][i] = Global::colorType::cWHITE;
			if(clearChar)
				display[positionY][i] = ' ';
		}
	}
}
void ConsoleDisplay::clear()
{
	for(int i=0;i<display.size();i++){
		for(int j=0;j<display[i].size();j++){
			display[i][j] = ' ';
			colours[i][j] = Global::colorType::cWHITE;
		}
	}
}
void ConsoleDisplay::print()
{
	for(int i=0;i<display.size();i++)
		std::cout << std::endl;
	for(int i=0;i<display.size();i++){
		for(int j=0;j<display[i].size();j++){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colours[i][j]);
			std::cout << display[i][j];
		}
	}
}