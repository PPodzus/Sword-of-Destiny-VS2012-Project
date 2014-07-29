#include "Character.h"
#include "Inventory.h"
#include "GameMap.h"
#include "Display.h"
#include "GameCore.h"

Band::Band()
{
	myPosition = MapPos(-1,-1);
	bandType = Global::eventType::EVENT_NOTHING;
	myTerrainTile = Global::mTerrain::TILE_COUNT;
}
Band::Band(std::string name)
{
	bandName = name;
}
Band::Band(std::vector<Character*> &bandMembers, MapPos position, Global::mTerrain terrain)
{
	myPosition = position;
	myTerrainTile = terrain;
	for(int i=0;i<bandMembers.size();i++){
		members.push_back(bandMembers.at(i));
		members.at(i)->setBand(this);
		members.at(i)->setPosition(position);
		members.at(i)->setTerrainTile(terrain);
	}
}
int Band::getBandSize()
{
	return members.size();
}
bool Band::hasMembers()
{
	if(members.size() > 0)
		return true;
	return false;

}
Global::eventType Band::getBandType()
{
	return bandType;
}
void Band::setBandType(Global::eventType newType)
{
	bandType = newType;
}
std::string Band::getBandName()
{
	return bandName;
}
void Band::generateBandName()
{
	//since only 1 band per tile is allowed, this name is complex enough to be unique
	std::string temp;
	switch(myTerrainTile)
	{
	case 0: temp="CountrySide"; break;
	case 1: temp="Farmland"; break;
	case 2: temp="Forest"; break;
	case 3: temp="Hill"; break;
	case 4: temp="Mountain"; break;
	case 5: temp="Desert"; break;
	case 6: temp="Swamp"; break;
	}
	temp.append("x").append(Global::ntos(myPosition.x)).append("y").append(Global::ntos(myPosition.y)).append("Band");
	bandName = temp;
}
bool Band::addMember(Character* newMemeber)
{
	for(int i=0;i<members.size();i++){
		if(members.at(i) == newMemeber)
			return false;
	}
	//if not, add new ally
	members.push_back(newMemeber);
	newMemeber->setBand(this);
	return true;
}
bool Band::removeMember(Character* member, bool killMember)
{
	//if we have no members, there's no1 to remove
	if(members.size() == 0)
		return false;

	//if there's just one member
	if(members.size() == 1 && members.at(0) == member){
		//if it is, delete it
		if(killMember)
			members[0]->setBand(NULL); // killed at defragment stage in GameCore, not accessible in-game from that point on anyway
		members.resize(0);
		return true;
	}

	//if more than one
	for(int i=0;i<members.size();i++){
		//find the ally to be removed
		if(members.at(i) == member){
			if(killMember)
				members[i]->setBand(NULL); // killed at defragment stage in GameCore, not accessible in-game from that point on anyway
			//replace it with the last one on members list
			members.at(i) = members.at(members.size()-1);
			//and resize the members count by 1 to delete the duplicate
			members.resize(members.size()-1);
			return true;
		}
	}
	//if you couldnt find the member, return false
	return false;
}
bool Band::removeMember(int index, bool killMember)
{
	//if index is equal to last position in the vector, just get rid of that last pointer
	if(members.size()-1 == index){
		if(killMember)
			members[index]->setBand(NULL); // killed at defragment stage in GameCore, not accessible in-game from that point on anyway
		members.resize(members.size()-1);
		return true;
	}
	//else, remove and defragment
	if(members.size()-1 > index){
		if(killMember)
			members[index]->setBand(NULL); // killed at defragment stage in GameCore, not accessible in-game from that point on anyway
		//replace it with the last one on members list
		members.at(index) = members.at(members.size()-1);
		//and resize the members count by 1 to delete the duplicate
		members.resize(members.size()-1);
		return true;
	}
	return false;
}
Band::~Band()
{
	for(int i=0;i<members.size();i++)
		members[i]->setBand(NULL);
}
Character* Band::getMember(int index)
{
	//if index is not bigger than the band size, return the member
	if(members.size()-1 >= index)
		return members.at(index);
	else
		return NULL;
}
Character* Band::getMember(std::string name)
{
	//browse through members 
	for(int i=0;i<members.size();i++)
	{
		//if it's name is the one we're looking for, return it
		if(members.at(i)->getName() == name)
		{
			return members.at(i);
		}
	}
	return NULL;
}
bool Band::checkBandHealthy()
{
	for(int i=0;i<members.size();i++)
		if(members[i]->getWounds() > 0)
			return false;
	return true;
}

Character::Character(void)
{
	MersenneTwister attributeGenerator;
	attributeGenerator.randomize(rand());
	int attrValue;
	do{
		endurance = (attributeGenerator.nextUInt()+rand()+10);
		if(endurance < 0)
			endurance *= (-1);
		endurance = endurance%10;
	}while(endurance <= 0);
	attributeGenerator.randomize(rand());
	do{
		combatStrength = (attributeGenerator.nextUInt()+rand()+10);
		if(combatStrength < 0)
			combatStrength *= (-1);
		combatStrength = combatStrength%10;
	}while(combatStrength <= 0);
	attributeGenerator.randomize(rand());
	do{
		charisma = (attributeGenerator.nextUInt()+rand()+10);
		if(charisma < 0)
			charisma *= (-1);
		charisma = charisma%10;
	}while(charisma <= 0);
	attributeGenerator.randomize(rand());
	do{
		wit = (attributeGenerator.nextUInt()+rand()+10);
		if(wit < 0)
			wit *= (-1);
		wit = wit%10;
	}while(wit <= 0);
	attributeGenerator.randomize(rand());
	items = new Inventory(this);
	int gold;
	do{

		gold = (attributeGenerator.nextUInt()+rand()+10);
		if(gold < 0)
			gold *= (-1);
		gold = gold%10;
	}while(gold <= 0);
	items->modifyGold(gold);
	wounds = 0;
	myFood = 0;

	hasHintItem = false;
	hasWinItem = false;
}
Character::Character(int initial_endurance, int initial_combatSTR, int initial_charisma, int initial_wit, int initial_money)
{
	endurance = initial_endurance;
	combatStrength = initial_combatSTR;
	charisma = initial_charisma;
	wit = initial_wit;
	items = new Inventory(this);
	items->modifyGold(initial_money);

	hasHintItem = false;
	hasWinItem = false;
}
Character::Character(int cEndurance, int cCombatStrength, int cWounds, int cCharisma, int cWit, int cMoney)
{
	endurance = cEndurance;
	combatStrength = cCombatStrength;
	wounds = cWounds;
	charisma = cCharisma;
	wit = cWit;
	items = new Inventory(this);
	hasHintItem = false;
	hasWinItem = false;
}
std::string Character::getName()
{
	return name;
}
std::string Character::getID()
{
	return charID;
}
MapPos Character::getPosition()
{
	return currentPosition;
}
Inventory* Character::getInventory()
{
	return items;
}
Band* Character::getBand()
{
	return myBand;
}
void Character::updateEndurance(int enduranceModifier)
{
	endurance += enduranceModifier;
}
void Character::updateCombatStrength(int combatStrengthModifier)
{
	combatStrength += combatStrengthModifier;
}
void Character::updateWounds(int woundsModifier)
{
	if(wounds+woundsModifier > 10)
		wounds = 10;
	else if(wounds+woundsModifier <0)
		wounds = 0;
	else
		wounds += woundsModifier;
}
void Character::updateCharisma(int charismaModifier)
{
	charisma += charismaModifier;
}
void Character::updateWit(int witModifier)
{
	wit += witModifier;
}
void Character::setPosition(MapPos position)
{
	currentPosition = position;
}
void Character::setPosition(int x, int y)
{
	currentPosition = MapPos(x,y);
}
void Character::setTerrainTile(Global::mTerrain terrain)
{
	currentTile = terrain;
}
void Character::setBand(Band* newBand)
{
	myBand = newBand;
}
void Character::setName(std::string newName)
{
	name = newName;
}
void Character::setID(std::string newID)
{
	charID = newID;
}
int Character::getEndurance()
{
	if(endurance > 10)
		return 10;
	else if(endurance < 0)
		return 0;
	else
		return endurance;
}
int Character::getCombatStrength()
{
	if(combatStrength > 10)
		return 10;
	else if(combatStrength < 0)
		return 0;
	else
		return combatStrength;
}
int Character::getWounds()
{
	if(wounds > 10)
		return 10;
	else if(wounds < 0)
		return 0;
	else
		return wounds;
}
int Character::getCharisma()
{
	if(charisma > 10)
		return 10;
	else if(charisma < 0)
		return 0;
	else
		return charisma;
}
int Character::getWit()
{
	if(wit > 10)
		return 10;
	else if(wit < 0)
		return 0;
	else
		return wit;
}
int Character::getAlliesCount()
{
	return myBand->getBandSize();
}
int Character::getFoodCount()
{
	return myFood;
}
void Character::updateWinItem(std::string winItemID)
{
	for(int i=0;i<items->getInventorySize();i++){
		if(items->getItem(i)->getID() == winItemID){
			if(items->getItem(i)->getOwner() == this->getInventory()){
				hasWinItem = true;
				//winItemInventoryIndex = i;
			}
		}
	}
}
std::string Character::exportToString()
{
	std::string temp = "";
	temp.append(charID).append(" ").append(Global::ntos(currentPosition.x)).append(" ").append(Global::ntos(currentPosition.y))
		.append(" ").append(Global::ntos(endurance)).append(" ").append(Global::ntos(combatStrength))
		.append(" ").append(Global::ntos(wounds)).append(" ").append(Global::ntos(charisma))
		.append(" ").append(Global::ntos(wit)).append(" ").append(Global::ntos(items->getGold())).append(" ")
		.append(" ").append(name + "\n");
	temp.append("  Items\n");
	for(int i=0;i<items->getInventorySize();i++){
		temp.append("    Item = ").append(items->getItem(i)->getID()).append(" ").append(items->getItem(i)->getFunction()->exportFunctionValues()).append("\n");
	}
	temp.append("  End\n");
	return temp;
}
void Character::trade(Character* tradeTarget, GameMap* map, ConsoleDisplay* display)
{
	Inventory* myInventory = items;
	Inventory* targetInventory = tradeTarget->getInventory();
	bool trading = true;
	int tradeMenuIn = 0;
	do{
		display->clear();
		display->addString(displayTradeInventories(tradeTarget),0,0);
		display->addString("[1] Take an item\n[2] Give an item\n[3] Drop Item\n[4] Finish",0,21);
		tradeMenuIn = GameCore::getPlayerInput(1,4,false,display);
		if(tradeMenuIn == 1 && myInventory->checkFull())
				MessageBox(NULL, "You cannot add any more items to this inventory!\n", "Inventory is full", MB_OK | MB_ICONEXCLAMATION);
		else if(tradeMenuIn == 2 && targetInventory->checkFull())
			MessageBox(NULL, "You cannot add any more items to this inventory!", std::string(tradeTarget->getName() + "'s inventory is full").c_str(), MB_OK | MB_ICONEXCLAMATION);
		else{
			int selectItemInInt = -65;
			std::string selectItemIn;
			switch(tradeMenuIn){
			case 1:
			case 2:{
				display->clear();
				display->addString(displayTradeInventories(tradeTarget),0,0);
				if(tradeMenuIn == 1)
					display->addString("Select item slot from the right (1-10), 'N' to cancel",0,display->getDisplayHeight()-1);
				else
					display->addString("Select item slot from the left (1-10), 'N' to cancel",0,display->getDisplayHeight()-1);
				selectItemInInt = GameCore::getPlayerInput(1,10,true,display);
				if(selectItemInInt == -1)
					break;
				if(tradeMenuIn == 1){
					if(targetInventory->getItem(selectItemInInt-1)->getID() == "TChest" || (targetInventory->getInventorySize() > selectItemInInt-1 &&
						!myInventory->checkFull(targetInventory->getItem(selectItemInInt-1)->getWeight())))
						targetInventory->moveItemFromInventory(selectItemInInt-1,myInventory);
				}
				else{
					if(myInventory->getInventorySize() > selectItemInInt-1 && !targetInventory->checkFull(myInventory->getItem(selectItemInInt-1)->getWeight()) && //if item within range
						((myInventory->getItem(selectItemInInt-1)->getFunction()->getFunctionType() == Global::functionType::fHINT_ITEM || //but if item is hint
						myInventory->getItem(selectItemInInt-1)->getFunction()->getFunctionType() == Global::functionType::fWIN_ITEM)&&  //or win item
							MessageBox(NULL, "This is a quest item, it's required to finish the game\nAre you sure you want to give it to the ally?"
							, "Inventory is full", MB_OKCANCEL | MB_ICONEXCLAMATION) == MB_OK)) //ask if he's sure
						myInventory->moveItemFromInventory(selectItemInInt-1,targetInventory);
				}
				}break;
			case 3:{
				display->clear();
				display->addString(displayTradeInventories(tradeTarget),0,0);
				display->addString("Select item slot from the left to drop item (1-" + Global::ntos(myInventory->getInventorySize()) + "), 'N' to cancel",0,display->getDisplayHeight()-1);
				selectItemInInt = GameCore::getPlayerInput(1,myInventory->getInventorySize(),true,display);
				if(selectItemInInt == -1)
					break;
				else //because we can only control player's team, player will always be member 0
					myInventory->moveItemFromInventory(selectItemInInt-1,map->getInventoryAt(static_cast<Player*>(myBand->getMember(0))->getPosition()));
				}break;
			case 4:	trading = false; break;
			}
		}
	}while(trading);
}
std::string Character::displayTradeInventories(Character* tradeTarget)
{
	std::string toBeDisplayed = "";
	Inventory* targetInventory = tradeTarget->getInventory();
	std::string charItemName,targetItemName;
	toBeDisplayed = name + Global::printSpace(80-(name.size()+tradeTarget->getName().size())) + tradeTarget->getName() + "\n";
	//update items to display
	for(int i=0;i<10;i++){
		//if we have items in inventory, display them
		if(i < items->getInventorySize()){
			charItemName = "[" + Global::ntos(i+1) + "] ";
			if(i < 9)
				charItemName.append(" ");
			charItemName.append(items->getItem(i)->getName());
		}
		else{
			//display empty slot
			charItemName = "[" + Global::ntos(i+1) + "]";
			if(i < 9)
				charItemName.append(" ");
			charItemName.append(" ---");
		}
		//same for target inventory
		if(i < targetInventory->getInventorySize()){
			targetItemName = targetInventory->getItem(i)->getName();
			if(i < 9)
				targetItemName.append(" ");
			targetItemName.append(" [" + Global::ntos(i+1) + "]");
		}
		else{
			targetItemName = "--- ";
			if(i < 9)
				targetItemName.append(" ");
			targetItemName.append("[" + Global::ntos(i+1) + "]");
		}
		toBeDisplayed.append(charItemName).append(Global::printSpace(80-(charItemName.size()+targetItemName.size()))).append(targetItemName);
	}
	toBeDisplayed.append("Gold: " + Global::ntos(items->getGold()) + "\n");
	toBeDisplayed.append("\nTotal Weight" + Global::printSpace(56) + "Total Weight\n" + Global::ntos(items->getCurrentWeight()) + Global::printSpace(76));
	if(items->getCurrentWeight() < 9)
		toBeDisplayed.append(Global::printSpace(1));
	if(targetInventory->getCurrentWeight() < 9)
		toBeDisplayed.append(Global::printSpace(1));
	toBeDisplayed.append(Global::ntos(targetInventory->getCurrentWeight()) + "\n\n");
	return toBeDisplayed;
}
bool Character::scanForWinItem()
{
	for(int j=0;j<items->getInventorySize();j++)
		if(items->getItem(j)->getFunction()->getFunctionType() == Global::functionType::fWIN_ITEM){
			hasWinItem = true;
			return true;
		}
	hasWinItem = false;
	return false;
}
bool Character::scanForHintItem()
{
	for(int j=0;j<items->getInventorySize();j++)
		if(items->getItem(j)->getFunction()->getFunctionType() == Global::functionType::fHINT_ITEM){
			hasHintItem = true;
			return true;
		}
	hasHintItem = false;
	return false;
}
void Character::printInventory(ConsoleDisplay* display)
{
	display->addString("Inventory",0,0);
	Global::colorType color;
	for(int i=0;i<10;i++){
		if(i%2 == 0)
			color = Global::colorType::cWHITE;
		else
			color = Global::colorType::cGREY_DARK;
		display->addString("[" + Global::ntos(i+1) + "] ",0,i+1,color);
		if(i<items->getInventorySize())
			display->addString(items->getItem(i)->getName(),7,i+1,color);
		else
			display->addString("Empty Slot",7,i+1,color);
	}
}
std::string Character::getCharStatsString()
{
	std::string temp = "";
	//using functions to ensure we don't print stats over 10
	temp.append(name + Global::printSpace(24-(name.size())));
	if(endurance < 10)
		temp.append(" ");
	temp.append(Global::ntos(getEndurance()));
	temp.append(Global::printSpace(10));
	if(combatStrength < 10)
		temp.append(" ");
	temp.append(Global::ntos(getCombatStrength()));
	temp.append(Global::printSpace(10));
	if(charisma < 10)
		temp.append(" ");
	temp.append(Global::ntos(getCharisma()));
	temp.append(Global::printSpace(6));
	if(wit < 10)
		temp.append(" ");
	temp.append(Global::ntos(getWit()));
	temp.append(Global::printSpace(5));
	if(wounds < 10)
		temp.append(" ");
	temp.append(Global::ntos(getWounds()));
	temp.append(Global::printSpace(7));
	temp.append(Global::ntos(items->getGold()));
	return temp;
}
bool Character::checkWinItem()
{
	if(hasWinItem)
		return true;
	return false;
}
bool Character::checkHintItem()
{
	if(hasHintItem)
		return true;
	return false;

}
void Character::kill()
{
	this->~Character();
}
Character::~Character()
{
	items->~Inventory();
}

Player::Player()
{
	charID = "Player";
	moveDirection = Global::direction::DIR_NULL;
	myBand = new Band("PlayerBand");
	myBand->addMember(this);
	rested = false;
	teamHealthy = false;
}
//used to load player from save file
Player::Player(int pEndurance, int pCombatStrength, int pWounds, int pCharisma, int pWit, int pMoney) :
	Character(pEndurance, pCombatStrength, pWounds, pCharisma, pWit, pMoney)
{
	charID = "Player";
	myBand = new Band("PlayerBand");
	myBand->addMember(this);
	rested = false;
}
Player::~Player()
{
	//std::cout << "Player destructor called" << std::endl;
}
void Player::setRested(bool rested_status)
{
	rested = rested_status;
}
bool Player::checkRested()
{
	if(rested)
		return true;
	return false;
}
MapPos Player::getNextPosition()
{
	return nextPosition;
}
Global::direction Player::getMoveDirection()
{
	return moveDirection;
}
std::string Player::getMoveDirAsString()
{
	switch(moveDirection){
	case 0: return "North"; break;
	case 1: return "South"; break;
	case 2: return "East";  break;
	case 3: return "West";  break;
	case 4: return "None";  break;
	}
}
void Player::move()
{
	switch(moveDirection){
	case 0:	currentPosition = MapPos(currentPosition.x,currentPosition.y-1); break;
	case 1: currentPosition = MapPos(currentPosition.x,currentPosition.y+1); break;
	case 2: currentPosition = MapPos(currentPosition.x+1,currentPosition.y); break;
	case 3: currentPosition = MapPos(currentPosition.x-1,currentPosition.y); break;
	case 4: /*nothing happens, we don't move*/ break;
	}

}
void Player::move(Global::direction nextDirection)
{
	switch(nextDirection){
	case 0: currentPosition = MapPos(currentPosition.x,currentPosition.y-1); break;
	case 1: currentPosition = MapPos(currentPosition.x,currentPosition.y+1); break;
	case 2: currentPosition = MapPos(currentPosition.x+1,currentPosition.y); break;
	case 3: currentPosition = MapPos(currentPosition.x-1,currentPosition.y); break;
	case 4: /*nothing happens, we don't move*/ break;
	}
}
bool Player::setMoveDirection(Global::direction newDirection)
{
	switch(newDirection){
		case 0:
			if(currentPosition.y-1 >= 0){
				nextPosition.x = currentPosition.x; nextPosition.y = currentPosition.y-1;
				moveDirection = newDirection;
				return true;
			}
			else
				return false;
		break;
		case 1:
			if(currentPosition.y+1 < 20){
				nextPosition.x = currentPosition.x; nextPosition.y = currentPosition.y+1;
				moveDirection = newDirection;
				return true;
			}
			else
				return false;
		break;
		case 2:
			if(currentPosition.x+1 < 20){
				nextPosition.x = currentPosition.x+1; nextPosition.y = currentPosition.y;
				moveDirection = newDirection;
				return true;
			}
			else
				return false;
		break;
		case 3:
			if(currentPosition.x-1 >= 0){
				nextPosition.x = currentPosition.x-1; nextPosition.y = currentPosition.y;
				moveDirection = newDirection;
				return true;
			}
			else
				return false;
		break;
		case 4:
			nextPosition.x = currentPosition.x; nextPosition.y = currentPosition.y;
			moveDirection = newDirection;
		break;

	}
}
std::string Player::exportPlayerToString()
{
	std::string temp = "  ";
	temp.append(exportToString());
	if(rested)
		temp.append(" Yes");
	else temp.append(" No");
	return temp;
}
void Player::searchArea(GameMap* map, ConsoleDisplay* display)
{
	Inventory* tileInventory = map->getInventoryAt(getPosition());
	int averageWit = 0, playerBandSize = myBand->getBandSize(), itemFindChance;
	float averageWitFloat = 0;
	for(int i=0;i<playerBandSize;i++){
		averageWitFloat += myBand->getMember(i)->getWit();
	}
	averageWitFloat = averageWitFloat/playerBandSize;
	averageWit = Global::roundNumber(averageWitFloat);
	Character* ItemsFoundOwner = new Character();
	//used in trade function
	ItemsFoundOwner->setName("Items Found");
	Inventory* itemsFound = ItemsFoundOwner->getInventory();
	for(int i = 0;i<tileInventory->getInventorySize();i++){
		itemFindChance = Global::getRandomInt() + rand();
		if(itemFindChance < 0 )
			itemFindChance *= (-1);
		itemFindChance = itemFindChance%100;
		int multiplier = 0;
		do{
			multiplier = Global::getRandomInt() + rand();
			if(multiplier < 0 )
				multiplier *= (-1);
			itemFindChance = itemFindChance%15;
		}while(multiplier < 11);
		//HintItem and WinItem are always found
		if(tileInventory->getItem(i)->getFunction()->getFunctionType() == Global::functionType::fWIN_ITEM &&
			tileInventory->getItem(i)->getFunction()->getFunctionType() == Global::functionType::fHINT_ITEM)
			tileInventory->moveItemFromInventory(i,itemsFound);
		else if((averageWit*multiplier) > itemFindChance)
			tileInventory->moveItemFromInventory(i,itemsFound);
	}
	trade(ItemsFoundOwner, map, display);
	//if player has dropped any items, add them to tile's inventory
	for(int i=0;i<itemsFound->getInventorySize();i++){
		bool gotThisItem = false;
		for(int j=0;j<tileInventory->getInventorySize();j++)
			if(itemsFound->getItem(i) == tileInventory->getItem(j))
				gotThisItem = true;
		if(!gotThisItem)
			itemsFound->moveItemFromInventory(i,tileInventory);
	}
}
void Player::setTeamHealthy(bool isHealthy)
{
	teamHealthy = isHealthy;
}
bool Player::checkTeamHealthy()
{
	return teamHealthy;
}
void Player::consumeFoodHasEnough(int &foodConsumed)
{	
	int membersRemaining = myBand->getBandSize();
	setTeamHealthy(true);
	while(membersRemaining > 0){
		for(int i=0;i<myBand->getBandSize();i++){
			for(int j=0;j<myBand->getMember(i)->getInventory()->getInventorySize();j++){
				if(myBand->getMember(i)->getInventory()->getItem(j)->getFunction()->getFunctionType() == Global::functionType::fFOOD){
					if(myBand->getMember(i)->getInventory()->getItem(j)->use() == 0){
						membersRemaining--;
						foodConsumed++;
						if(membersRemaining == 0)
							break;
					}
				}
			}
			if(membersRemaining == 0)
				break;
		}
	}
}
bool Player::consumeFoodNotEnough(int &foodConsumed, int &membersHungry, std::string &foodSummary, Global::colorType &foodColor)
{
	int membersRemaining = myBand->getBandSize();
	setTeamHealthy(false);
	for(int i=0;i<myBand->getBandSize();i++){
		for(int j=0;j<myBand->getMember(i)->getInventory()->getInventorySize();j++){
			if(myBand->getMember(i)->getInventory()->getItem(j)->getFunction()->getFunctionType() == Global::functionType::fFOOD){
				if(myBand->getMember(i)->getInventory()->getItem(j)->use() == 0){
					membersRemaining--;
					foodConsumed++;
				}
			}
		}
	}
	while(membersRemaining > 0){
		for(int i=0;i<myBand->getBandSize();i++){
			if(myBand->getBandSize() > 1){
				if(myBand->getMember(i) != this){
					myBand->getMember(i)->updateWounds(1);
					membersHungry++;
					membersRemaining--;
					if(myBand->getMember(i)->getWounds() >= myBand->getMember(i)->getEndurance()){
						foodSummary.append(myBand->getMember(i)->getName() +" has starved to death.\n");
						foodColor = Global::colorType::cRED_LIGHT;
						myBand->removeMember(i,true);
					}
				}
			}
			else{
				myBand->getMember(i)->updateWounds(1);
				membersHungry++;
				membersRemaining--;
				if(myBand->getMember(i)->getWounds() >= myBand->getMember(i)->getEndurance())
					return false;					
			}
		}
		rested = false;
	}
}
bool Player::consumeDailyFood(std::string &infoString, Global::colorType &infoStringColor)
{
	int memberCount = myBand->getBandSize();
	int foodIndex = 0, foodConsumed = 0, membersHungry = 0;
	infoString = "";
	infoStringColor = Global::colorType::cWHITE;
	if(getAllFood() > memberCount)
		consumeFoodHasEnough(foodConsumed);
	else if(!consumeFoodNotEnough(foodConsumed,membersHungry,infoString, infoStringColor))
			return false;
	infoString.append("Total food consumed: " + Global::ntos(foodConsumed) + "\n");
	if(membersHungry > 0){
		infoStringColor = Global::colorType::cRED;
		infoString.append("Allies starving: " + Global::ntos(membersHungry) + "\n");
	}
	return true;
}
int Player::getAllFood()
{
	int totalFood = 0;
	for(int i=0;i<myBand->getBandSize();i++){
		for(int j=0;j<myBand->getMember(i)->getInventory()->getInventorySize();j++){
			if(myBand->getMember(i)->getInventory()->getItem(j)->getFunction()->getFunctionType() == Global::functionType::fFOOD){
				int val = 0;
				Global::ston(myBand->getMember(i)->getInventory()->getItem(j)->getFunction()->getFunctionValue(),val);
				totalFood += val;
			}
		}
	}
	return totalFood;
}
void Player::kill()
{
	this->~Player();
}

Swordsman::Swordsman()
{
	name = "Swordman";
}
Swordsman::Swordsman(std::string newName)
{
	charID = newName;
	name = "Swordman";
}
Swordsman::Swordsman(int sEndurance, int sCombatStrength, int sWounds, int sCharisma, int sWit, int sMoney) :
	Character(sEndurance, sCombatStrength, sWounds, sCharisma, sWit, sMoney)
{
	name = "Swordman";
}
Swordsman::~Swordsman()
{
	//std::cout << "Swordsman destructor called" << std::endl;
}
void Swordsman::kill()
{
	this->~Swordsman();
}

Dwarf::Dwarf()
{
	name = "Dwarf";
}
Dwarf::Dwarf(std::string newName)
{
	charID = newName;name = "Dwarf";
}
Dwarf::Dwarf(int dEndurance, int dCombatStrength, int dWounds, int dCharisma, int dWit, int dMoney) :
	Character(dEndurance, dCombatStrength, dWounds, dCharisma, dWit, dMoney)
{
	name = "Dwarf";
}
Dwarf::~Dwarf()
{
	//std::cout << "Dwarf destructor called" << std::endl;
}
void Dwarf::kill()
{
	this->~Dwarf();
}

Mercenary::Mercenary()
{
	name = "Mercenary";
}
Mercenary::Mercenary(std::string newName)
{
	charID = newName;
	name = "Mercenary";
}
Mercenary::Mercenary(int mEndurance, int mCombatStrength, int mWounds, int mCharisma, int mWit, int mMoney) :
	Character(mEndurance, mCombatStrength, mWounds, mCharisma, mWit, mMoney)
{
	name = "Mercenary";
}
Mercenary::~Mercenary()
{
	//std::cout << "Mercenary destructor called" << std::endl;
}
void Mercenary::kill()
{
	this->~Mercenary();
}

IrateFarmer::IrateFarmer()
{
	name = "Irate Farmer";
}
IrateFarmer::IrateFarmer(std::string newName)
{
	charID = newName;
	name = "Irate Farmer";
}
IrateFarmer::IrateFarmer(int iEndurance, int iCombatStrength, int iWounds, int iCharisma, int iWit, int iMoney) :
	Character(iEndurance, iCombatStrength, iWounds, iCharisma, iWit, iMoney)
{
	name = "IrateFarmer";
}
IrateFarmer::~IrateFarmer()
{
	//std::cout << "Irate Farmer destructor called" << std::endl;
}
void IrateFarmer::kill()
{
	this->~IrateFarmer();
}