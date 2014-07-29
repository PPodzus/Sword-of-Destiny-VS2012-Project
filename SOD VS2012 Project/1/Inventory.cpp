#include "Inventory.h"
#include "Character.h"

Item::Item(std::string itemName, std::string itemID, int itemWeight,Inventory* itemOwner, std::vector<std::pair<int, Global::mTerrain>> &itemDropData,
		 ItemFunction* function, std::string itemDescription)
{
	name = itemName;
	ID = itemID;
	weight = itemWeight;
	owner = itemOwner;
	this->function = function;
	//copy all contents from itemDropData to dropData
	for(int i=0;i<itemDropData.size();i++){
		dropData.push_back(itemDropData[i]);
		if(dropData[i].second == Global::mTerrain::TILE_COUNT){
			defaultDropIndex = i;
			if(dropData[i].first == 0)
				isDropable = false;
		}
	}
	isDropable = true;
	itemOwner->addItemToInventory(this);
	description = itemDescription;
}
Item::Item(const Item &obj)
{
	this->name = obj.name;
	this->ID = obj.ID;
	this->description = obj.description;
	this->weight = obj.weight;
	this->isDropable = obj.isDropable;
	this->owner = obj.owner;
	this->function = new ItemFunction(*obj.function);
	for(int i=0;i<obj.dropData.size();i++)
		this->dropData.push_back(obj.dropData[i]);
}
void Item::addItemFunction(ItemFunction* function)
{
	function = new ItemFunction(*function);
}
int Item::use()
{
	return function->onUse();
}
bool Item::onPickup()
{
	return function->onPickup();
}
bool Item::onDrop()
{
	return function->onDropOrDestroy();
}
std::string Item::getName()
{
	return name;
}
std::string Item::getID()
{
	return ID;
}
std::string Item::getDescription()
{
	return description;
}
int Item::getWeight()
{
	return weight;
}
int Item::getDropChance()
{
	return dropData[defaultDropIndex].first;
}
Inventory* Item::getOwner()
{
	return owner;
}
void Item::changeOwner(Inventory* newOwner)
{
	owner = newOwner;
	function->setParent(newOwner);
}
ItemFunction* Item::getFunction()
{
	return function;
}
bool Item::checkIfDropable()
{
	return isDropable;
}
int Item::getDropChanceAt(Global::mTerrain terrainModifier)
{
	for(int i=0;i<dropData.size();i++){
		if(dropData[i].second == terrainModifier)
			return dropData[i].first;
	}
	return dropData[defaultDropIndex].first;
}
std::string Item::getItemDetails()
{
	std::string temp = "";
	temp.append("Name: " + name + "\nDescription: " + description + "\nWeight: " + Global::ntos(weight) + "\nQuest Item: ");
	bool isQuestItem = false;
	if(function->getFunctionType() == Global::functionType::fHINT_ITEM || function->getFunctionType() == Global::functionType::fWIN_ITEM)
		isQuestItem = true;
	if(isQuestItem)
		temp.append("Yes\n");
	else
		temp.append("No\n");

	return temp;
}
Item::~Item()
{
	function->onDropOrDestroy();
	function->~ItemFunction();
}

ItemFunction::ItemFunction(std::string name, Global::functionType type, std::string values)
{
	this->name = name;
	this->type = type;
	amountMin = 0;
	amountMax = 0;
	switch(type){
	case Global::functionType::fDROPMONEY:
		Global::ston(Global::noWhiteSpaceString(values.substr(0,values.find_first_of(" ")),true,true),amountMin);
		Global::ston(Global::noWhiteSpaceString(values.substr(values.find_first_of(" "),values.size()),true,true),amountMax);
		break;
	case Global::functionType::fHINT_ITEM:
		if(values == "(#@!56%&%$#!'[]\;/.,")
			hintCondition = 2;
		else{
			hintCondition = 1;
			hint = values;
		}
		break;
	case Global::functionType::fWIN_ITEM:
		if(values == "1")
			winCondition = 1;
		else if(values == "2")
			winCondition = 2;
		break;
	case Global::functionType::fFOOD:
	case Global::functionType::fHEAL:
	case Global::functionType::fMODFIY_COMBATSTRENGTH:
	case Global::functionType::fMODFIY_ENDURANCE:
	case Global::functionType::fMODFIY_WIT:
	case Global::functionType::fMODIFY_CHARISMA:
	case Global::functionType::fWOUND:
		Global::ston(Global::noWhiteSpaceString(values,true,true),amountMin);
		break;
	}
}
ItemFunction::ItemFunction(const ItemFunction &obj)
{
	parentInventory = NULL;
	this->name = obj.name;
	this->type = obj.type;
	this->amountMin = obj.amountMin; 
	this->amountMax = obj.amountMax;
	this->winCondition = obj.winCondition;
	this->hintCondition = obj.hintCondition;
	this->hint = obj.hint;
}
void ItemFunction::setParent(Inventory* newParent)
{
	parentInventory = newParent;
}
Global::functionType ItemFunction::getFunctionType()
{
	return type;
}
std::string ItemFunction::getFunctionID()
{
	return name;
}
std::string ItemFunction::getFunctionValue()
{
	std::string value = "";
	int goldAmount = 0;
	if(parentInventory->getOwner() != NULL){
		switch(type){
		case Global::functionType::fDROPMONEY:
			goldAmount = Global::getRandomInt()%(amountMax-amountMin) + amountMin;
			value.append(Global::ntos(goldAmount));
			break;
		case Global::functionType::fHINT_ITEM:
			if(hintCondition == 2)
				value.append(Global::ntos(hintCondition));
			else
				value.append(hint);
			break;
		case Global::functionType::fWIN_ITEM:
			value.append(Global::ntos(winCondition));
			break;
		case Global::functionType::fFOOD:
		case Global::functionType::fMODFIY_COMBATSTRENGTH:
		case Global::functionType::fMODFIY_ENDURANCE:
		case Global::functionType::fMODFIY_WIT:
		case Global::functionType::fMODIFY_CHARISMA:
		case Global::functionType::fWOUND:
		case Global::functionType::fHEAL:
			value.append(Global::ntos(amountMin));
			break;
		}
	}
	return value;
}
std::string ItemFunction::exportFunctionValues()
{
	return std::string(Global::ntos(amountMin) + " " + Global::ntos(amountMax));
}
void ItemFunction::overrideFunctionAmount(int amountMin, int amountMax)
{
	if(amountMin != -1)
		this->amountMin = amountMin;
	if(amountMax != -1)
		this->amountMax = amountMax;
}
bool ItemFunction::onPickup()
{
	if(parentInventory->getOwner() != NULL){
		int goldAmount = 0;
		switch(type){
		case Global::functionType::fDROPMONEY:
			goldAmount = Global::getRandomInt()%(amountMax-amountMin) + amountMin;
			parentInventory->modifyGold(goldAmount);
			return false;
			break;
		case Global::functionType::fHINT_ITEM:
			parentInventory->getOwner()->scanForHintItem();
			break;
		case Global::functionType::fWIN_ITEM:
			parentInventory->getOwner()->scanForWinItem();
			break;
		case Global::functionType::fMODFIY_COMBATSTRENGTH:
			parentInventory->getOwner()->updateCombatStrength(amountMin);
			break;
		case Global::functionType::fMODFIY_ENDURANCE:
			parentInventory->getOwner()->updateEndurance(amountMin);
			break;
		case Global::functionType::fMODFIY_WIT:
			parentInventory->getOwner()->updateWit(amountMin);
			break;
		case Global::functionType::fMODIFY_CHARISMA:
			parentInventory->getOwner()->updateCharisma(amountMin);
			break;
		case Global::functionType::fWOUND:
		case Global::functionType::fHEAL:
		case Global::functionType::fFOOD:
			break;
		}
	}
	return true;
}
int ItemFunction::onUse()
{
	switch(type){
	case Global::functionType::fDROPMONEY:
	case Global::functionType::fHINT_ITEM:
	case Global::functionType::fWIN_ITEM:
	case Global::functionType::fMODFIY_COMBATSTRENGTH:
	case Global::functionType::fMODFIY_ENDURANCE:
	case Global::functionType::fMODFIY_WIT:
	case Global::functionType::fMODIFY_CHARISMA:
		break;
	case Global::functionType::fFOOD:
		if(amountMin <= 0)
			return 1;
		amountMin -= 1;
		break;
	case Global::functionType::fWOUND:
		parentInventory->getOwner()->updateWounds(amountMin);
		amountMin = 0;
		return 1;
		break;
	case Global::functionType::fHEAL:
		parentInventory->getOwner()->updateWounds(-amountMin);
		amountMin = 0;
		return 1;
		break;
	}
	return 0;
}
bool ItemFunction::onDropOrDestroy()
{
	switch(type){
	case Global::functionType::fHINT_ITEM:
		return false;
		break;
	case Global::functionType::fWIN_ITEM:
		return false;
		break;
	case Global::functionType::fFOOD://food does nothing, consuming is handled by onuse
		break;
	case Global::functionType::fMODFIY_COMBATSTRENGTH:
		parentInventory->getOwner()->updateCombatStrength(-amountMin);
		break;
	case Global::functionType::fMODFIY_ENDURANCE:
		parentInventory->getOwner()->updateEndurance(-amountMin);
		break;
	case Global::functionType::fMODFIY_WIT:
		parentInventory->getOwner()->updateWit(-amountMin);
		break;
	case Global::functionType::fMODIFY_CHARISMA:
		parentInventory->getOwner()->updateCharisma(-amountMin);
		break;
	case Global::functionType::fDROPMONEY:
	case Global::functionType::fWOUND:
	case Global::functionType::fHEAL:
		break;
	}
	return true;
}
ItemFunction::~ItemFunction()
{

}

Inventory::Inventory(Character* newOwner)
{
	owner = newOwner;
	itemSlots.resize(0);
	totalWeight = 0;
	gold = 0;
}
Inventory::Inventory()
{
	itemSlots.resize(0);
	totalWeight = 0;
	gold = 0;
}
void Inventory::addItemToInventory(Item* newItem)
{
	itemSlots.push_back(newItem);
}
Item* Inventory::getItem(int index)
{
	return itemSlots.at(index);
}
Character* Inventory::getOwner()
{
	return owner;
}
void Inventory::setOwner(Character* myOwner)
{
	owner = myOwner;
}
short Inventory::getCurrentWeight()
{
	return totalWeight;
}
int Inventory::getInventorySize()
{
	return itemSlots.size();
}
int Inventory::getGold()
{
	return gold;
}
void Inventory::modifyGold(int goldModoficator)
{
	gold += goldModoficator;
	if(gold < 0)
		gold = 0;
}
void Inventory::setCurrentWeight(int currentWeight)
{
	totalWeight = currentWeight;
}
int Inventory::getItemIndex(std::string itemName)
{
	for(int i=0;i<getInventorySize();i++)
		if(getItem(i)->getName() == itemName)
			return i;
	return -1;
}
int Inventory::getItemIndexID(std::string itemID)
{
	for(int i=0;i<getInventorySize();i++)
		if(getItem(i)->getID() == itemID)
			return i;
	return -1;
}
void Inventory::copyItemToInventory(Item* copiedItem)
{
	if(itemSlots.size() < 10){
		Item* newItem;
		newItem = new Item(*copiedItem);
		itemSlots.push_back(newItem);
		newItem->changeOwner(this);
		if(this->owner != 0)
			newItem->getFunction()->onPickup();
		updateWeight();
		defragmentInventory();
	}
}
void Inventory::moveItemFromInventory(int index, Inventory* newOwner)
{
	//remove any passive modifiers
	Item* movedItem = itemSlots[index];
	//dereference for defragmenting
	itemSlots[index] = 0;
	defragmentInventory();
	updateWeight();
	if(this->owner != 0)
		if(!movedItem->getFunction()->onDropOrDestroy()){
			this->owner->scanForHintItem();
			this->owner->scanForWinItem();
		}
	newOwner->addItemToInventory(movedItem);
	movedItem->changeOwner(newOwner);
	//and them to the new owner
	if(this->owner != 0 && !movedItem->onPickup())
		newOwner->deleteItemFromInventory(newOwner->getItemIndexID(movedItem->getID()));
	newOwner->updateWeight();
	newOwner->defragmentInventory();	
}
void Inventory::deleteItemFromInventory(int index)
{ 
	//remove any passive modifiers
	if(this->owner != 0)
		itemSlots.at(index)->getFunction()->onDropOrDestroy();
	
	//delete item
	if(index < itemSlots.size()){
		delete itemSlots.at(index);
		itemSlots.at(index) = 0;
	}
	defragmentInventory();
}
void Inventory::defragmentInventory()
{
	std::vector<bool> slotBools;
	bool allSorted = false;
	for(int i=0;i<itemSlots.size();i++)
		slotBools.push_back(false);
	for(int i=0;i<itemSlots.size();i++)
		if(!(itemSlots.at(i) == 0))
			slotBools.at(i) = true;
	while(!allSorted)
	{
		int boolCount = 0,emptyCount = 0;
		for(int i=0;i<itemSlots.size();i++)
		{
			if(itemSlots.at(i) == 0 || slotBools.at(i) == false)
			{
				for(int j = i;j < itemSlots.size();j++)
				{
					if(j+1 < itemSlots.size())
					{
						itemSlots.at(j) = itemSlots.at(j+1);
						itemSlots.at(j+1) = 0;
						//check if we swapped for an actual item or not
						if(!(itemSlots.at(j) == 0))
							//make that slot true if we did
							slotBools.at(j) = true;
					}
					else
					{}
				}
			}
		}
		//check if all bools are correct
		int lastItem = -1, firstNonItem = -1;
		for(int i=0;i<slotBools.size();i++)
		{
			if(slotBools.at(i) == true)
				boolCount++;
			if(!itemSlots.at(i))
			{
				if(firstNonItem == -1)
					firstNonItem = i;
				emptyCount++;
			}
			if(lastItem < i)
				lastItem = i;
			
		}

		//set allSorted to true if they are
		if((boolCount == slotBools.size()) || ((boolCount + emptyCount) == slotBools.size()) || lastItem+1 == firstNonItem)
			allSorted = true;
	}
	//once defragmented, delete any 0 pointers from the ending
	for(int i = 0;i < itemSlots.size();i++)
		if(itemSlots.at(i) == 0)
			itemSlots.resize(i);
}
void Inventory::updateWeight()
{
	int itemsWeight = 0;
	for(int i=0;i<itemSlots.size();i++)
	{
		itemsWeight += itemSlots.at(i)->getWeight();
	}
	setCurrentWeight(itemsWeight);
}
bool Inventory::checkFull(int newItemWeight)
{
	if(itemSlots.size() == 10 || totalWeight+newItemWeight > 10)
		return true;
	else 
		return false;
}
Inventory::~Inventory()
{
	for(int i=0;i<itemSlots.size();i++)
	{
		itemSlots[i]->~Item();
	}
}