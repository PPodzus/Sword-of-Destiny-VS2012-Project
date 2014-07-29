/*
This source file is part of Sword of Destiny Game
Copyright (c) 2014 Pawel Podzus
File:        GameSave.h
*/
#ifndef GAMESAVE_H
#define GAMESAVE_H

#include "Global.h"

class GameSave
{
	std::string saveName, filePath;
	int slot;
public:
	GameSave(std::string filePath, std::string saveName, int slot);
	std::string getFilePath();
	std::string getSaveName();
	int getSlot();
	~GameSave();
};

#endif // !GAMESAVE_H
