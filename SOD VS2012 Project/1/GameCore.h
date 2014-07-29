/*
This source file is part of Sword of Destiny Game
Copyright (c) 2014 Pawel Podzus
File:        GameCore.h
*/

#pragma once
#ifndef GAMECORE_H
#define GAMECORE_H

#include "Global.h"

class GameMap;
class MapTile;
class ConsoleDisplay;
class Inventory;
class Item;
class Character;
class Player;
class Statistics;
class ItemFunction;
class Band;
class GameSave;
class GameHelp;

//Main game class
class GameCore
{
private:
	MersenneTwister randomGenerator;
	bool inGame;
	Global::difficultyLevels difficulty;
	std::vector<std::string> startingItems[3][3];
	int minStats[3][3][4];

	MapPos playerStartingPos;
	MapPos SODLocation;
	MapPos SODLocHintRandomizer;
	Global::eventType lastEvent;

	ConsoleDisplay* display;
	GameMap* map;
	Statistics* stats;
	GameHelp* help;
	GameSave* savedGames[10];

	//used to give unique names to NPCs
	double npcCounter;
	std::vector<Character*> NPCs;
	std::vector<Band*> bands;
	Player* gPlayer;
	std::vector<ItemFunction*> functions;
	//works as a cache for all items
	Inventory* items;

	Global::functionType extractFuncType(std::string s);
	Global::mTerrain extractTerrainType(std::string s);
	Global::difficultyLevels extractDifficulty(std::string s);
	ItemFunction* getFunction(std::string name);
	
	//SAVE/LOAD FUNCTIONS
	void updateSaveFiles();
	std::string getSaveFileList();
	void startSaveSequence();
	bool saveGame(std::string saveName, int slot);
	std::string getSaveStringDifficulty();
	std::string getSaveStringMap();
	std::string getSaveStringMainLocs();
	std::string getSaveStringCharacters();
	std::string getSaveStringBands();
	std::string getSaveStringStats();
	bool startLoadSequence();
	void loadDifficulty(std::string s);
	void loadStatistics(std::string s);
	void loadWinItemLoc(std::string s);
	void loadWinItemRand(std::string s);
	void loadPlrStartLoc(std::string s);
	void loadPlayer(std::ifstream &file, int &currentLine);
	void loadNPCs(std::ifstream &file, int &currentLine);
	void loadBands(std::ifstream &file, int &currentLine);
	void loadMap(std::ifstream &file, int &currentLine);
	/*loads game from slot inidcated
	@saveSlot - save slot index to load*/
	void loadGame(int saveSlot);

	//INITIALIZATION FUNCTIONS
	/*loads all functions from functions.ini file*/
	void loadFunctions();
	/*loads all items from data folder*/
	void loadItems();
	/*loads help data*/
	void loadHelp();
	/*loads items to give to player based on difficluty from gamedifficulty.ini*/
	void loadDifficultyData();
	/**/
	int getNextRandInt();
	//NEW GAME INITIALIZATION
	/*prepares the main menu display*/
	void loadMainMenu();
	/*initiates pre-new-game sequence including difficulty selection and player name selection*/
	void selectNewGame();
	/*creates a new random game with named player*/
	void newGame(std::string charName);
	/*TO BE REBUILD FOR LOADING GAMES*/
	void setMap(std::string map_file_path);
	/*creates a fresh randomly generated map*/
	void setMap();
	/*sets the game difficulty level and generates player's band and food, to be called after newGame*/
	void setDifficulty(int playerChoice);
	/*fills the player's band with allies based of difficulty*/
	void createPlayerBand();
	//IN GAME FUNCTIONALITY
	/*prepares turn summary including all details and current menu choices based on player position*/
	int loadGameTurnState(std::string eventSummary, Global::colorType eventColor, std::string huntingSummary, Global::colorType huntingColor, std::string foodSummary, Global::colorType foodColor);
	/*sets the player's team status to rested*/
	void restTurn();
	/*initates travel sequence, including random events and fights*/
	void travel(std::string &eventSummary, Global::colorType &eventSummaryColor);
	/*initates player inventory management sequence*/
	void managePlayerInventory();
	/*initates band management sequence*/
	void managePlayerBand();
	/*prints the map with current game state to the screen*/
	void printGameMapState();
	/*initiates hunt sequence*/
	void hunt(std::string &huntingSummary, Global::colorType &huntingSummaryColor);
	/*initiates the shopping sequence, allowing player to buy food for his band*/
	void startShopping();
	/*initiates pause menu sus menu, allowing player to save/load games, look up statistics and exit the game*/
	bool pauseMenu();
	/*generates a random scenario for new game, including random position for quest items and player position*/
	void generateScenario();
	/*prints out all statistics to the screen*/
	void printStatistics();
	/*sets target map tile as bool
	@position - x/y map position to mark
	@whats_the_status - sets tile to visited if true*/
	void setVisitedAtTile(MapPos position,bool whats_the_status);
	/*returns player starting position for this game*/
	MapPos getPlrStartPos();
	/*returns a random event to be used as even for this turn*/
	Global::eventType generateEvent();
	/*initiates the event sequence
	@newEvent - event to happens this turn
	returns false if player dies*/
	bool executeEvent(Global::eventType &newEvent);
	/*check if bands exists at target location, changes event appropriately if it does
	generates a new one if it doesnt
	@position - position to check
	@newEvent - event to happen this turn, changes if a band already exists*/
	Band* generateOrGetBandAt(MapPos position, Global::eventType &newEvent);
	/*goes through all NPCs and deletes any without a band*/
	void defragmentBands();
	/*initiates the fight sequence
	@enemyBand - band to have a fight with
	returns false if player dies*/
	bool executeFight(Band* enemyBand);
	int calculateDamage(Band* playerBand, Band* enemyBand, bool playersTurn);
	void loadInitialFightScreen(Band* enemyBand);
	void loadFightBandStats(Band* playerBand, Band* enemyBand);
	bool tryPersuade(Band* playerBand, Band* enemyBand);
	bool tryRun(Band* playerBand, Band* enemyBand, std::string &fightSummary);
	void inFightDrinkPotion();
	int advanceFight(Band* playerBand, Band* enemyBand, bool playersTurn, std::string &fightSummary);
	/*creates a string containing current state of all player's band members' inventories*/
	std::string printPlrBandInventory();
	/*gives item to target inventory from global item database
	@index - index of the item in inventory
	@newOwner - target inventory to copy the item to*/
	void copyItem(int index, Inventory* newOwner);
	/*gives item to target inventory from global item database
	@itemID - item ID to find target item
	@newOwner - target inventory to copy the item to*/
	void copyItem(std::string itemID, Inventory* newOwner);
	/*checks if the player met win conditions in game*/
	bool checkWin();
	/*displays the victory message and resets the game*/
	void win();
	/*displays the defeat message and resets the game*/
	void lose();
	/*resets the game to main-menu status*/
	void reset();
public:
	GameCore(ConsoleDisplay* display);
	/*loads all item, function and save data*/
	void initialSetup();
	/*enters loop requesting input between min/max values from player
	@minValue & maxValue - requests value in minValue =< input =< maxValue format
	@display - re-prints the display with each unsuccessfull loop a display is passed
	returns value within bounds, keeps looping otherwise*/
	static int getPlayerInput(int minValue, int maxValue, bool NtoCancel, ConsoleDisplay* display = NULL);
	/*waits until players presses enter*/
	static void waitForEnter();
	/*starts the main game loop*/
	void go();
};

#endif //!GAMECORE_H