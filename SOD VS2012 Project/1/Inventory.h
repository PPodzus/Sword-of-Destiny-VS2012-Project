/*
This source file is part of Sword of Destiny Game
Copyright (c) 2014 Pawel Podzus
File:        Inventory.h
*/

#ifndef INVENTORY_H
#define INVENTORY_H

#include "Global.h"

class Character;
class Item;

class Inventory
{
private:
	std::vector<Item*> itemSlots;
	Character* owner;
	int totalWeight;
	int gold;
	
public:
	Inventory(Character* newOwner);
	Inventory();
	//copies the item to this inventory, so in effect we get 2 instances of same item in 2 inventories
	void copyItemToInventory(Item* copiedItem);
	//adds item pointer to items list, should not be used for item management, use move/copy instead
	void addItemToInventory(Item* newItem);
	//moves an item from one inventory to another, deletes the pointer
	void moveItemFromInventory(int index, Inventory* newOwner);
	void deleteItemFromInventory(int index);
	Item* getItem(int index);
	Character* getOwner();
	void setOwner(Character* myOwner);
	short getCurrentWeight();
	int getItemIndex(std::string itemName);
	int getItemIndexID(std::string itemID);
	int getInventorySize();
	int getGold();
	void modifyGold(int goldModoficator);
	void setCurrentWeight(int currentWeight);
	void updateWeight();
	//this function has built in optional check that can be used before adding new items to calculate total weight with the new item
	bool checkFull(int newItemWeight = 0);
	void defragmentInventory();

	~Inventory();
};

class ItemFunction
{
protected:
	Inventory* parentInventory;
	std::string name;
	Global::functionType type;
	int amountMin, amountMax, winCondition, hintCondition;
	std::string hint;
public:
	ItemFunction(std::string name, Global::functionType type, std::string values);
	ItemFunction(const ItemFunction &obj);
	std::string getFunctionID();
	Global::functionType getFunctionType();
	void setParent(Inventory* newParent);
	std::string getFunctionValue();
	std::string exportFunctionValues();
	void overrideFunctionAmount(int amountMin = -1, int amountMax = -1);
	bool onPickup();
	int onUse();
	bool onDropOrDestroy();
	~ItemFunction();
};

class Item
{
	std::string name;
	std::string ID;
	std::string description;
	int weight;

	ItemFunction* function;
	std::vector<std::pair<int, Global::mTerrain>> dropData;
	int defaultDropIndex;
	Inventory* owner;

	bool isDropable, isOwned;
	MapPos myPosition;
public:
	Item(std::string itemName, std::string itemID, int itemWeight,Inventory* itemOwner, std::vector<std::pair<int, Global::mTerrain>> &itemDropData,
		 ItemFunction* function, std::string itemDescription = ""); 
	Item(const Item &obj);
	int use();
	bool onPickup();
	bool onDrop();
	std::string getName();
	std::string getID();
	std::string getDescription();
	int getWeight();
	int getDropChance();
	int getDropChanceAt(Global::mTerrain terrainModifier);
	Inventory* getOwner();
	void changeOwner(Inventory* newOwner);
	void addItemFunction(ItemFunction* function);
	ItemFunction* getFunction();
	bool checkIfDropable();

	std::string getItemDetails();

	~Item();
};

#endif // !INVENTORY_H
