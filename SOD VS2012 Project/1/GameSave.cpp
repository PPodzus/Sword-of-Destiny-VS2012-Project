#include "GameSave.h"

GameSave::GameSave(std::string filePath, std::string saveName, int slot)
{
	this->filePath = filePath;
	this->saveName = saveName;
	this->slot = slot;
}
std::string GameSave::getFilePath()
{
	return filePath;
}
int GameSave::getSlot()
{
	return slot;
}
std::string GameSave::getSaveName()
{
	return saveName;
}
GameSave::~GameSave()
{

}