/*
This source file is part of Sword of Destiny Game
Copyright (c) 2014 Pawel Podzus
File:        GameHelp.h
*/

#ifndef GAMEHELP_H
#define GAMEHELP_H

#include "Global.h"

class GameCore;
class ConsoleDisplay;

class GameHelp
{
	ConsoleDisplay* display;
	std::vector<std::pair<std::string,std::string>> helpData;
	/*returns a help string with corresponding ID
	or MISSING:'ID' if not there*/
	bool getHelpString(std::string ID, std::string &s);
	void prepareHelpString(std::string ID);
	int callHelpDisplayString(std::string ID, int choiceMax, int choiceMin = -1);
public:
	GameHelp(ConsoleDisplay* display);
	/*enters help loop*/
	void callHelp();
};

#endif // !GAMEHELP_H
