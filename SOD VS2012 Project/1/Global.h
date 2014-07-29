/*
This source file is part of Sword of Destiny Game
Copyright (c) 2014 Pawel Podzus
File:        Global.h
*/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <time.h>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <Windows.h>
#include <assert.h>
#include <iomanip> 

#include "dirent.h"
#include "MersenneTwister.h"
//-----------------------------
//Generic Functions------------
//-----------------------------

class MapPos
{
public:
	int x,y;
	MapPos(void){x = 0, y = 0;}
	MapPos(int x_pos, int y_pos){x = x_pos; y = y_pos;}
};

namespace Global
{
	enum colorType{cBLACK, cBLUE, cGREEN, cCYAN, cRED, cMAGENTA, cBROWN, cGREY_LIGHT, cGREY_DARK, cBLUE_LIGHT, cGREEN_LIGHT, cCYAN_LIGHT, cRED_LIGHT, cMAGNETA_LIGHT, cYELLOW, cWHITE};
	enum textAlign{tLEFT, tCENTER, tRIGHT};

	enum mTerrain{TILE_COUNTRYSIDE, TILE_FARMLAND, TILE_FOREST, TILE_HILL, TILE_MOUNTAIN, TILE_DESERT, TILE_SWAMP, TILE_COUNT};
	enum eventType{EVENT_LOST, EVENT_MEET_SWORDSMAN, EVENT_MEET_MERCENARY, EVENT_MEET_DWARF, EVENT_MEET_FARMER, EVENT_NOTHING};
	enum direction{DIR_NORTH, DIR_SOUTH, DIR_EAST, DIR_WEST, DIR_NULL};
	enum functionType{fMODIFY_CHARISMA, fMODFIY_COMBATSTRENGTH, fMODFIY_ENDURANCE, fMODFIY_WIT, fFOOD, fWIN_ITEM, fHINT_ITEM, fDROPMONEY, fWOUND, fHEAL};
	enum difficultyLevels{dEASY, dMEDIUM, dHARD};

	template <typename T> std::string ntos(T number)
	{
		std::stringstream ss;//create a stringstream
		ss << number;//add number to the stream
		return ss.str();//return a string with the contents of the stream
	}
	bool isNumber(const std::string s);

	//String to numeric
	template <typename T> bool ston(std::string s, T &number)
	{
		//handling negative numbers
		if(s.at(0) == '-')
		{
			if(isNumber(s.substr(1,s.length()-1)))
			{
				number = std::stoi(s);
				return true;
			}
		}
		if(isNumber(s))
		{
			number = std::stoi(s);
			return true;
		}
		return false;
	}
	int getRandomInt();
	int getRandomInt(int maxValue,int minValue = 0);
	std::string ToStringConvert(int number);
	std::string ToStringConvert(float number);
	void toUpperConvert(std::string &s);
	std::string toUpperString(std::string s);
	std::string noWhiteSpaceString(std::string s, bool trimLeft, bool trimRight);
	void noWhiteSpaceConvert(std::string &s);

	bool getLeLine(std::ifstream &file, std::string &string);
	bool getLeLine(std::ifstream &file, std::string &string,int &lineCounter);
	std::string getLeVarVal(std::string string);
	std::string getLeVarName(std::string string);
	void convertToLeVarVal(std::string &string);
	void convertToLeVarName(std::string &string);

	template <typename T> int roundNumber(T toBeRounded){return std::floor(toBeRounded-0.5);}

	std::string printSpace(int amount);
	void clearBuffer();
}
#endif // !GLOBAL_H


