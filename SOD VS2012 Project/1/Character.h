/*
This source file is part of Sword of Destiny Game
Copyright (c) 2014 Pawel Podzus
File:        Character.h
*/
#ifndef PLAYER_H
#define PLAYER_H

#include "Global.h"

class GameCore;
class ConsoleDisplay;
class Inventory;
class Item;
class ItemFunction;
class Character;
class GameMap;

class Band
{
	std::string bandName;
	std::vector<Character*> members;
	MapPos myPosition;
	Global::mTerrain myTerrainTile;
	Global::eventType bandType;
public:
	Band();
	Band(std::vector<Character*> &bandMembers, MapPos position, Global::mTerrain terrain);
	Band(std::string name);
	int getBandSize();
	bool hasMembers();
	/*adds a member to the band unless it's already a part of it*/
	bool addMember(Character* newMemeber);
	/*removes a member from the team if he's part of it and defragments the vector*/
	bool removeMember(Character* member, bool killMember = false);
	/*removes a member from place X and defragments the vector*/
	bool removeMember(int index, bool killMember = false);
	/*returns a character at position *index*
	returns NULL if out of range*/
	Character* getMember(int index);
	/*returns a character with name passed
	returns NULL if not found*/
	Character* getMember(std::string name);
	bool checkBandHealthy();
	Global::eventType getBandType();
	void setBandType(Global::eventType newType);
	std::string getBandName();
	void generateBandName();
	void dissolveBand();

	~Band();
};

class Character
{
protected:
	std::string name;
	std::string charID;

	//attributes
	static const int maxAttribute = 10;
	int endurance;
	int combatStrength;
	int wounds;
	int charisma;
	int wit;
	int winItemIndex;
	int myFood;
	bool  hasWinItem, hasHintItem;
	//bool hit;

	MapPos currentPosition;
	Global::mTerrain currentTile;

	Band* myBand;
	Inventory* items;
	/*This function is used by trade function to display the items in inventories*/
	std::string displayTradeInventories(Character* tradeTarget);
public:
	Character(void);
	Character(int initial_endurance, int initial_combatSTR, int initial_charisma, int initial_wit, int initial_money);
	//used for loading characters from save files
	Character(int pEndurance, int pCombatStrength, int wounds, int pCharisma, int pWit, int pMoney);
	virtual ~Character();
	std::string getName();
	std::string getID();
	int getEndurance();
	int getCombatStrength();
	int getWounds();
	int getCharisma();
	int getWit();
	int getAlliesCount();
	int getFoodCount();
	MapPos getPosition();
	Inventory* getInventory();
	Band* getBand();

	bool checkHealth();
	void updateEndurance(int enduranceModifier);
	void updateCombatStrength(int combatStrengthModifier);
	void updateWounds(int woundsModifier);
	void updateCharisma(int charismaModifier);
	void updateWit(int witModifier);
	void updateWinItem(std::string winItemID);
	void updateAllies();
	
	void setPosition(MapPos position);
	void setPosition(int x, int y);
	void setTerrainTile(Global::mTerrain terrain);
	void setBand(Band* newBand);
	void setName(std::string newName);
	void setID(std::string newID);
	void trade(Character* tradeTarget, GameMap* map, ConsoleDisplay* display);
	void giveItemToTeamMember(Item* the_item, Character* targetTeamMember);
	void receiveItemFromMember(Item* the_item);

	bool scanForWinItem();
	bool scanForHintItem();

	bool checkWinItem();
	bool checkHintItem();

	void printInventory(ConsoleDisplay* display);
	std::string getCharStatsString();
	void kill();
	//combines all of character's attributes into a save compatible format
	std::string exportToString();
};

class Player : public Character
{
	//used to indicate which action player has executed this round
	Global::direction moveDirection;
	MapPos nextPosition;
	bool rested, teamHealthy;
	void consumeFoodHasEnough(int &foodConsumed);
	bool consumeFoodNotEnough(int &foodConsumed, int &membersHungry, std::string &foodSummary, Global::colorType &foodColor);
public:

	Player();
	//used to load player from save file
	Player(int pEndurance, int pCombatStrength, int pWounds, int pCharisma, int pWit, int pMoney);
	~Player();
	//used to move player according to moveDirection
	void move();
	//used to directly control player's next movement
	void move(Global::direction nextDirection);
	//in place on player to make him being the last one to starve easier
	bool consumeDailyFood(std::string &infoString, Global::colorType &infoStringColor);
	int getAllFood();
	//std::string getWinItemID(){return winItemID;}

	void searchArea(GameMap* map, ConsoleDisplay* display);
	void setRested(bool rested_status);
	void setTeamHealthy(bool isHealthy);
	bool checkRested();
	bool checkTeamHealthy();
	MapPos getNextPosition();
	Global::direction getMoveDirection();
	std::string getMoveDirAsString();
	bool setMoveDirection(Global::direction newDirection);

	void kill();
	std::string exportPlayerToString();
};

class Swordsman : public Character
{
public:
	Swordsman();
	Swordsman(std::string newName);
	Swordsman(int sEndurance, int sCombatStrength, int sWounds, int sCharisma, int sWit, int sMoney);
	~Swordsman();
	void kill();
};

class Dwarf : public Character
{
public:
	Dwarf();
	Dwarf(std::string newName);
	Dwarf(int dEndurance, int dCombatStrength, int dWounds, int dCharisma, int dWit, int dMoney);
	~Dwarf();
	void kill();
};

class Mercenary : public Character
{
public:
	Mercenary();
	Mercenary(std::string newName);
	Mercenary(int mEndurance, int mCombatStrength, int mWounds, int mCharisma, int mWit, int mMoney);
	~Mercenary();
	void kill();
};

class IrateFarmer : public Character
{
public:
	IrateFarmer();
	IrateFarmer(std::string newName);
	IrateFarmer(int iEndurance, int iCombatStrength, int iWounds, int iCharisma, int iWit, int iMoney);
	~IrateFarmer();
	void kill();
};

#endif // !PLAYER_H
