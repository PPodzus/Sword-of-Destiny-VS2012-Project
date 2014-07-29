#include "GameCore.h"
#include "Statistics.h"
#include "Display.h"
#include "Gamemap.h"
#include "Character.h"
#include "Inventory.h"
#include "GameSave.h"
#include "GameHelp.h"

GameCore::GameCore(ConsoleDisplay* display)
{
	this->display = display;
	inGame = false;
	npcCounter = 0;
	short tempShort = randomGenerator.nextUInt() + rand();
	if(tempShort < 0) tempShort *= (-1);
	tempShort = tempShort%9;
	switch(tempShort){
	case 0: SODLocHintRandomizer = MapPos(-1,-1); break;
	case 1: SODLocHintRandomizer = MapPos(-1,0); break;
	case 2: SODLocHintRandomizer = MapPos(-1,1); break;
	case 3: SODLocHintRandomizer = MapPos(0,-1); break;
	case 4: SODLocHintRandomizer = MapPos(0,0); break;
	case 5: SODLocHintRandomizer = MapPos(0,1); break;
	case 6: SODLocHintRandomizer = MapPos(1,-1); break;
	case 7: SODLocHintRandomizer = MapPos(1,0); break;
	case 8: SODLocHintRandomizer = MapPos(1,1); break;
	}
	lastEvent = Global::eventType::EVENT_NOTHING;randomGenerator.randomize();
	stats = new Statistics();
	map = new GameMap();
	items = new Inventory();
}
void GameCore::initialSetup()
{
	loadFunctions();
	loadItems();
	loadHelp();
	loadDifficultyData();
	updateSaveFiles();
}
void GameCore::newGame(std::string charName)
{
	setMap();
	generateScenario();
	gPlayer->setName(charName);
	inGame = true;
}
void GameCore::setMap(std::string map_file_path)
{
	std::string temp, varName, varVal;
	std::ifstream mapFile(map_file_path);
	while(Global::getLeLine(mapFile,temp)){
		varName = Global::getLeVarName(temp);
		varVal = Global::getLeVarVal(temp);
		if(varName == "MAP"){
			int mapType;
			if(varVal == "COMPLEX")
				mapType = 1;
			else if(varVal == "SIMPLE")
				mapType = 2;
			else{
				/*error, unknown map type*/}

			if(mapType == 1){
				while(Global::getLeLine(mapFile,temp) && temp != "End"){
					if(temp == "DefaultTile"){
						while(Global::getLeLine(mapFile,temp) && temp != "End"){
							varName = Global::getLeVarName(temp);
							varVal = Global::getLeVarVal(temp);
							Global::mTerrain defaultTile;
							//map->setTerrain(varVal,defaultTile);
							for(int i = 0;i < 20;i++)
								for(int j = 0; j< 20;j++)
									map->setTerrain(MapPos(i,j),varVal);
						}
					}
					if(temp == "Tile"){
						MapPos tilePosition;
						std::string tileType;
						bool gotX = false, gotY = false, gotTile = false;
						while(Global::getLeLine(mapFile,temp) && temp != "End"){
							varName = Global::getLeVarName(temp);
							varVal = Global::getLeVarVal(temp);
							if(varName == "X"){
								tilePosition.x = std::stoi(varVal);
								gotX = true;
							}
							else if(varName == "Y"){
								tilePosition.y = std::stoi(varVal);
								gotY = true;
							}
							else if(varName == "TYPE"){
								gotTile = true;
								tileType = varVal;
							}
							if(gotTile && gotX && gotY)
								map->setTerrain(tilePosition,tileType);
						}
					}
				}
			}
			else{
				int currentX = 0, currentY = 0;
				std::string temp;
				while(currentY < 20){
					std::getline(mapFile,temp);
					while(temp != " End"){
						std::string currentTile = temp.substr(temp.find_first_not_of(' '),temp.find_first_not_of(' ')+1);
						int tileID = std::stoi(currentTile);
						map->setTerrain(MapPos(currentX,currentY),tileID);
						temp = temp.substr(temp.find_first_not_of(' ') +1,temp.find("End")+3);
						currentX++;

						if(currentX == 20){
							currentY++;
							currentX=0;
						}
					}
				}
			}
		}
	}
}
void GameCore::setMap()
{
	randomGenerator.randomize(rand());
	int randomNumber;
	//srand(time(NULL));
	int x = rand();
	for(int i = 0;i < 20;i++){
		for(int j = 0; j<20;j++){
			randomNumber = (randomGenerator.nextUInt()*rand())%7;
			Global::mTerrain nextTile;
			switch(randomNumber){case 0: nextTile = Global::mTerrain::TILE_COUNTRYSIDE; break;
			case 1: nextTile = Global::mTerrain::TILE_FARMLAND; break;
			case 2: nextTile = Global::mTerrain::TILE_FOREST; break;
			case 3: nextTile = Global::mTerrain::TILE_HILL; break;
			case 4: nextTile = Global::mTerrain::TILE_MOUNTAIN; break;
			case 5: nextTile = Global::mTerrain::TILE_DESERT; break;
			case 6: nextTile = Global::mTerrain::TILE_SWAMP; break;}
			map->setTerrain(MapPos(j,i),nextTile);
		}
	}
}
void GameCore::setVisitedAtTile(MapPos position,bool whats_the_status)
{
	map->setVisitedAt(position,whats_the_status);
}
MapPos GameCore::getPlrStartPos()
{
	return playerStartingPos;
}
Global::functionType GameCore::extractFuncType(std::string s)
{
	s = Global::toUpperString(s);
	//WinItem, HintItem, Wound, Heal, ModifyCombStrength, ModifyCharisma, ModifyWit, ModifyEndurance
	if(s == "WINITEM")
		return Global::functionType::fWIN_ITEM;
	else if(s == "HINTITEM")
		return Global::functionType::fHINT_ITEM;
	else if(s == "WOUND")
		return Global::functionType::fWOUND;
	else if(s == "HEAL")
		return Global::functionType::fHEAL;
	else if(s == "MODIFYCOMBATSTRENGTH")
		return Global::functionType::fMODFIY_COMBATSTRENGTH;
	else if(s == "MODIFYCHARISMA")
		return Global::functionType::fMODIFY_CHARISMA;
	else if(s == "MODIFYWIT")
		return Global::functionType::fMODFIY_WIT;
	else if(s == "MODIFYENDURANCE")
		return Global::functionType::fMODFIY_ENDURANCE;
	else if(s == "DROPMONEY")
		return Global::functionType::fDROPMONEY;
	else if(s == "FOOD")
		return Global::functionType::fFOOD;
}
Global::mTerrain GameCore::extractTerrainType(std::string s)
{
	s = Global::toUpperString(s);
	if(s == "COUNTRYSIDE"){
		return Global::mTerrain::TILE_COUNTRYSIDE;
	}
	else if(s == "FARMLAND"){
		return Global::mTerrain::TILE_FARMLAND;
	}
	else if(s == "FOREST"){
		return Global::mTerrain::TILE_FOREST;
	}
	else if(s == "HILL"){
		return Global::mTerrain::TILE_HILL;
	}
	else if(s == "MOUNTAIN"){
		return Global::mTerrain::TILE_MOUNTAIN;
	}
	else if(s == "DESERT"){
		return Global::mTerrain::TILE_DESERT;
	}
	else if(s == "SWAMP"){
		return Global::mTerrain::TILE_SWAMP;
	}
	else
		return Global::mTerrain::TILE_COUNT;
}
Global::difficultyLevels GameCore::extractDifficulty(std::string s)
{
	Global::toUpperConvert(s);
	if(s == "EASY")
		return Global::difficultyLevels::dEASY;
	else if(s == "MEDIUM")
		return Global::difficultyLevels::dMEDIUM;
	else if(s == "HARD")
		return Global::difficultyLevels::dHARD;
	else throw std::string("Unknown difficulty level detected.");
}
ItemFunction* GameCore::getFunction(std::string name)
{
	for(int i=0;i<functions.size();i++)
		if(functions[i]->getFunctionID() == name)
			return functions[i];
	return NULL;
}
void GameCore::loadFunctions()
{
	std::string filePath = "Data/Functions.ini";
	std::ifstream file(filePath);
	int lineCount = 0;
	
	std::string temp, varName, varVal, tempMin = "", tempMax = "";;
	std::string funcName;
	std::string errorHeader = "Whoops! Error occured!", errorContent;
	bool error = false,warning = false;
	bool errorEarlyEnd = false, errorUnknownVar = false, errorUnknownVal = false;
	while(Global::getLeLine(file,temp,lineCount)){
		varName = Global::getLeVarName(temp);
		varVal = Global::getLeVarVal(temp);
		if(varName == "FUNCTION"){
			std::string funcName = varVal;
			for(int i=0;i<functions.size();i++)
				if(functions[i]->getFunctionID() == funcName)
					throw std::string("Duplicate function found in Data/Functions.ini at line " + Global::ntos(lineCount) + "\nFunction: " + funcName);
			Global::functionType funcType;
			std::string funcValues = "";
			//functionValues.at(0) = name of the function
			while(Global::getLeLine(file,temp,lineCount) && temp != "End"){
				varName = Global::getLeVarName(temp);
				varVal = Global::getLeVarVal(temp);
				if(varName == "TYPE"){
					funcType = extractFuncType(varVal);
					switch(funcType){
					case Global::functionType::fDROPMONEY:
						while(Global::getLeLine(file,temp,lineCount) && temp != "End"){
							varName = Global::getLeVarName(temp);
							varVal = Global::getLeVarVal(temp);
							if(varName == "AMOUNTMIN")
								tempMin = varVal;
							else if(varName == "AMOUNTMAX")
								tempMax = varVal;
							else
								throw std::string("Unknown function parameter in Data/Functions.ini at line " + Global::ntos(lineCount) + "\nFunction: " + funcName + " (" + varName + ")");
						}
						break;
					case Global::functionType::fHINT_ITEM:
						while(Global::getLeLine(file,temp,lineCount) && temp != "End"){
							varName = Global::getLeVarName(temp);
							varVal = Global::getLeVarVal(temp);
							if(varName == "HINT"){
								tempMin = varVal;
								if(varVal.size() > 0 && varVal[0] == '\"' && varVal.back() == '\"')
									tempMin = tempMin.substr(1,temp.size()-2);
								else if(varVal == "WinItemLocation")
									tempMin = "(#@!56%&%$#!'[]\;/.,";//highly unlikely someone will set this as a message
								else
									throw std::string("Unknown function value in Data/Functions.ini at line " + Global::ntos(lineCount) + "\nFunction: " + funcName + " (" + varName + ")");
							}
							else
								throw std::string("Unknown function parameter in Data/Functions.ini at line " + Global::ntos(lineCount) + "\nFunction: " + funcName + " (" + varName + ")");
						}
						break;
					case Global::functionType::fWIN_ITEM: 
						while(Global::getLeLine(file,temp,lineCount) && temp != "End"){
							varName = Global::getLeVarName(temp);
							varVal = Global::getLeVarVal(temp);
							if(varName == "CONDITION"){
								tempMin = varVal;
								if(varVal == "StartReturn")
									tempMin = "1";
								else if(varVal == "Pickup")
									tempMin = "2";
								else
									throw std::string("Unknown function value in Data/Functions.ini at line " + Global::ntos(lineCount) + "\nFunction: " + funcName + " (" + varName + ")");
							}
							else
								throw std::string("Unknown function parameter in Data/Functions.ini at line " + Global::ntos(lineCount) + "\nFunction: " + funcName + " (" + varName + ")");
						}
						break;
					case Global::functionType::fFOOD:
					case Global::functionType::fHEAL: 
					case Global::functionType::fWOUND:
					case Global::functionType::fMODFIY_COMBATSTRENGTH:
					case Global::functionType::fMODFIY_ENDURANCE:
					case Global::functionType::fMODFIY_WIT:
					case Global::functionType::fMODIFY_CHARISMA:
						while(Global::getLeLine(file,temp,lineCount) && temp != "End"){
							varName = Global::getLeVarName(temp);
							varVal = Global::getLeVarVal(temp);
							if(varName == "AMOUNT")
								tempMin = varVal;
							else
								throw std::string("Unknown function parameter in Data/Functions.ini at line " + Global::ntos(lineCount) + "\nFunction: " + funcName + " (" + varName + ")");
						}
						break;
					}
				}
			}
			funcValues.append(tempMin).append(" ").append(tempMax);
			functions.push_back(new ItemFunction(funcName, funcType, funcValues));
		}
		else{
			throw std::string("Unknown data found in Data/Functions.ini at line " + Global::ntos(lineCount) + "\n'" + varName + "'");
		}
	}
}
void GameCore::loadItems()
{
	DIR *dir;
	struct dirent *ent;
	std::string path = "Data/Items";
	std::string temp, varName, varVal;
	
	if ((dir = opendir (path.c_str())) != NULL){
		while ((ent = readdir (dir)) != NULL){
			std::string fullPath = path + "/";
			fullPath.append(ent->d_name);
			temp = ent->d_name;
			int strPos = temp.length()-4;
			int iniPos = temp.find(".ini");
			if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 && (strPos == iniPos)){
				//open next file
				std::ifstream itemFile(fullPath);
				std::string itemName,ID,description;
				int weight, dropChance;
				std::vector<std::pair<int,Global::mTerrain>> dropModifiers;
				ItemFunction* itemFunc = NULL;
				int lineCount = 0;
				//start extracting values from the file,
				while(Global::getLeLine(itemFile,temp,lineCount) && (temp != "End")){
					varName = Global::getLeVarName(temp);
					varVal = Global::getLeVarVal(temp);
					if(varName == "ITEM"){
						itemName = varVal;
						while(Global::getLeLine(itemFile,temp,lineCount) && (temp != "End")){
							varName = Global::getLeVarName(temp);
							varVal = Global::getLeVarVal(temp);
							if(varName == "ID"){
								for(int i=0;i<items->getInventorySize();i++)
									if(items->getItem(i)->getID() == varVal)
										throw std::string("Duplicate item ID found at " + Global::ntos(lineCount) + "\nItem: '" + itemName + "'");
								ID = varVal;
							}
							else if(varName == "DESCRIPTION"){
								description = varVal;
							}
							else if(varName == "WEIGHT"){
								Global::ston(varVal,weight);
							}
							else if(varName == "DROPDATA"){
								while(Global::getLeLine(itemFile,temp,lineCount) && (temp != "End")){
									varName = Global::getLeVarName(temp);
									varVal = Global::getLeVarVal(temp);
									if(varName == "DROPCHANCE"){
										if(varVal.back() == '%'){
											Global::ston(varVal.substr(0,varVal.size()-1),dropChance);
											dropModifiers.push_back(std::make_pair(dropChance,Global::mTerrain::TILE_COUNT));
										}
										else
											throw std::string("Unknown value for DropChance in DropData for Item '" + itemName + "',\nat line: " + Global::ntos(lineCount) + " in file '" +  ent->d_name);
									}
									else if(varName == "TERRAINMODIFIER"){
										Global::mTerrain modifier = extractTerrainType(varVal);
										Global::getLeLine(itemFile,temp,lineCount);
										if(Global::getLeVarName(temp) == "MODIFIERVALUE" && modifier != Global::mTerrain::TILE_COUNT){
											if(Global::getLeVarVal(temp).back() == '%'){
												int drop = 0;
												Global::ston(Global::getLeVarVal(temp).substr(0,varVal.size()-1),drop);
												dropModifiers.push_back(std::make_pair(drop,modifier));
											}
											else
												throw std::string("Unknown value for ModifierValue in DropData for Item '" + itemName + "',\nat line: " + Global::ntos(lineCount) + " in file '" +  ent->d_name);
											Global::getLeLine(itemFile,temp,lineCount); //looking for "End"
											if(temp != "End")
												throw std::string("Unknown parameter in DropData for Item '" + itemName + "',\nat line: " + Global::ntos(lineCount) + " in file '" +  ent->d_name);
										}
										else
											throw std::string("ModifierValue missing for TerrainModifier in DropData for Item '" + itemName + "',\nat line: " + Global::ntos(lineCount) + " in file '" +  ent->d_name);
									}
									else
										throw std::string("Unknown parameter in DropData for Item '" + itemName + "',\nat line: " + Global::ntos(lineCount) + " in file '" +  ent->d_name);
								}
							}
							else if(varName == "FUNCTION"){
								itemFunc = getFunction(varVal);
								if(itemFunc == NULL)
									throw std::string("Unknown Function(" + varVal +")declared in Item '" + itemName + "',\nat line: " + Global::ntos(lineCount) + " in file '" +  ent->d_name);
							}
							else
								throw std::string("Unknown parameter in Item '" + itemName + "',\nat line: " + Global::ntos(lineCount) + " in file '" +  ent->d_name);
						}
						new Item(itemName,ID,weight,items,dropModifiers,itemFunc,description);
					}
					else
						throw std::string("Unknown line at: " + Global::ntos(lineCount) + " in file '" +  ent->d_name);
				}
				itemFile.close();
			}
		}
	}
}
void GameCore::loadHelp()
{
	help = new GameHelp(display);
}
void GameCore::loadDifficultyData()
{
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++){
			minStats[i][j][0] = 1;
			minStats[i][j][1] = 1;
			minStats[i][j][2] = 1;
			minStats[i][j][3] = 1;
		}
	std::ifstream file;
	std::string temp = "", varName = "", path = "Data/GameDifficulty.ini";
	Global::difficultyLevels difficultyPreset;
	std::stringstream ss;
	int currentLine = 0;
	bool gotStats[3][3] = {{false,false,false},{false,false,false},{false,false,false}};
	file.open(path);
	while(Global::getLeLine(file,temp,currentLine)){
		varName = Global::getLeVarName(temp);
		if(varName == "DIFFICULTY"){
			difficultyPreset = extractDifficulty(Global::getLeVarVal(temp));
			while(Global::getLeLine(file,temp,currentLine) && temp != "End"){
				varName = Global::getLeVarName(temp);
				if(varName == "PLAYER"){
					if(gotStats[difficultyPreset][0])
						throw std::string("Player data already specified for this difficulty level at line: " + 
											Global::ntos(currentLine) + "\nin file 'Data/GameDifficulty.ini'");
					while(Global::getLeLine(file,temp,currentLine) && temp != "End"){
						varName = Global::getLeVarName(temp);
						if(varName == "STATS"){
							ss << Global::getLeVarVal(temp);
							ss >> minStats[difficultyPreset][0][0] >> minStats[difficultyPreset][0][1]
							>> minStats[difficultyPreset][0][2] >> minStats[difficultyPreset][0][3];
						}
						else if(varName == "ITEM"){
							startingItems[difficultyPreset][0].push_back(Global::getLeVarVal(temp));
							if(items->getItemIndexID(startingItems[difficultyPreset][0].back()) == -1)
								throw std::string("Unknown ItemID specified in file 'Data/GameDifficulty.ini'\nat line: " +
								Global::ntos(currentLine));
						}
					}
					gotStats[difficultyPreset][0] = true;
				}
				else if(varName == "ALLY"){
					if(gotStats[difficultyPreset][1])
						throw std::string("Ally data already specified for this difficulty level at line: " +
											Global::ntos(currentLine) + "\nin file 'Data/GameDifficulty.ini'");
					while(Global::getLeLine(file,temp,currentLine) && temp != "End"){
						varName = Global::getLeVarName(temp);
						if(varName == "STATS"){
							ss << Global::getLeVarVal(temp);
							ss >> minStats[difficultyPreset][1][0] >> minStats[difficultyPreset][1][1]
							>> minStats[difficultyPreset][1][2] >> minStats[difficultyPreset][1][3];
						}
						else if(varName == "ITEM"){
							startingItems[difficultyPreset][1].push_back(Global::getLeVarVal(temp));
							if(items->getItemIndexID(startingItems[difficultyPreset][1].back()) == -1)
								throw std::string("Unknown ItemID specified in file 'Data/GameDifficulty.ini'\nat line: " +
								Global::ntos(currentLine));
						}
					}
					gotStats[difficultyPreset][1] = true;
				}
				else if(varName == "ENEMY"){
					if(gotStats[difficultyPreset][2])
						throw std::string("Enemy data already specified for this difficulty level at line: " +
											Global::ntos(currentLine) + "\nin file 'Data/GameDifficulty.ini'");
					while(Global::getLeLine(file,temp,currentLine) && temp != "End"){
						varName = Global::getLeVarName(temp);
						if(varName == "STATS"){
							ss << Global::getLeVarVal(temp);
							ss >> minStats[difficultyPreset][2][0] >> minStats[difficultyPreset][2][1]
							>> minStats[difficultyPreset][2][2] >> minStats[difficultyPreset][2][3];
						}
						else if(varName == "ITEM"){
							startingItems[difficultyPreset][2].push_back(Global::getLeVarVal(temp));
							if(items->getItemIndexID(startingItems[difficultyPreset][2].back()) == -1)
								throw std::string("Unknown ItemID specified in 'Data/GameDifficulty.ini'\nat line: " +
								Global::ntos(currentLine));
						}
					}
					gotStats[difficultyPreset][2] = true;
				}
				else throw std::string("Unknown value for character type at line: " + Global::ntos(currentLine) +
										"\nin file 'Data/GameDifficulty.ini'");

			}
		}
		else throw std::string("Unknown value for difficlty level at line: " + Global::ntos(currentLine) +
								"\nin file 'Data/GameDifficulty.ini'");
	}
	if(!(gotStats[0][0] && gotStats[0][1] && gotStats[0][2] && gotStats[1][0] && gotStats[1][1]
		&& gotStats[1][2] && gotStats[2][0] && gotStats[2][1] && gotStats[2][2]))
			throw std::string("Unexpected end of file: 'Data/GameDifficulty.ini', critical data missing");
}
int GameCore::getPlayerInput(int minValue, int maxValue, bool NtoCancel, ConsoleDisplay* display)
{
	std::string sInput;
	int input = -1;
	do{
		if(display != NULL)
			display->print();
		Global::clearBuffer();
		if(NtoCancel){
			std::cin >> sInput;
			if(Global::toUpperString(sInput) == "N")
				return -1;
			Global::ston(sInput,input);
		}
		else{
			std::cin >> input;
		}
	}while(input < minValue || input > maxValue);
	return input;
}
void GameCore::waitForEnter()
{
	Global::clearBuffer();
	std::cin.get();
}
void GameCore::go()
{
	int mainMenuIn = -1;
	bool gameRunning = true, jumpToMainMenu = false, saveGameTrigger = false, gameLoaded = false;
	while(gameRunning){
		if(gameLoaded)
			mainMenuIn = 1;
		else{
			loadMainMenu();
			mainMenuIn = getPlayerInput(1,4,false,display);
		}
		if(jumpToMainMenu)
			jumpToMainMenu = false;
		switch(mainMenuIn){
		case 1:{
			if(!gameLoaded)
				selectNewGame();
			else 
				gameLoaded = false;
			std::string eventInfo = "", huntingInfo = "", foodInfo = "";
			Global::colorType eventInfoColor = Global::colorType::cWHITE, huntingInfoColor = Global::colorType::cWHITE, foodColor = Global::colorType::cWHITE;
			int actionMenuIn, actionOptionsCount;
			while(inGame && !jumpToMainMenu){
				actionOptionsCount = loadGameTurnState(eventInfo,eventInfoColor,huntingInfo,huntingInfoColor,foodInfo,foodColor);
				actionMenuIn = getPlayerInput(1,actionOptionsCount,false,display);				
				switch(actionMenuIn){
				case 1:restTurn();break;
				case 2:travel(eventInfo,eventInfoColor);break;
				case 3:{
					gPlayer->searchArea(map,display);
					stats->totalSearchAreas++;
					   }break;
				case 4:managePlayerInventory();break;
				case 5:managePlayerBand();break;
				case 6:printGameMapState();break;
				case 7:{//menu if in desert hunting otherwise
					if(map->getTerrainAt(gPlayer->getPosition()) == Global::mTerrain::TILE_DESERT)
						goto ActionMenu9Jumper;
					else
						hunt(huntingInfo,huntingInfoColor);
					   }break;
				case 8:{//if in farmland, buy food from farmers, specifies amount of food, and allows distribution among team members
					if(map->getTerrainAt(gPlayer->getPosition()) == Global::mTerrain::TILE_FARMLAND)
						startShopping();
					else
						goto ActionMenu9Jumper;
					   }break;
				case 9:{//displays menu options
					ActionMenu9Jumper:
					actionMenuIn = 9;
					if(!pauseMenu()){
						reset();
						jumpToMainMenu = true;
						gameLoaded = false;
					}
					   }break;
				}
				if(inGame && actionMenuIn != 4 && actionMenuIn != 5 && actionMenuIn != 6 && actionMenuIn != 9){
					if(!gPlayer->consumeDailyFood(foodInfo,foodColor)){
						lose();
						break;
					}
					defragmentBands(); //clean up the carnage, get rid of dead bodies
					stats->totalTurns++;
					if(checkWin())
						win();
				}
			}
			   }break;
		case 2:gameLoaded = startLoadSequence();break;
		case 3: help->callHelp(); break;
		case 4:if(MessageBox(NULL, "Are you sure you want to exit?", "Exit", MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK) return; break;			
		}		
	}
}
void GameCore::loadMainMenu()
{
	display->clear();
	display->addString("////////////////////////////////////////////////////////////////////////////////",0,0,Global::colorType::cYELLOW);
	display->addString("////////      //  //////  //       //       //      /////       //      ////////",0,1,Global::colorType::cYELLOW);
	display->addString("////////  //////  //////  //  ///  //  ///  //  ///  ////  ///  //  ////////////",0,2,Global::colorType::cYELLOW);
	display->addString("////////      //  //  //  //  ///  //       //  ///  ////  ///  //     /////////",0,3,Global::colorType::cYELLOW);
	display->addString("////////////  //  //  //  //  ///  //  //  ///  ///  ////  ///  //  ////////////",0,4,Global::colorType::cYELLOW);
	display->addString("////////      //          //       //  ///  //      /////       //  ////////////",0,5,Global::colorType::cYELLOW);
	display->addString("////////////////////////////////////////////////////////////////////////////////",0,6,Global::colorType::cYELLOW);
	display->addString("////////////////////////////////////////////////////////////////////////////////",0,7,Global::colorType::cYELLOW);
	display->addString("///////////      ///      //      //      //      //  ////  //  ////  //////////",0,8,Global::colorType::cYELLOW);
	display->addString("///////////  ///  //  //////  ////////  //////  ////    //  //   //  ///////////",0,9,Global::colorType::cYELLOW);
	display->addString("///////////  ///  //    ////      ////  //////  ////        ////    ////////////",0,10,Global::colorType::cYELLOW);
	display->addString("///////////  ///  //  //////////  ////  //////  ////  //    /////  /////////////",0,11,Global::colorType::cYELLOW);
	display->addString("///////////      ///      //      ////  ////      //  ////  /////  /////////////",0,12,Global::colorType::cYELLOW);
	display->addString("////////////////////////////////////////////////////////////////////////////////",0,13,Global::colorType::cYELLOW);
	display->addString("[1] New Game",40,display->getDisplayHeight()-4,Global::colorType::cGREY_DARK);
	display->alignLine(display->getDisplayHeight()-4,Global::textAlign::tCENTER);
	display->addString("[2] Load Game",40,display->getDisplayHeight()-3);
	display->alignLine(display->getDisplayHeight()-3,Global::textAlign::tCENTER);
	display->addString("[3] Help",40,display->getDisplayHeight()-2,Global::colorType::cGREY_DARK);
	display->alignLine(display->getDisplayHeight()-2,Global::textAlign::tCENTER);
	display->addString("[4] Exit",40,display->getDisplayHeight()-1);
	display->alignLine(display->getDisplayHeight()-1,Global::textAlign::tCENTER);
}
void GameCore::selectNewGame()
{
	int mainMenuIn = -1;
	display->clear();
	display->addString("Select difficulty level (N to cancel):",40,display->getDisplayHeight()-4,Global::colorType::cGREY_DARK);
	display->alignLine(display->getDisplayHeight()-4,Global::textAlign::tCENTER);
	display->addString("[1] Easy",40,display->getDisplayHeight()-3);
	display->alignLine(display->getDisplayHeight()-3,Global::textAlign::tCENTER);
	display->addString("[2] Medium",40,display->getDisplayHeight()-2,Global::colorType::cGREY_DARK);
	display->alignLine(display->getDisplayHeight()-2,Global::textAlign::tCENTER);
	display->addString("[3] Hard",40,display->getDisplayHeight()-1);
	display->alignLine(display->getDisplayHeight()-1,Global::textAlign::tCENTER);
	
	mainMenuIn = getPlayerInput(1,3,true,display);
	if(mainMenuIn == -1)
		return;
	//generate no. of allies to be spawned based on selected difficulty 
	char characterName[20];
	characterName[0] = 0;
	display->clear();
	display->addString("Enter your Character's name [Max. 20 characters]: ",0,display->getDisplayHeight()-1,Global::colorType::cGREY_DARK);
	bool nameEmpty = true;
	do{
		//create a new random game
		display->print();
		Global::clearBuffer();
		std::cin.getline(characterName,20);
	}while(characterName[0] == 0);
	setDifficulty(mainMenuIn);
	newGame(characterName);
	createPlayerBand();
}
bool GameCore::pauseMenu()
{
	bool inPauseMenu = true;
	int pauseMenuIn = -1;
	while(inPauseMenu){
		display->clear();
		display->addString("[1] Continue",40,display->getDisplayHeight()-6,Global::colorType::cGREY_DARK);
		display->alignLine(display->getDisplayHeight()-6,Global::textAlign::tCENTER);
		display->addString("[2] Look up statistics",40,display->getDisplayHeight()-5);
		display->alignLine(display->getDisplayHeight()-5,Global::textAlign::tCENTER);
		display->addString("[3] Save Game",40,display->getDisplayHeight()-4,Global::colorType::cGREY_DARK);
		display->alignLine(display->getDisplayHeight()-4,Global::textAlign::tCENTER);
		display->addString("[4] Load Game",40,display->getDisplayHeight()-3);
		display->alignLine(display->getDisplayHeight()-3,Global::textAlign::tCENTER);
		display->addString("[5] Help",40,display->getDisplayHeight()-2,Global::colorType::cGREY_DARK);
		display->alignLine(display->getDisplayHeight()-2,Global::textAlign::tCENTER);
		display->addString("[6] Main Menu",40,display->getDisplayHeight()-1);;
		display->alignLine(display->getDisplayHeight()-1,Global::textAlign::tCENTER);

		pauseMenuIn = getPlayerInput(1,6,false,display);

		switch(pauseMenuIn){
		case 1: inPauseMenu = false; break;
		case 2:{ printStatistics();
				waitForEnter();
			   }break;
		case 3: startSaveSequence(); break;
		case 4: startLoadSequence(); break;
		case 5: help->callHelp(); break;
		case 6:{
			if(MessageBox(NULL, "Are you sure you want to exit to main menu?\nAll unsaved progress will be lost.", "Exit to main menu", MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK){
				reset();
				return false;
			}
			   }break;
		}
	}
	return true;
}
void GameCore::printStatistics()
{
	display->clear();
	display->addString("Total Turns:                  " + Global::ntos(stats->totalTurns),0,0,Global::colorType::cGREY_LIGHT);
	display->addString("Number of times traveled:     " + Global::ntos(stats->totalTravel),0,1);
	display->addString("Number of fights:             " + Global::ntos(stats->totalFights),0,2,Global::colorType::cGREY_LIGHT);
	display->addString("Number of fights won:         " + Global::ntos(stats->fightsWon),0,3);
	display->addString("Number of fights fled:        " + Global::ntos(stats->fightsFled),0,4,Global::colorType::cGREY_LIGHT);
	display->addString("Total allies killed:          " + Global::ntos(stats->totalAlliesKilled),0,5);
	display->addString("Total enemies killed:         " + Global::ntos(stats->totalEnemiesKilled),0,6,Global::colorType::cGREY_LIGHT);
	display->addString("Total hunting attempts:       " + Global::ntos(stats->totalHuntAttempts),0,7);
	display->addString("Successfull hunting attempts: " + Global::ntos(stats->totalHunt),0,8,Global::colorType::cGREY_LIGHT);
	display->addString("Total food bought:            " + Global::ntos(stats->totalFoodBought),0,9);
	display->addString("Total locations searched:     " + Global::ntos(stats->totalSearchAreas),0,10,Global::colorType::cGREY_LIGHT);
	display->addString("Days rested:                  " + Global::ntos(stats->totalRests),0,11);
	display->addString("Total money found:            " + Global::ntos(stats->totalMoney),0,12,Global::colorType::cGREY_LIGHT);
	display->addString("Press Enter to go back",40,display->getDisplayHeight()-1);
	display->alignLine(display->getDisplayHeight()-1,Global::textAlign::tCENTER);
	display->print();	
}
int GameCore::loadGameTurnState(std::string eventSummary, Global::colorType eventColor, std::string huntingSummary, Global::colorType huntingColor, std::string foodSummary, Global::colorType foodColor)
{
	display->clear();
	display->addString(eventSummary,0,0,eventColor);
	display->addString(huntingSummary,0,1,huntingColor);
	display->addString(foodSummary,40,7,foodColor);
	display->addString("Game Status:",0,2);
	display->alignLine(2,Global::textAlign::tCENTER);
	display->addString("Current Position: x:" + Global::ntos(gPlayer->getPosition().x+1) + " y:" + Global::ntos(gPlayer->getPosition().y+1),0,3,Global::colorType::cGREY_DARK);
	display->addString("Day: " + Global::ntos(stats->totalTurns),0,4);
	display->addString("Currently on: " + map->getTerrainStringAt(gPlayer->getPosition()),0,5,Global::colorType::cGREY_DARK);
	display->addString("Band status: ",0,6);
	if(!gPlayer->getBand()->checkBandHealthy())
		display->addString("Wounded",13,6,Global::colorType::cRED_LIGHT);
	else
		display->addString("Healthy",13,6,Global::colorType::cGREEN_LIGHT);
	if(gPlayer->checkRested())
		display->addString(", rested\n", 20,6,Global::colorType::cGREEN_LIGHT);
	display->addString("Food remaining: " + Global::ntos(gPlayer->getAllFood()),0,7,Global::colorType::cGREY_DARK);
	if(gPlayer->getAllFood() < gPlayer->getBand()->getBandSize()*2)
		display->addString("***Food supply critical***",40,6,Global::colorType::cRED_LIGHT);
	else if(gPlayer->getAllFood() < gPlayer->getBand()->getBandSize()*3)
		display->addString("***Food supply low***",40,6,Global::colorType::cYELLOW);
	display->addString("Current Objective:",0,9);
	if(!gPlayer->checkWinItem())
		display->addString("Find the Sword of Destiny",0,10);
	else if(gPlayer->checkWinItem())
		display->addString("Return to your starting position",0,10);
	if(!gPlayer->checkHintItem() && !gPlayer->checkWinItem())
		display->addString("Optional: Find clue about the Sword of Destiny",0,11,Global::colorType::cGREY_DARK);

	display->addString("Select Action:",0,14,Global::colorType::cGREY_DARK);
	display->alignLine(80,Global::textAlign::tCENTER);
	display->addString("[1] Rest",0,15);
	display->alignLine(80,Global::textAlign::tCENTER);
	display->addString("[2] Travel",0,16,Global::colorType::cGREY_DARK);
	display->alignLine(80,Global::textAlign::tCENTER);
	display->addString("[3] Search area",0,17);
	display->alignLine(80,Global::textAlign::tCENTER);
	display->addString("[4] Open Player Inventory",0,18,Global::colorType::cGREY_DARK);
	display->alignLine(80,Global::textAlign::tCENTER);
	display->addString("[5] Manage Band Members",0,19);
	display->alignLine(80,Global::textAlign::tCENTER);
	display->addString("[6] Map",0,20,Global::colorType::cGREY_DARK);
	display->alignLine(80,Global::textAlign::tCENTER);
	if(map->getTerrainAt(gPlayer->getPosition()) == Global::mTerrain::TILE_FARMLAND){
		display->addString("[7] Hunt",0,21);
		display->alignLine(80,Global::textAlign::tCENTER);
		display->addString("[8] Buy Food",0,22,Global::colorType::cGREY_DARK);
		display->alignLine(80,Global::textAlign::tCENTER);
		display->addString("[9] Menu",0,23);
		display->alignLine(80,Global::textAlign::tCENTER);
		return 9;
	}
	else if(map->getTerrainAt(gPlayer->getPosition()) == Global::mTerrain::TILE_DESERT){
		display->addString("[7] Menu\n",0,21);
		display->alignLine(80,Global::textAlign::tCENTER);
		return 7;
	}
	else{
		//always update just to make sure
		display->addString("[7] Hunt",0,21);
		display->alignLine(80,Global::textAlign::tCENTER);
		display->addString("[8] Menu",0,22,Global::colorType::cGREY_DARK);
		display->alignLine(80,Global::textAlign::tCENTER);
		return 8;
	}
}
void GameCore::printGameMapState()
{
	display->clear();
	for(int i=0;i<21;i++){
		for(int j=0;j<21;j++){
			if(i==0 && j < 20)
				display->addString(" " + Global::ntos(j+1),j*3+3,0);
			if(j==0 && i != 0)
				display->addString(Global::ntos(i),0,i);
			if(i >= 0 && i < 20 && j >= 0 && j < 20){
				//handle visited/not visited areas
				if(map->checkVisitedAt(MapPos(i,j)))
					display->addString("[*]",i*3+3,j+1,Global::colorType::cGREY_DARK);
				else
					display->addString("[ ]",i*3+3,j+1);
			}
		}
	}
	//add starting pos, overridden by anything
	MapPos sPos = getPlrStartPos();
	display->addString("{S}",sPos.x*3+3,sPos.y+1,Global::colorType::cCYAN_LIGHT);
	if(gPlayer->checkHintItem()){
		MapPos sodPos = SODLocHintRandomizer, tempPos;
		sodPos.x += SODLocation.x;
		sodPos.y += SODLocation.y;
		if(sodPos.x < 1)
			sodPos.x = 1;
		else if(sodPos.x > 18)
			sodPos.x = 18;

		if(sodPos.y < 1)
			sodPos.y = 1;
		else if(sodPos.y > 18)
			sodPos.y = 18;

		tempPos = sodPos;
		display->addString("?",(tempPos.x-1)*3+4,(tempPos.y-1)+1,Global::colorType::cGREEN_LIGHT);
		display->addString("?",tempPos.x*3+4,(tempPos.y-1)+1,Global::colorType::cGREEN_LIGHT);
		display->addString("?",(tempPos.x+1)*3+4,(tempPos.y-1)+1,Global::colorType::cGREEN_LIGHT);
		display->addString("?",(tempPos.x-1)*3+4,tempPos.y+1,Global::colorType::cGREEN_LIGHT);
		display->addString("?",tempPos.x*3+4,tempPos.y+1,Global::colorType::cGREEN_LIGHT);
		display->addString("?",(tempPos.x+1)*3+4,tempPos.y+1,Global::colorType::cGREEN_LIGHT);
		display->addString("?",(tempPos.x-1)*3+4,(tempPos.y+1)+1,Global::colorType::cGREEN_LIGHT);
		display->addString("?",tempPos.x*3+4,(tempPos.y+1)+1,Global::colorType::cGREEN_LIGHT);
		display->addString("?",(tempPos.x+1)*3+4,(tempPos.y+1)+1,Global::colorType::cGREEN_LIGHT);
	}

	//add player pos, always on top
	MapPos pPos = gPlayer->getPosition();
	display->addString("O",pPos.x*3+4,pPos.y+1,Global::colorType::cYELLOW);

	//add legend
	display->addString("Legend:",67,0);
	display->addString("      Player's\n  [ ] Current\n      Position\n\n  [ ] not visited\n\n  [ ] visited\n\n      Player" + 
		std::string("\n    - Starting\n      Position\n\n      Sword of\n  [ ] Destiny\n      Location"),63,3);
	display->addString("O",66,4,Global::colorType::cYELLOW);
	display->addString("*",66,9,Global::colorType::cGREY_DARK);
	display->setColorAt(65,9,Global::colorType::cGREY_DARK);
	display->setColorAt(67,9,Global::colorType::cGREY_DARK);
	display->addString("{S}",65,12,Global::colorType::cCYAN_LIGHT);
	display->addString("?",66,16,Global::colorType::cGREEN_LIGHT);
	display->addString("Press Enter go go back.",0,display->getDisplayHeight()-1);
	display->alignLine(display->getDisplayHeight()-1,Global::textAlign::tCENTER);
	display->print();
	waitForEnter();
}
void GameCore::travel(std::string &eventSummary, Global::colorType &eventSummaryColor)
{
	//Traveling, player selects direction, an event might kick in, fight can be initiated
	Global::direction nextDir;
	int travelMenuIn = -1;
	display->clear();
	display->addString("Select travel direction (N to cancel):",0,display->getDisplayHeight()-5);
	display->addString("[1] North",0,display->getDisplayHeight()-4,Global::colorType::cGREY_DARK);
	display->addString("[2] South",0,display->getDisplayHeight()-3);
	display->addString("[3] West",0,display->getDisplayHeight()-2,Global::colorType::cGREY_DARK);
	display->addString("[4] East",0,display->getDisplayHeight()-1);
	travelMenuIn = getPlayerInput(1,4,true,display);
	if(travelMenuIn == -1)
		return;

	switch(travelMenuIn){
	case 1: nextDir = Global::direction::DIR_NORTH;	break;
	case 2: nextDir = Global::direction::DIR_SOUTH; break;
	case 3: nextDir = Global::direction::DIR_WEST; break;
	case 4: nextDir = Global::direction::DIR_EAST; break;
	}

	if(gPlayer->setMoveDirection(nextDir)){
		Global::eventType newEvent = generateEvent();
		if(newEvent != Global::eventType::EVENT_LOST){
			if(newEvent == Global::eventType::EVENT_NOTHING){
				eventSummary = "You have reached your destination safely";
				eventSummaryColor = Global::colorType::cGREEN_LIGHT;
			}
		}
		else{
			eventSummary = "You got lost, and could not travel to the destination as a result";
			eventSummaryColor = Global::colorType::cRED;
		}

		if(executeEvent(newEvent)){
			gPlayer->move(nextDir);
			setVisitedAtTile(gPlayer->getPosition(),true);
			if(newEvent != Global::eventType::EVENT_LOST){
				eventSummary = "You have reached your destination safely";
				eventSummaryColor = Global::colorType::cGREEN_LIGHT;
				stats->totalTravel++;
			}
			gPlayer->setRested(false);
		}
		else{
			reset();
		}
	}
	else{
		eventSummary = "Can't move in this direction, you're at a border of a map";
		eventSummaryColor = Global::colorType::cYELLOW;
	}
}
void GameCore::managePlayerInventory()
{
	bool managingInventory = true;
	while(managingInventory){
		int inventoryIn = -1;
		display->clear();
		gPlayer->printInventory(display);
		display->addString("What would you like to do?",0,display->getDisplayHeight()-4);
		display->addString("[1] Drop item",0,display->getDisplayHeight()-3,Global::colorType::cGREY_DARK);
		display->addString("[2] Look up item details",0,display->getDisplayHeight()-2);
		display->addString("[3] Finish",0,display->getDisplayHeight()-1,Global::colorType::cGREY_DARK);
		inventoryIn = getPlayerInput(1,3,false,display);

		int lookupInventoryInInt = -1;
		switch(inventoryIn){
		case 1:{
			lookupInventoryInInt = -1;
			display->clear();
			gPlayer->printInventory(display);
			if(gPlayer->getInventory()->getInventorySize() > 0){
				display->addString("Select item to drop",0,display->getDisplayHeight()-1);
				if(gPlayer->getInventory()->getInventorySize() == 1)
					display->addString(" (1) or 'N' to cancel.",19,display->getDisplayHeight()-1);
				else
					display->addString(" (1-" + Global::ntos(gPlayer->getInventory()->getInventorySize()) + ") or 'N' to cancel. ",19,display->getDisplayHeight()-1);
			}
			else{
				display->addString("You have no items in your inventory!\nPress Enter to go back.",0,display->getDisplayHeight()-1);
				waitForEnter();
				break;
			}
			lookupInventoryInInt = getPlayerInput(1,gPlayer->getInventory()->getInventorySize(),true,display);
			if(lookupInventoryInInt == -1)
				break;

			//drop the item on the ground below player 
			gPlayer->getInventory()->moveItemFromInventory(lookupInventoryInInt-1, map->getInventoryAt(gPlayer->getPosition()));
			   }break;
		case 2:{
			lookupInventoryInInt = -1;
			display->clear();
			gPlayer->printInventory(display);
			if(gPlayer->getInventory()->getInventorySize() > 0){
				display->addString("Select item to look up",0,display->getDisplayHeight()-1);
				if(gPlayer->getInventory()->getInventorySize() == 1)
					display->addString(" (1) or 'N' to cancel.",19,display->getDisplayHeight()-1);
				else
					display->addString(" (1-" + Global::ntos(gPlayer->getInventory()->getInventorySize()) + ") or 'N' to cancel. ",19,display->getDisplayHeight()-1);
			}
			else{
				display->addString("You have no items in your inventory!\nPress Enter to go back.",0,display->getDisplayHeight()-1);
				waitForEnter();
				break;
			}
			lookupInventoryInInt = getPlayerInput(1,gPlayer->getInventory()->getInventorySize(),true,display);
			if(lookupInventoryInInt == -1)
				break;

			display->clear();
			display->addString(gPlayer->getInventory()->getItem(lookupInventoryInInt-1)->getItemDetails(),0,2);
			display->addString("Press Enter to go back",0,display->getDisplayHeight()-1);
			display->print();
			waitForEnter();

			   }break;
		case 3:	managingInventory = false; break;
		}
	}
}
void GameCore::managePlayerBand()
{
	bool managingBand = true;
	while(managingBand){
		display->clear();
		int manageTeamSelect = -1;
		display->clear();
		display->addString("Player's Band:      Endurance / Combat STR / Charisma / Wit / Wounds / Gold\n",0,3,Global::colorType::cGREY_DARK);
		for(int i=0;i<gPlayer->getBand()->getBandSize();i++){
			if(i%2 == 1)
				display->addString("[" + Global::ntos(i+1) + "] ",0,i+4,Global::colorType::cGREY_DARK);
			else
				display->addString("[" + Global::ntos(i+1) + "] ",0,i+4);
			display->addString(gPlayer->getBand()->getMember(i)->getCharStatsString(),0,i+4);
		}
		if(gPlayer->getBand()->getBandSize()%2 == 1){
			display->addString("What would you like to do?",0,display->getDisplayHeight()-4);
			display->addString("[1] Trade with ally",0,display->getDisplayHeight()-3,Global::colorType::cGREY_DARK);
			display->addString("[2] Dismiss ally",0,display->getDisplayHeight()-2);
			display->addString("[3] Finish",0,display->getDisplayHeight()-1,Global::colorType::cGREY_DARK);
		}
		else{
			display->addString("What would you like to do?",0,display->getDisplayHeight()-4,Global::colorType::cGREY_DARK);
			display->addString("[1] Trade with ally",0,display->getDisplayHeight()-3);
			display->addString("[2] Dismiss ally",0,display->getDisplayHeight()-2,Global::colorType::cGREY_DARK);
			display->addString("[3] Finish",0,display->getDisplayHeight()-1);
		}
		manageTeamSelect = getPlayerInput(1,3,false,display);
		switch(manageTeamSelect){//since case 1 and 2 are very similar, they are together, and actual action changes based on manageTeamSelect
		case 1:
		case 2:{
			if(gPlayer->getBand()->getBandSize() > 1){
				std::string selectTradeMember = "";
				int selectTradeMemberInt = -1;
				display->clearString(display->getDisplayHeight()-4);
				display->clearString(display->getDisplayHeight()-3);
				display->clearString(display->getDisplayHeight()-2);
				display->clearString(display->getDisplayHeight()-1);
				int lastCharacterLine = 0;

				display->addString("Select ally 1",0,display->getDisplayHeight()-1);
				if(gPlayer->getBand()->getBandSize() > 2)
					display->addString(" - " + Global::ntos(gPlayer->getBand()->getBandSize()-1),14,display->getDisplayHeight()-1);
				if(manageTeamSelect == 1)
					display->addString(" to trade with  or 'N' to cancel.",18,display->getDisplayHeight()-1);
				else
					display->addString(" to dismiss  or 'N' to cancel.",18,display->getDisplayHeight()-1);
				selectTradeMemberInt = getPlayerInput(1,gPlayer->getBand()->getBandSize()-1,true,display);
				if(selectTradeMemberInt == -1)
					break;

				if(manageTeamSelect == 1){
					gPlayer->trade(gPlayer->getBand()->getMember(selectTradeMemberInt),map,display);
					break;
				}
				else{
					if(MessageBox(NULL, "Are you sure you want to dismiss this band member?\nAll items will be lost", "Dismiss band member?", MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK){
						gPlayer->getBand()->removeMember(manageTeamSelect,true);
						break;
					}
					else
						break;
				}
			}
			else{
				display->clear();
				display->addString("You don't have any allies to trade with! Press Enter to go back.",0,display->getDisplayHeight()-1);
				display->print();
				waitForEnter();
			}
			   }break;
		case 3:{managingBand = false;}break;
		}
	}
}
void GameCore::hunt(std::string &huntingSummary, Global::colorType &huntingSummaryColor)
{
	int foodChance = getNextRandInt() + rand() + 123;
	if(foodChance<0)
		foodChance *= (-1);
	foodChance = foodChance&10;
	float huntingPower = 0;
	for(int i=0;i<gPlayer->getBand()->getBandSize();i++)
		huntingPower += (gPlayer->getBand()->getMember(i)->getWit() + 
		(gPlayer->getBand()->getMember(i)->getCombatStrength() - gPlayer->getBand()->getMember(i)->getWounds()));
	huntingPower = huntingPower/gPlayer->getBand()->getBandSize();
	huntingPower = Global::roundNumber(huntingPower);
	if(gPlayer->checkRested())
		huntingPower++;
	if(map->getTerrainAt(gPlayer->getPosition()) == Global::mTerrain::TILE_MOUNTAIN)
		huntingPower--;
	if(huntingPower-1 >= foodChance){
		int foodHunted = 1;
		if(huntingPower > foodChance)
			foodHunted = 2;
		for(int i=0;i<gPlayer->getBand()->getBandSize();i++){
			for(int j=0;j<foodHunted;j++){
				if(!gPlayer->getBand()->getMember(i)->getInventory()->checkFull(1))
					copyItem("Food",gPlayer->getBand()->getMember(i)->getInventory());
				else{
					int huntFullIn = -1;
					display->clear();
					if(i==0)
						display->addString("Your hunt was successfull, but you don't have enough space in your inventory,"+ 
						std::string("would you like to open your inventory to drop some items?\n[1] Yes\n[2] No"),0,20);
					else
						display->addString(gPlayer->getBand()->getMember(i)->getName() +
						std::string("'s hunt was successfull, but he doesn't have enough space in your inventory,\n") +
						std::string("would you like to open his inventory to drop some items?\n[1] Yes\n[2] No"),0,20);
					huntFullIn = getPlayerInput(1,2,false,display);
					switch(huntFullIn)
					{
					case 1:{
						Character* theFood = new Character();
						theFood->setName("Food Hunted");
						copyItem("Food",theFood->getInventory());
						gPlayer->getBand()->getMember(i)->trade(theFood,map,display);
						if(theFood->getInventory()->getInventorySize() > 0)
							for(int k=0;k<theFood->getInventory()->getInventorySize();k++)
								theFood->getInventory()->moveItemFromInventory(k,map->getInventoryAt(gPlayer->getPosition()));
						   }break;
					case 2:
						break;
					}
				}
			}
		}
		huntingSummary = "Your band has successfully hunted " + Global::ntos(foodHunted) + " food each\n";
		huntingSummaryColor = Global::colorType::cGREEN_LIGHT;
		if(map->getTerrainAt(gPlayer->getPosition()) == Global::mTerrain::TILE_FARMLAND){
			int farmerAttackChance = getNextRandInt() + rand() + 123;
			if(farmerAttackChance < 0)
				farmerAttackChance *= (-1);
			farmerAttackChance = farmerAttackChance%100;
			if(farmerAttackChance <= 33){
				int farmerCount = getNextRandInt() + rand() + 10;
				if(farmerCount < 0)
					farmerCount *= (-1);
				farmerCount = farmerCount%10;
				Band* farmers = new Band("Irate Farmers");
				farmers->setBandType(Global::eventType::EVENT_MEET_FARMER);
				for(int j=0;j<farmerCount;j++){
					IrateFarmer* farmer = new IrateFarmer();
					farmers->addMember(farmer);
				}
				if(!executeFight(farmers))
					reset();
			}
		}
		stats->totalHunt++;
	}
	else{
		huntingSummary = "Your hunt was unsuccessfull\n";
		huntingSummaryColor = Global::colorType::cGREEN_LIGHT;
	}
	stats->totalHuntAttempts++;
}
void GameCore::startShopping()
{
	bool shopping = true;
	while(shopping){
		int shoppingMenuIn = -1, totalGold = 0;
		for(int i=0;i<gPlayer->getBand()->getBandSize();i++)
			totalGold += gPlayer->getBand()->getMember(i)->getInventory()->getGold();
		if(totalGold == 0){
			display->addString("You have no gold!\nPress Enter to go back.",0,display->getDisplayHeight()-1);
			display->print();
			waitForEnter();
			return;
		}
		display->clear();
		display->addString(printPlrBandInventory(),0,0);
		display->addString("Total Gold: " + Global::ntos(totalGold) + "\nWhat would you like to do?\n[1] Buy food for member [x]\n[2] Finish trading",0,20);
		shoppingMenuIn = getPlayerInput(1,2,false,display);

		switch(shoppingMenuIn){
		case 1:{
			int selectMember = -1;
			std::string stringSelectMember = "";
			display->clear();
			display->addString(printPlrBandInventory(),0,0);
			if(gPlayer->getBand()->getBandSize() > 1)
				display->addString("Select member (1-" + Global::ntos(gPlayer->getBand()->getBandSize()) + "), or 'N' to cancel.",0,display->getDisplayHeight()-1);
			else
				display->addString("Select member (1), or 'N' to cancel.",0,display->getDisplayHeight()-1);
			selectMember = getPlayerInput(1,gPlayer->getBand()->getBandSize(),true,display);
			if(selectMember == -1)
				break;
			std::string selectFoodAmount = "";
			int foodAmount = -1;
			display->clear();
			display->addString("How much food would you like to buy for this character?",0,23);
			if(10-gPlayer->getBand()->getMember(selectMember-1)->getInventory()->getInventorySize() < totalGold)
				display->addString("(max. " + Global::ntos(10-gPlayer->getBand()->getMember(selectMember-1)->getInventory()->getInventorySize())
				+ ", or 'N' to cancel)",0,display->getDisplayHeight()-1);
			else 
				display->addString("(max. " + Global::ntos(totalGold) + ", or 'N' to cancel)",0,display->getDisplayHeight()-1);
			foodAmount = getPlayerInput(0,10-gPlayer->getBand()->getMember(selectMember-1)->getInventory()->getInventorySize(),true,display);
			if(foodAmount == -1)
				break;

			int goldRemaining = foodAmount;
			for(int i=0;i<foodAmount;i++)
				copyItem("Food",gPlayer->getBand()->getMember(selectMember-1)->getInventory());
			for(int i=0;i<gPlayer->getBand()->getBandSize();i++){
				if(gPlayer->getBand()->getMember(i)->getInventory()->getGold() > foodAmount){
					gPlayer->getBand()->getMember(i)->getInventory()->modifyGold(-goldRemaining);
					break;
				}
				else{
					goldRemaining =- gPlayer->getBand()->getMember(i)->getInventory()->getGold();
					gPlayer->getBand()->getMember(i)->getInventory()->modifyGold(-11);
				}
			}
			stats->totalFoodBought += foodAmount;
			   }break;
		case 2:{/*back to game menu*/}break;
		}
	}
}
void GameCore::restTurn()
{
	//if we're healthy, resting, resetting band wounds to 0;
	if(gPlayer->checkTeamHealthy())
		for(int i =0;i<gPlayer->getBand()->getBandSize();i++)
			gPlayer->getBand()->getMember(i)->updateWounds(-999);
	gPlayer->setRested(true);
	stats->totalRests++;
}
void GameCore::setDifficulty(int playerChoice)
{
	int randomNewMember, randomAllyCount, foodAmount;
	switch(playerChoice){
	case 1:	difficulty = Global::difficultyLevels::dEASY; break;
	case 2:	difficulty = Global::difficultyLevels::dMEDIUM; break;
	case 3: difficulty = Global::difficultyLevels::dHARD; break;	
	}
}
void GameCore::createPlayerBand()
{
	
	int randomNewMember, randomAllyCount, foodAmount;
	switch(difficulty){
	case Global::difficultyLevels::dEASY:{//easy difficulty, player gets 4-5 allies
		randomAllyCount = Global::getRandomInt(5,4);
		   }break;
	case Global::difficultyLevels::dMEDIUM:{//medium difficulty, player gets 2-3 allies
		randomAllyCount = Global::getRandomInt(3,2);
		   }break;
	case Global::difficultyLevels::dHARD:{//hard difficulty, player gets 0-1 allies
		randomAllyCount = Global::getRandomInt(1,0);
		   }break;	
	}
	int diff = static_cast<int>(difficulty);
	for(int i=0;i<randomAllyCount;i++){
		int endurance = Global::getRandomInt(10,minStats[diff][1][0]),
			combatStrength = Global::getRandomInt(10,minStats[diff][1][1]),
			wit = Global::getRandomInt(10,minStats[diff][1][2]),
			charisma = Global::getRandomInt(10,minStats[diff][1][3]);
		Character* newChar;
		do{
			randomNewMember = getNextRandInt() + rand();
			randomNewMember = randomNewMember%3;
		}while(randomNewMember < 0);
		switch(randomNewMember){
		case 0:{ newChar = new Mercenary(endurance,combatStrength,0,charisma,wit,rand()%11);
			gPlayer->getBand()->addMember(newChar);
			newChar->setID(std::string("AlliedMercenary" + Global::ntos(i)));
			   }break;
		case 1:{ newChar = new Swordsman(endurance,combatStrength,0,charisma,wit,rand()%11);
			gPlayer->getBand()->addMember(newChar);
			newChar->setID(std::string("AlliedSwordman" + Global::ntos(i)));
			   }break;
		case 2:{ newChar = new Dwarf(endurance,combatStrength,0,charisma,wit,rand()%11);
			gPlayer->getBand()->addMember(newChar);
			newChar->setID(std::string("AlliedDwarf" + Global::ntos(i)));
			}break;
		}
		NPCs.push_back(newChar);
	}
	for(int i=1;i<gPlayer->getBand()->getBandSize();i++)
		for(int j=0;j<startingItems[difficulty][1].size();j++)
			gPlayer->getBand()->getMember(i)->getInventory()->copyItemToInventory(items->getItem(items->getItemIndexID(startingItems[difficulty][1][j])));
}
void GameCore::generateScenario()
{
	randomGenerator.randomize(rand());
	int x,y;
	//Generating Player start position
	int PlrPos, plrX, plrY;
	do{
		PlrPos = randomGenerator.nextFloat()+rand()+85469;
		PlrPos = PlrPos%400;
		x = PlrPos/20, y = PlrPos%20;
		plrX = x, plrY = y;
	}while(x < 0 || y < 0);

	//adding player to the game
	gPlayer = new Player(minStats[difficulty][0][0],minStats[difficulty][0][1],0,minStats[difficulty][0][3],minStats[difficulty][0][2],0);
	for(int i=0;i<startingItems[difficulty][0].size();i++)
		gPlayer->getInventory()->copyItemToInventory(items->getItem(items->getItemIndexID(startingItems[difficulty][0][i])));
	gPlayer->setPosition(plrX,plrY);
	map->setVisitedAt(gPlayer->getPosition(),true);
	playerStartingPos = gPlayer->getPosition();
	//generating Sword of Destiny location
	int SODPos;
	do{
		SODPos = randomGenerator.nextFloat()+rand()+7563;
		SODPos = SODPos%400;
		x = SODPos/20, y = SODPos%20;
	}
	while((plrX == x && plrY == y) || x < 0 || y < 0);	
	int SODx = x, SODy = y;
	//placing Sword of Destiny
	map->getInventoryAt(MapPos(x,y))->copyItemToInventory(items->getItem(items->getItemIndexID("SOD")));
	SODLocation.x = x;
	SODLocation.y = y;

	//generating Hint location
	int HintPos;
	do{
		HintPos = randomGenerator.nextFloat()+rand()+27530;
		HintPos = HintPos%400;
		x = HintPos/20, y = HintPos%20;
	}
	while((plrX == x && plrY == y) || (SODx == x && SODy == y)  || x < 0 || y < 0);
	//Placinng Hint
	map->getInventoryAt(MapPos(x,y))->copyItemToInventory(items->getItem(items->getItemIndexID("SODClue")));

	//using dropChance for each item, drop
	//called here to avoid repeated values
	for(int i=0;i<20;i++)
		for(int j=0;j<20;j++){
			//getDropChanceIf input has to be updated per terrain tile only, not per item
			Global::mTerrain thisTile = map->getTerrainAt(MapPos(j,i));
			for(int k=0;k<items->getInventorySize();k++){
				//get a new chance value
				int nextDropChance,itemDropChance;
				nextDropChance = (randomGenerator.nextUInt()+rand()+786);
				if(nextDropChance<0)
					nextDropChance *= (-1);
				nextDropChance = nextDropChance%100;
				//if the item drop chance is bigger or equal to random generated chance value, copy item to intentory at this slot
				itemDropChance = items->getItem(k)->getDropChanceAt(thisTile);
				//if the % chance is 0, change it to out-of-range value (-1) because %100 gives values 0-99 not 1-100
				if(itemDropChance == 0)
					itemDropChance = -1;
				if(itemDropChance >= nextDropChance){
					map->getInventoryAt(MapPos(j,i))->copyItemToInventory(items->getItem(k));
				}
			}
		}
}
Global::eventType GameCore::generateEvent()
{
	int eventChance, lostMin, lostMax, mSwrdMin, mSwrdMax, mDwrfMin, mDwrfMax, mMercMin, mMercMax, nothingMin, nothingMax;
	Global::mTerrain playerPos = map->getTerrainAt(gPlayer->getNextPosition());
	randomGenerator.randomize(rand());

	do{
		eventChance = randomGenerator.nextFloat()+rand();
		eventChance = eventChance%100;
	}while(eventChance < 0);

	switch(playerPos){	//COUNTRYSIDE
	case 0:	lostMin = 0,lostMax = 19;
		mSwrdMin = 20, mSwrdMax = 39;
		mDwrfMin = 40, mDwrfMax = 59;
		mMercMin = 60, mMercMax = 79;
		nothingMin = 80, nothingMax = 99;
		break;	//FARMLAND
	case 1:	lostMin = 101,lostMax = 101;
		mSwrdMin = 101, mSwrdMax = 101;
		mDwrfMin = 0, mDwrfMax = 32;
		mMercMin = 33, mMercMax = 65;
		nothingMin = 66, nothingMax = 99;
		break;	//FOREST
	case 2:	lostMin = 0,lostMax = 19;
		mSwrdMin = 20, mSwrdMax = 39;
		mDwrfMin = 40, mDwrfMax = 59;
		mMercMin = 60, mMercMax = 79;
		nothingMin = 80, nothingMax = 99;
		break;	//HILL
	case 3:	lostMin = 0,lostMax = 14;
		mSwrdMin = 15, mSwrdMax = 29;
		mDwrfMin = 30, mDwrfMax = 44;
		mMercMin = 45, mMercMax = 84;
		nothingMin = 85, nothingMax = 99;
		break;	//MOUNTAIN
	case 4:	lostMin = 0,lostMax = 19;
		mSwrdMin = 101, mSwrdMax = 101;
		mDwrfMin = 20, mDwrfMax = 59;
		mMercMin = 60, mMercMax = 79;
		nothingMin = 80, nothingMax = 99;
		break;	//DESERT
	case 5: lostMin = 0,lostMax = 39;
		mSwrdMin = 40, mSwrdMax = 54;
		mDwrfMin = 55, mDwrfMax = 69;
		mMercMin = 70, mMercMax = 84;
		nothingMin = 85, nothingMax = 99;
		break;	//SWAMP
	case 6:	lostMin = 0,lostMax = 19;
		mSwrdMin = 20, mSwrdMax = 39;
		mDwrfMin = 40, mDwrfMax = 59;
		mMercMin = 60, mMercMax = 79;
		nothingMin = 80, nothingMax = 99;
		break;

	}
	if((eventChance >= lostMin) && (eventChance <= lostMax))
		return Global::eventType::EVENT_LOST;
	else if((eventChance >= mSwrdMin) && (eventChance <= mSwrdMax))
		return Global::eventType::EVENT_MEET_SWORDSMAN;
	else if((eventChance >= mDwrfMin) && (eventChance <= mDwrfMax))
		return Global::eventType::EVENT_MEET_DWARF;
	else if((eventChance >= mMercMin) && (eventChance <= mMercMax))
		return Global::eventType::EVENT_MEET_MERCENARY;
	else if((eventChance >= nothingMin) && (eventChance <= nothingMax))
		return Global::eventType::EVENT_NOTHING;
	else{
		//error! out of bounds
	}
}
Band* GameCore::generateOrGetBandAt(MapPos position, Global::eventType &newEvent)
{
	if(map->checkBandAt(gPlayer->getNextPosition())){
		Global::eventType existingBandType = map->getBandAt(gPlayer->getNextPosition())->getBandType();
		switch(existingBandType){
		case 1:
			newEvent = Global::eventType::EVENT_MEET_SWORDSMAN;
			break;
		case 2: newEvent = Global::eventType::EVENT_MEET_MERCENARY;
			break;
		case 3: newEvent = Global::eventType::EVENT_MEET_DWARF;
			break;
		}
		return map->getBandAt(gPlayer->getNextPosition());
	}
	else{
		bool gotCharacterCount = false;
		int charCount;
		do{
			randomGenerator.randomize(rand());
			charCount = randomGenerator.nextUInt()+rand();
			charCount = charCount%(rand()+11);
			if(charCount < 0)
				charCount *= (-1);
			switch(newEvent){
			case 1:
				charCount = charCount%5; 
				if(charCount>=1)
					gotCharacterCount = true;
				break;
			case 2: charCount = charCount%7; 
				if(charCount>=2)
					gotCharacterCount = true;
				break;
			case 3: charCount = charCount%11; 
				if(charCount>=1)
					gotCharacterCount = true;
				break;
			}
		}while(!gotCharacterCount);
		std::vector<Character*> newBandMembers;
		for(int i = 0; i< charCount;i++){
			int endurance = Global::getRandomInt(10,minStats[difficulty][2][0]),
				combatStrength = Global::getRandomInt(10,minStats[difficulty][2][1]),
				wit = Global::getRandomInt(10,minStats[difficulty][2][2]),
				charisma = Global::getRandomInt(10,minStats[difficulty][2][3]);
			std::string npcName;
			switch(newEvent){
			case 1: npcName = "Swordman" + Global::ntos(npcCounter);
				newBandMembers.push_back(new Swordsman(endurance,combatStrength,0,charisma,wit,rand()%11));
				break;
			case 2: npcName = "Mercenary" + Global::ntos(npcCounter);
				newBandMembers.push_back(new Mercenary(endurance,combatStrength,0,charisma,wit,rand()%11));
				break;
			case 3: npcName = "Dwarf" + Global::ntos(npcCounter);
				newBandMembers.push_back(new Dwarf(endurance,combatStrength,0,charisma,wit,rand()%11));
				break;
			}
			newBandMembers.back()->setName(npcName);
			NPCs.push_back(newBandMembers.back());
			npcCounter++;
		}
		MapPos bandSpawn = gPlayer->getNextPosition();
		Band* newBand = new Band(newBandMembers, bandSpawn,map->getTerrainAt(bandSpawn));
		switch(newEvent){
		case 1: newBand->setBandType(Global::eventType::EVENT_MEET_SWORDSMAN);
			break;
		case 2: newBand->setBandType(Global::eventType::EVENT_MEET_MERCENARY);
			break;
		case 3: newBand->setBandType(Global::eventType::EVENT_MEET_DWARF);
			break;
		}
		newBand->generateBandName();
		bands.push_back(newBand);
		map->setBandAt(bandSpawn,newBand);
		return newBand;
	}
}
void GameCore::defragmentBands()
{
	for(int i=0;i<bands.size();i++)
		if(bands[i]->getBandSize() == 0){
			bands[i]->~Band();
			if(i<bands.size()-1)
				bands[i] = bands.back();
			bands.resize(bands.size()-1);
			i--; //because we moved last band to current i, we want to check this position again
		}
	for(int i=0;i<NPCs.size();i++){
		if(NPCs[i]->getBand() == NULL){
			std::string name = NPCs[i]->getName();
			if(name == "Dwarf")
				static_cast<Dwarf*>(NPCs[i])->~Dwarf();
			else if(name == "Swordman")
				static_cast<Swordsman*>(NPCs[i])->~Swordsman();
			else if(name == "Mercenary")
				static_cast<Mercenary*>(NPCs[i])->~Mercenary();
			else if(name == "Irate Farmer")
				static_cast<IrateFarmer*>(NPCs[i])->~IrateFarmer();
			NPCs[i] = NULL;
			if(i < NPCs.size()-1)
				NPCs[i] = NPCs.back();
			NPCs.resize(NPCs.size()-1);
			i--; //because we moved last NPC to current i, we want to check this position again
		}
	}

}
bool GameCore::executeEvent(Global::eventType &newEvent)
{
	lastEvent = newEvent;
	if(newEvent == Global::eventType::EVENT_LOST)
		gPlayer->setMoveDirection(Global::direction::DIR_NULL);
	else{
		if(newEvent == Global::eventType::EVENT_MEET_SWORDSMAN ||
			newEvent == Global::eventType::EVENT_MEET_DWARF ||
			newEvent == Global::eventType::EVENT_MEET_MERCENARY){				
				if(executeFight(generateOrGetBandAt(gPlayer->getNextPosition(),newEvent)))
					return true;
				else
					return false;
		}
		else if(newEvent = Global::eventType::EVENT_NOTHING){
			return true;
		}
		else{
			//out of bounds
		}
	}
}
int GameCore::calculateDamage(Band* playerBand, Band* enemyBand, bool playersTurn)
{
	int pPower = 0, ePower = 0, powerDifference, randomValue, turnDamage;
	for(int i=0;i<playerBand->getBandSize();i++){
		int memberPower = playerBand->getMember(i)->getCombatStrength() - playerBand->getMember(i)->getWounds();
		pPower += memberPower;
	}
	for(int i=0;i<enemyBand->getBandSize();i++){
		int eMemberPower = enemyBand->getMember(i)->getCombatStrength() - enemyBand->getMember(i)->getWounds();
		ePower += eMemberPower;
	}
	randomGenerator.randomize();
	do{
		randomValue = randomGenerator.nextUInt() + rand();
		if(randomValue < 0)
			randomValue *= (-1);
		randomValue = randomValue%11;
	}while(randomValue <= 0);
	if(playersTurn)
		powerDifference = pPower - ePower;
	else
		powerDifference = ePower - pPower;
	if(powerDifference <=-5){
		if(randomValue <= 7)
			turnDamage = 0;
		else if(randomValue == 8)
			turnDamage = 1;
		else if(randomValue == 9)
			turnDamage = 2;
		else if(randomValue == 10)
			turnDamage = 3;
	}
	else if(powerDifference >=-4 && powerDifference <=-2){
		if(randomValue <= 6)
			turnDamage = 0;
		else if(randomValue == 7 || randomValue == 8)
			turnDamage = 1;
		else if(randomValue == 9)
			turnDamage = 2;
		else if(randomValue == 10)
			turnDamage = 3;
	}
	else if(powerDifference == -1){
		if(randomValue <= 5)
			turnDamage = 0;
		else if(randomValue == 6 || randomValue == 7)
			turnDamage = 1;
		else if(randomValue == 8 || randomValue == 9)
			turnDamage = 2;
		else if(randomValue == 10)
			turnDamage = 3;
	}
	else if(powerDifference >=0 && powerDifference <=1){
		if(randomValue <= 4)
			turnDamage = 0;
		else if(randomValue == 5 || randomValue == 6 )
			turnDamage = 1;
		else if(randomValue == 7 || randomValue == 8)
			turnDamage = 2;
		else if(randomValue == 9 || randomValue == 10)
			turnDamage = 3;
	}
	else if(powerDifference >=2 && powerDifference <=4){
		if(randomValue <= 3)
			turnDamage = 0;
		else if(randomValue == 4 || randomValue == 5)
			turnDamage = 1;
		else if(randomValue == 6 || randomValue == 7 || randomValue == 8)
			turnDamage = 2;
		else if(randomValue == 9 || randomValue == 10)
			turnDamage = 3;
	}
	else if(powerDifference >=5){
		if(randomValue <= 2)
			turnDamage = 0;
		else if(randomValue == 3 || randomValue == 4)
			turnDamage = 1;
		else if(randomValue == 5 || randomValue == 6 || randomValue == 7)
			turnDamage = 2;
		else if(randomValue == 8 || randomValue == 9 || randomValue == 10)
			turnDamage = 3;
	}
	return turnDamage;
}
void GameCore::loadInitialFightScreen(Band* enemyBand)
{
	std::string eventOut;
	eventOut = "You've encountered a ";
	if(enemyBand->getBandSize() > 1){
		eventOut.append("group of ");
		switch(enemyBand->getBandType()){
		case 1: eventOut.append(Global::ntos(enemyBand->getBandSize())).append(" Swordmen "); break;
		case 2: eventOut.append(Global::ntos(enemyBand->getBandSize())).append(" Mercenaries "); break;
		case 3: eventOut.append(Global::ntos(enemyBand->getBandSize())).append(" Dwarfs "); break;
		case 4: eventOut.append(Global::ntos(enemyBand->getBandSize())).append(" Irate Farmers "); break;
		}
	}
	else{
		switch(enemyBand->getBandType()){
		case 1: eventOut.append("Swordman "); break;
		case 2: eventOut.append("Mercenary "); break;
		case 4: eventOut.append("Dwarf "); break;
		case 3: eventOut.append("Irate Farmer "); break;
		}
	}

	eventOut.append("on your path,");
	display->clear();
	display->addString(eventOut,0,20);
	display->addString("What would you like to do?\n[1] Fight\n[2] Consume Potion\n[3] Persuade",0,21);
}
void GameCore::loadFightBandStats(Band* playerBand, Band* enemyBand)
{
	display->clear();
	int tableSize;
	if(enemyBand->getBandSize() > playerBand->getBandSize())
		tableSize = enemyBand->getBandSize();
	else
		tableSize = playerBand->getBandSize();
	display->addString("Player's Band            Endurance / Wounds / Combat STR              Enemy Band",0,0);
	for(int i=0;i<tableSize;i++){
		if(i<playerBand->getBandSize())
			display->addString("[" + Global::ntos(i+1) + "] " + playerBand->getMember(i)->getName(),0,i+1);
		else
			display->addString("[" + Global::ntos(i+1) + "] ---",0,i+1);

		if(i<enemyBand->getBandSize()){
			int xPos = display->getDisplayWidth()-enemyBand->getMember(i)->getName().size()-4;
			if(i>9)
				xPos -= 1;
			display->addString(enemyBand->getMember(i)->getName() + " [" + Global::ntos(i+1) + "]",xPos,i+1);
		}
		else{
			int xPos = 73;
			if(i>9)
				xPos -= 1;
			display->addString("--- [" + Global::ntos(i+1) + "]",xPos,i+1);
		}

		if(i<playerBand->getBandSize()){
			display->addString(Global::ntos(playerBand->getMember(i)->getEndurance()),30,i+1);
			display->addString("/",32,i+1);
			display->addString(Global::ntos(playerBand->getMember(i)->getWounds()),33,i+1);
			display->addString("/",35,i+1);
			display->addString(Global::ntos(playerBand->getMember(i)->getCombatStrength()),36,i+1);
		}

		if(i<enemyBand->getBandSize()){
			display->addString(Global::ntos(enemyBand->getMember(i)->getEndurance()),43,i+1);
			display->addString("/",45,i+1);
			display->addString(Global::ntos(enemyBand->getMember(i)->getWounds()),46,i+1);
			display->addString("/",48,i+1);
			display->addString(Global::ntos(enemyBand->getMember(i)->getCombatStrength()),49,i+1);
		}
		display->addString("||",39,i+1);
	}
	float pAverageEndurance = 0, pAverageCombatStr = 0, eAverageEndurance = 0, eAverageCombatStr = 0;
	for(int i=0;i<playerBand->getBandSize();i++)
		pAverageEndurance += playerBand->getMember(i)->getEndurance();
	pAverageEndurance = pAverageEndurance/playerBand->getBandSize();
	for(int i=0;i<playerBand->getBandSize();i++)
		pAverageCombatStr += playerBand->getMember(i)->getCombatStrength();
	pAverageCombatStr = pAverageCombatStr/playerBand->getBandSize();

	for(int i=0;i<enemyBand->getBandSize();i++)
		eAverageEndurance += enemyBand->getMember(i)->getEndurance();
	eAverageEndurance = eAverageEndurance/enemyBand->getBandSize();
	for(int i=0;i<enemyBand->getBandSize();i++)
		eAverageCombatStr += enemyBand->getMember(i)->getCombatStrength();
	eAverageCombatStr = eAverageCombatStr/enemyBand->getBandSize();

	display->addString("Average Endurance:",0,13);
	display->addString("||",39,13);
	display->addString(Global::ntos(pAverageEndurance).substr(0,4),34,13);
	display->addString(Global::ntos(eAverageEndurance).substr(0,4),43,13);
	display->addString("Average Combat Strength:",0,14);
	display->addString("||",39,14);
	display->addString(Global::ntos(pAverageCombatStr).substr(0,4),34,14);
	display->addString(Global::ntos(eAverageCombatStr).substr(0,4),43,14);
}
bool GameCore::tryPersuade(Band* playerBand, Band* enemyBand)
{
	float bestCharisma = 0;
	bool persuaded = false;
	for(int i=0;i<enemyBand->getBandSize();i++)
		if(enemyBand->getMember(i)->getCharisma() > bestCharisma)
			bestCharisma = enemyBand->getMember(i)->getCharisma();
	float plrCharisma;
	switch(difficulty){
	case Global::difficultyLevels::dEASY: plrCharisma = gPlayer->getCharisma()*1.4f;	break;//likely
	case Global::difficultyLevels::dMEDIUM: plrCharisma = gPlayer->getCharisma()*1.0f;	break;//no influence
	case Global::difficultyLevels::dHARD: plrCharisma = gPlayer->getCharisma()*0.85f;	break;//difficult
	}

	if(plrCharisma > bestCharisma){
		int persuadePower = (plrCharisma-bestCharisma)*10;
		int persuadeChance = randomGenerator.nextUInt() + rand() + 100;
		persuadeChance = randomGenerator.nextUInt() + rand() + 100;
		if(persuadeChance < 0)
			persuadeChance *= (-1);
		persuadeChance = persuadeChance%100;
		if(persuadeChance < persuadePower)
			persuaded = true;
	}
	else
		persuaded = false;
	display->clear();
	if(persuaded){
		if((playerBand->getBandSize() + enemyBand->getBandSize()) > 10)
			display->addString("Your band is too big to make new home for all new members,\nyou have however convinced those who couldn't join\nto peacefully go separate way.",0,1);
		while(playerBand->getBandSize() < 10 && enemyBand->getBandSize() > 0){
			playerBand->addMember(enemyBand->getMember(enemyBand->getBandSize()-1));
			display->addString(enemyBand->getMember(enemyBand->getBandSize()-1)->getName() + " has joined your cause!",0, playerBand->getBandSize()+2);
			enemyBand->removeMember(enemyBand->getBandSize()-1);
		}
		display->addString("Press Enter to continue.",0,display->getDisplayHeight()-1);
		display->print();
		waitForEnter();
		return true;
	}
	else{
		display->addString("You have failed to persuade the enemy to join your cause!\nYou have to fight them!\n\n\nPress Enter to continue",0,20);
		display->print();
		waitForEnter();
		return false;
	}
}
bool GameCore::tryRun(Band* playerBand, Band* enemyBand, std::string &fightSummary)
{
	fightSummary = "";
	int runChance = randomGenerator.nextUInt() + rand() + 100;
	if(runChance < 0)
		runChance *= (-1);
	runChance = runChance%100;
	if(runChance < gPlayer->getWit()*10){
		//if ally's wit is bigger than players, take 50% chance for him to leave the band
		//and get magically killed by lord destructor	
		int leaveChance = 0;
		for(int i = 0;i<playerBand->getBandSize();i++){
			if(playerBand->getMember(i)->getWit() > gPlayer->getWit()){
				leaveChance = randomGenerator.nextUInt() + rand() + 100;
				if(leaveChance < 0)
					leaveChance *= (-1);
				leaveChance = leaveChance%100;
				if(leaveChance < 50){
					fightSummary.append(playerBand->getMember(i)->getName()).append(" left your band after your escape.\n");
					playerBand->removeMember(i,true);
				}
			}
		}
		Global::direction runDirection = Global::direction::DIR_NULL;
		do{
			runChance = randomGenerator.nextUInt() + rand() + 10;
			if(runChance < 0)
				runChance *= (-1);
			runChance = runChance%4;

			switch(runChance){
			case 0: runDirection = Global::direction::DIR_NORTH; break;
			case 1: runDirection = Global::direction::DIR_SOUTH; break;
			case 2: runDirection = Global::direction::DIR_WEST; break;
			case 3: runDirection = Global::direction::DIR_EAST; break;
			}
		}while(!gPlayer->setMoveDirection(runDirection));
		map->setBandAt(gPlayer->getPosition(),enemyBand);
		stats->fightsFled++;
		return true;
	}
	else{
		fightSummary.append("Your attempt to flee has failed!\nEnemy has cought up and managed to attack you!\n");
		return false;
	}
}
void GameCore::inFightDrinkPotion()
{
	Global::functionType funcType;
	display->clear();
	int potionCount = 0;
	for(int i=0;i<gPlayer->getInventory()->getInventorySize();i++){
		funcType = gPlayer->getInventory()->getItem(i)->getFunction()->getFunctionType();
		if(funcType == Global::functionType::fHEAL|| funcType == Global::functionType::fWOUND){
			potionCount++;
		}
	}
	if(potionCount == 0){
		display->addString("You haven no potions left! Press enter to go back.",0,display->getDisplayHeight()-1);
		display->print();
		waitForEnter();
		return;
	}
	//find next potion
	for(int i=0;i<gPlayer->getInventory()->getInventorySize();i++){
		funcType = gPlayer->getInventory()->getItem(i)->getFunction()->getFunctionType();
		if(funcType == Global::functionType::fHEAL|| funcType == Global::functionType::fWOUND){
			gPlayer->getInventory()->getItem(i)->use();
			if(funcType == Global::functionType::fWOUND)
				display->addString("This was a poison! Your wounds have increased by " + gPlayer->getInventory()->getItem(i)->getFunction()->getFunctionValue(),0,23);
			else
				display->addString("Your health have been restored.",0,23);
			display->addString("Press enter to go back",0,display->getDisplayHeight()-1);
			display->print();
			waitForEnter();
			return;
		}
	}
}
int GameCore::advanceFight(Band* playerBand, Band* enemyBand, bool playersTurn, std::string &fightSummary)
{
	int turnDamage = calculateDamage(playerBand,enemyBand,playersTurn);
	switch(playersTurn){
	case true: 
		if(playerBand->getBandSize() == 1)
			fightSummary.append("You have wounded: ");
		while(turnDamage > 0){
			//while we have damage to deal, deal 1 damage to random enemy from the band
			int randomMember = randomGenerator.nextUInt() + rand();
			if(randomMember < 0)
				randomMember *= (-1);
			randomMember = randomMember%enemyBand->getBandSize();
			enemyBand->getMember(randomMember)->updateWounds(1);
			turnDamage -= 1;

			//start saving all damage dealt to a string
			fightSummary.append(enemyBand->getMember(randomMember)->getName()).append(" ");
		}
		break;
	case false:
		if(enemyBand->getBandSize() == 1)
			fightSummary.append("Enemy has wounded: ");
		int randomMember = 0;
		bool gotMember = false;
		do{
			randomMember = randomGenerator.nextUInt() + rand();
			if(randomMember < 0)
				randomMember *= (-1);
			randomMember = randomMember%playerBand->getBandSize();
			if(randomMember == 0 && (difficulty == Global::difficultyLevels::dEASY || difficulty == Global::difficultyLevels::dMEDIUM) && playerBand->getBandSize() > 1)
				gotMember = false;
			else
				gotMember = true;
		}while(!gotMember);
		while(turnDamage > 0){
			playerBand->getMember(randomMember)->updateWounds(1);
			turnDamage -= 1;
			if(playerBand->getMember(randomMember) == gPlayer)
				fightSummary.append("You ");
			else
				fightSummary.append(playerBand->getMember(randomMember)->getName()).append(" ");
			break;
		}
	}
	fightSummary.append("\n");
	for(int i=0;i<playerBand->getBandSize();i++){
		if(playerBand->getMember(i)->getWounds() >= playerBand->getMember(i)->getEndurance()){
			if(playerBand->getMember(i) == gPlayer){
				stats->totalFights++;
				display->clear();
				lose();
				return -1;
			}
			if(i == 0)
				fightSummary.append("Enemy has killed: ");
			fightSummary.append(playerBand->getMember(i)->getName()).append(" ");
			stats->totalAlliesKilled++;
			playerBand->removeMember(i,true);
		}
	}
	fightSummary.append("\n");
	for(int i=0;i<enemyBand->getBandSize();i++){
		if(enemyBand->getMember(i)->getWounds() >= enemyBand->getMember(i)->getEndurance()){
			if(i == 0)
				fightSummary.append("You have killed: ");
			fightSummary.append(enemyBand->getMember(i)->getName()).append(" ");
			stats->totalEnemiesKilled++;
			enemyBand->removeMember(i,true);
		}
	}
	if(enemyBand->getBandSize() == 0){
		stats->fightsWon++;
		stats->totalFights++;
		display->clear();
		display->addString("Enemy band defeated! Well done!",0,20);
		display->print();
		Sleep(2000);
		return 1;
	}
	return 0;
}
bool GameCore::executeFight(Band* enemyBand)
{
	bool inFight = true;
	int eventIn = -1;
	loadInitialFightScreen(enemyBand);
	eventIn = getPlayerInput(1,3,false,display);
	Band *pBand = gPlayer->getBand(), *eBand = enemyBand;
	bool eventRunning = true;
	while(eventRunning){
		switch(eventIn){
		case 1:	{
			std::string fightSummary = "";
			bool playersTurn;
			int bestEnemyWit = 0;
			for(int i=0;i<eBand->getBandSize();i++)
				if(eBand->getMember(i)->getWit() > bestEnemyWit)
					bestEnemyWit = eBand->getMember(i)->getWit();

			if(bestEnemyWit < gPlayer->getWit())
				playersTurn = true;
			else if(bestEnemyWit > gPlayer->getWit())
				playersTurn = false;
			//if wit is the same, randomly select
			else{
				int randomStart = randomGenerator.nextUInt() + rand();
				randomStart = randomStart %2;
				if(randomStart = 1)
					playersTurn = true;
				else if(randomStart = 0)
					playersTurn = false;
			}
			if(playersTurn)
				fightSummary = "You can attack first!\n";
			else
				fightSummary = "Enemy attacked first!\n";
			while(inFight){
				if(playersTurn){
					int midFightTurnInInt;
					loadFightBandStats(pBand,eBand);
					display->addString(fightSummary,0,15);
					display->addString("What's your next action?\n[1] Keep fighting\n[2] Drink Potion\n[3] Run",0,21);
					midFightTurnInInt = getPlayerInput(1,3,false,display);
					switch(midFightTurnInInt){
					case 1:	/*keep fighting, do nothing*/ break;
					case 2:	inFightDrinkPotion(); break;
					case 3:{
						if(tryRun(pBand,eBand,fightSummary))
							return true;
						else
							playersTurn = false;
						   }
						   break;
					}
					if(playersTurn)
						fightSummary = "";
				}

				int fightResult = advanceFight(pBand,eBand,playersTurn,fightSummary);
				if(fightResult == -1)
					return false;
				else if(fightResult == 1)
					return true;
				if(playersTurn)
					playersTurn = false;
				else
					playersTurn = true;
			}
				}break;
		case 2:inFightDrinkPotion(); break;
		case 3:{
			if(tryPersuade(pBand,eBand))
				eventRunning = false;
			else
				eventIn = 1;
			   }break;
		}
	}
	return true;
}
void GameCore::updateSaveFiles()
{
	for(int i=0;i<10;i++)
		savedGames[i] = NULL;
	std::string path = "SaveGames", fileName = "", saveName = "", temp = "";
	int slot = 0;
	DIR *dir;
	struct dirent *ent;
	std::ifstream saveFile;
	if((dir = opendir (path.c_str())) != NULL){
		while ((ent = readdir (dir)) != NULL){
			fileName = ent->d_name;
			if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0){
				if(Global::toUpperString(fileName.substr(fileName.length()-4,4)) == "SAVE"){
					try{
						saveFile.open(path + "/" + ent->d_name);
						Global::getLeLine(saveFile,saveName);
						Global::getLeLine(saveFile,temp);
						if(Global::getLeVarName(temp) == "SLOT"){
							Global::ston(Global::getLeVarVal(temp),slot);
						}
						else throw std::string(ent->d_name);
						if(savedGames[slot] == NULL)
							savedGames[slot] = new GameSave(fileName.substr(0,fileName.length()-5),saveName,slot);
						else throw std::string(ent->d_name);
						saveFile.close();
					}
					catch(std::string s){
						MessageBox(NULL,std::string("Corrupt save file found (" + s + ")").c_str(),"Warning! Corrupt save file detected", MB_OK);
					}
				}
			}
		}
	}
}
std::string GameCore::getSaveFileList()
{
	std::string s = "";
	for(int i=0;i<10;i++){
		if(savedGames[i] != NULL)
			s.append("[" + Global::ntos(i+1) + "] " + savedGames[i]->getSaveName() + "\n");
		else
			s.append("[" +  Global::ntos(i+1) + "] Empty Slot\n");
	}
	return s;
}
bool GameCore::startLoadSequence()
{
	display->clear();
	display->addString("Select a saved slot to load ('N' to cancel).",0,0);
	display->addString(getSaveFileList(),0,1);
	int playerDecision;
	bool correctSlotSelected = false;
	while(!correctSlotSelected){
		playerDecision = getPlayerInput(1,10,true,display);
		if(playerDecision == -1)
			return false;
		else if(savedGames[playerDecision-1] != NULL){
			if((inGame && MessageBox(NULL, "Are you sure you want to load this game?\nAny unsaved progress will be lost.","Load Game?", MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK) || !inGame)
				correctSlotSelected = true;	
		}
	}
	try{
		loadGame(playerDecision-1);
	}
	catch(std::string s){
		MessageBox(NULL, s.c_str(),"Save file corrupted.", MB_OK | MB_ICONERROR);
		reset();
		return false;
	}
	return true;
}
void GameCore::loadDifficulty(std::string s)
{
	int diff;
	Global::ston(s,diff);
	switch(diff){
	case 0: difficulty = Global::difficultyLevels::dEASY; break;
	case 1: difficulty = Global::difficultyLevels::dMEDIUM;break;
	case 2: difficulty = Global::difficultyLevels::dHARD;break;
	default: throw std::string("Unknown Difficulty level specified.");break;
	}
}
void GameCore::loadStatistics(std::string s)
{
	int fightsFled = -1,fightsWon = -1,tAlliesKilled = -1,tEnemiesKilled = -1,tFights = -1,
		tFoodBought = -1,tHunt = -1,tHuntAttempts = -1,tMoney = -1,tRests = -1,tSearchAreas = -1,tTravel = -1,tTurns = -1;
	std::string playerName;
	std::istringstream ss(s);
	ss >> fightsFled >> fightsWon >> tAlliesKilled >> tEnemiesKilled >> tFights >> tFoodBought >> tHunt >> tHuntAttempts >> tMoney >> tRests >> tSearchAreas >> tTravel >> tTurns;
	if(fightsFled == -1 || fightsWon == -1 || tAlliesKilled == -1 || tEnemiesKilled == -1 || tFights == -1 || 
		tFoodBought == -1 || tHunt == -1 || tHuntAttempts == -1 || tMoney == -1 || tRests == -1 || tSearchAreas == -1 || tTravel == -1 || tTurns == -1)
		throw std::string("Save file corrupted");
	stats->fightsFled = fightsFled;
	stats->fightsWon = fightsWon;
	stats->totalAlliesKilled = tAlliesKilled;
	stats->totalEnemiesKilled = tEnemiesKilled;
	stats->totalFights = tFights;
	stats->totalFoodBought = tFoodBought;
	stats->totalHunt = tHunt;
	stats->totalHuntAttempts = tHuntAttempts;
	stats->totalMoney = tMoney;
	stats->totalRests = tRests;
	stats->totalSearchAreas = tSearchAreas;
	stats->totalTravel = tTravel;
	stats->totalTurns = tTurns;
}
void GameCore::loadWinItemLoc(std::string s)
{
	int x = -1,y = -1;
	std::istringstream ss(s);
	ss >> x >> y;
	if(x == -1 || y == -1)
		throw std::string("Save file corrupted");
	SODLocation = MapPos(x,y);
}
void GameCore::loadWinItemRand(std::string s)
{
	int x = -1,y = -1;
	std::istringstream ss(s);
	ss >> x >> y;
	if(x < -1 || x > 1 || y < -1 || y > 1)
		throw std::string("Save file corrupted");
	SODLocHintRandomizer = MapPos(x,y);
}
void GameCore::loadPlrStartLoc(std::string s)
{
	int x = -1,y = -1;
	std::istringstream ss(s);
	ss >> x >> y;
	if(x == -1 || y == -1)
		throw std::string("Save file corrupted");
	playerStartingPos = MapPos(x,y);
}
void GameCore::loadPlayer(std::ifstream &file, int &currentLine)
{
	std::string temp = "", varName = "", varVal = "";
	int x = -1, y = -1, endurance = -1, combatStr = -1, wounds = -1, charisma = -1, wit = -1, gold = -1, overrideMin = -1, overrideMax = -1;
	std::string name = "-1";
	Global::getLeLine(file,temp,currentLine);
	std::istringstream ss(temp);
	ss >> name >> x >> y >> endurance >> combatStr >> wounds >> charisma >> wit >> gold >> name;//name used twice, ID is first (in this case, "Player")
	if(x == -1 || y == -1 || endurance == -1 || combatStr == -1 || wounds == -1 || charisma == -1 || wit == -1 || gold == -1 || name == "-1")
		throw std::string("Save file corrupted");
	gPlayer = new Player(endurance,combatStr,wounds,charisma,wit,gold);
	gPlayer->setName(name);
	gPlayer->setPosition(x,y);
	Global::getLeLine(file,temp,currentLine);
	if(Global::toUpperString(temp) == "ITEMS"){
		Inventory* inventory = gPlayer->getInventory();
		while(Global::getLeLine(file,temp,currentLine) && temp != "End"){
			varName = Global::getLeVarName(temp);
			varVal = Global::getLeVarVal(temp);
			if(varName == "ITEM"){
				std::istringstream ss(varVal);
				ss >> name >> overrideMin >> overrideMax;
				inventory->copyItemToInventory(items->getItem(items->getItemIndexID(name)));
				inventory->getItem(inventory->getInventorySize()-1)->getFunction()->overrideFunctionAmount(overrideMin,overrideMax);
			}
			else throw std::string("Save file corrupted");
		}
	}
	Global::getLeLine(file,temp,currentLine);
	if(Global::toUpperString(temp) == "YES")
		gPlayer->setRested(true);
	else if(Global::toUpperString(temp) == "NO")
		gPlayer->setRested(false);
	else throw std::string("Save file corrupted");
	Global::getLeLine(file,temp,currentLine);
	if(temp != "End")
		throw std::string("Save file corrupted");
}
void GameCore::loadNPCs(std::ifstream &file, int &currentLine)
{
	std::string temp = "", varName = "", varVal = "";
	while(Global::getLeLine(file,temp,currentLine) && temp != "End"){
		varName = Global::getLeVarName(temp);
		varVal = Global::getLeVarVal(temp);
		Character* newChar = NULL;
		int x = -1, y = -1, endurance = -1, combatStr = -1, wounds = -1, charisma = -1, wit = -1, gold = -1, overrideMin = -1, overrideMax = -1;
		std::string name = "-1", ID = "";
		std::istringstream ss(temp);
		ss >> ID >> x >> y >> endurance >> combatStr >> wounds >> charisma >> wit >> gold >> name;
		if(x == -1 || y == -1 || endurance == -1 || combatStr == -1 || wounds == -1 || charisma == -1 || wit == -1 || gold == -1 || name == "-1" || ID == "-1" )
			throw std::string("Save file corrupted");
		if(name == "Mercenary")
			newChar = new Mercenary(endurance,combatStr,wounds,charisma,wit,gold);
		else if(name == "Dwarf")
			newChar = new Dwarf(endurance,combatStr,wounds,charisma,wit,gold);
		else if(name == "Swordman")
			newChar = new Swordsman(endurance,combatStr,wounds,charisma,wit,gold);
		else if(name == "Irate Farmer")
			newChar = new IrateFarmer(endurance,combatStr,wounds,charisma,wit,gold);
		else throw std::string("Save file corrupted");
		newChar->setID(ID);
		Global::getLeLine(file,temp,currentLine);
		if(Global::toUpperString(temp) == "ITEMS"){
			Inventory* inventory = newChar->getInventory();
			while(Global::getLeLine(file,temp,currentLine) && temp != "End"){
				varName = Global::getLeVarName(temp);
				varVal = Global::getLeVarVal(temp);
				if(varName == "ITEM"){
					std::istringstream ss(varVal);
					ss >> name >> overrideMin >> overrideMax;
					inventory->copyItemToInventory(items->getItem(items->getItemIndexID(name)));
					inventory->getItem(inventory->getInventorySize()-1)->getFunction()->overrideFunctionAmount(overrideMin,overrideMax);
				}
			}
		}
		else throw std::string("Save file corrupted");
		NPCs.push_back(newChar);
	}
}
void GameCore::loadBands(std::ifstream &file, int &currentLine)
{
	std::string temp = "", varName = "", varVal = "";
	while(Global::getLeLine(file,temp,currentLine) && Global::toUpperString(temp) != "END"){
		if(Global::getLeVarName(temp) == "BAND"){
			bool plrBand = false;
			Band* newBand = new Band(Global::getLeVarVal(temp));
			while(Global::getLeLine(file,temp,currentLine) && Global::toUpperString(temp) != "END"){
				if(Global::getLeVarName(temp) == "MEMBER"){
					std::string memberID = Global::getLeVarVal(temp);
					if(Global::toUpperString(memberID) == "PLAYER")
						plrBand = true;
					else{
						for(int i=0;i<NPCs.size();i++){
							if(NPCs.at(i)->getID() == memberID){
								if(plrBand)
									gPlayer->getBand()->addMember(NPCs.at(i));
								else
									newBand->addMember(NPCs.at(i));
								break;
							}
						}
					}
				} 
				else throw std::string("Save file corrupted");
			}
			if(plrBand)
				newBand->~Band();
			bands.push_back(newBand);
		}
	}
}
void GameCore::loadMap(std::ifstream &file, int &currentLine)
{
	std::string temp = "", varName = "", varVal = "";
	map->reset();
	while(Global::getLeLine(file,temp,currentLine) && Global::toUpperString(temp) != "END"){
		int tileX, tileY;
		Global::mTerrain nextTerrain;
		if(Global::getLeVarName(temp) == "TILE"){
			temp = Global::getLeVarVal(temp);
			Global::ston(temp.substr(0,2),tileX);
			temp = temp.substr(temp.find_first_of(" "),temp.npos);
			Global::ston(temp.substr(0,temp.npos),tileY);
			while(Global::getLeLine(file,temp,currentLine) && Global::toUpperString(temp) != "END"){
				if(Global::getLeVarName(temp) == "TERRAIN"){
					map->setTerrain(MapPos(tileX,tileY),extractTerrainType(Global::getLeVarVal(temp)));
				}
				else if(Global::getLeVarName(temp) == "ITEM"){
					map->getInventoryAt(MapPos(tileX,tileY))->copyItemToInventory(items->getItem(items->getItemIndex(Global::getLeVarVal(temp))));
				}
				else if(Global::getLeVarName(temp) == "BAND"){
					for(int i=0;i<bands.size();i++)
						if(bands.at(i)->getBandName() == Global::getLeVarVal(temp)){
							map->setBandAt(MapPos(tileX,tileY),bands.at(i));
							break;
						}
				}
				else if(Global::getLeVarName(temp) == "VISITED"){
					if(Global::getLeVarVal(temp) == "Yes")
						map->setVisitedAt(MapPos(tileX,tileY),true);
					//set to false by default, no need to change if "NO"
				}
				else throw std::string("Save file corrupted");
			}
		}
	}
}
void GameCore::loadGame(int saveSlot)
{
	std::string filePath = "SaveGames/" + savedGames[saveSlot]->getFilePath() + ".save";
	std::ifstream saveFile;
	saveFile.open(filePath);
	std::string temp, varName, varVal;
	int currentLine = 0;
	bool saveCorrupt = false;
	reset();
	Global::getLeLine(saveFile,temp,currentLine); //name
	Global::getLeLine(saveFile,temp,currentLine); //slot
	try{
		while(Global::getLeLine(saveFile,temp,currentLine) && temp != "End"){
			varName = Global::getLeVarName(temp);
			varVal = Global::getLeVarVal(temp);
			if(varName == "DIFFICULTY")
				loadDifficulty(varVal);
			else if(varName == "STATISTICS")
				loadStatistics(varVal);
			else if(varName == "WINITEMLOCATION")
				loadWinItemLoc(varVal);
			else if(varName == "WINLOCRANDOMIZER")
				loadWinItemRand(varVal);
			else if(varName == "STARTINGPOS")
				loadPlrStartLoc(varVal);
			else if(varName == "PLAYER")
				loadPlayer(saveFile,currentLine);
			else if(varName == "NPCS")
				loadNPCs(saveFile,currentLine);
			else if(varName == "BANDS")
				loadBands(saveFile,currentLine);
			else if(varName == "MAP")
				loadMap(saveFile,currentLine);
			else
				throw std::string("Save file corrupted.");
		}
	}
	catch(std::string s){
		s.append("\nSave file: " + filePath + "\nat line: " + Global::ntos(currentLine));
		throw s;
	}
	inGame = true;
}
void GameCore::startSaveSequence()
{
	bool playerConfirm = false;
	int playerDecision;
	display->clear();
	display->addString("Select a slot to save the game ('N' to cancel).",0,0);
	display->addString(getSaveFileList(),0,1);
	while(!playerConfirm){
		playerDecision = getPlayerInput(1,10,true,display);
		if(playerDecision == -1)
			return;
		else{
			if(savedGames[playerDecision-1] != NULL){
				if(MessageBox(NULL, "This slot already contains a file,\nwould you like to overwrite it?","Overwrite game save?", MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK)
					playerConfirm = true;
			}
			else
				playerConfirm = true;
		}
	}
	saveGame(gPlayer->getName(), playerDecision-1);
}
std::string GameCore::getSaveStringDifficulty()
{
	std::string s = std::string("Difficulty = ");
	switch(difficulty){
	case Global::difficultyLevels::dEASY: s.append("0\n"); break;
	case Global::difficultyLevels::dMEDIUM: s.append("1\n"); break;
	case Global::difficultyLevels::dHARD: s.append("2\n"); break;
	}
	return s;
}
std::string GameCore::getSaveStringMap()
{
	std::string s = "Map = COMPLEX\n";
	for(int i=0;i<20;i++)
		for(int j=0;j<20;j++){
			s.append("  Tile = ").append(Global::ntos(j)).append(" ").append(Global::ntos(i)).append("\n")
				.append("    Terrain = ").append(map->getTerrainStringAt(MapPos(j,i)) + "\n");
			Inventory* nextInventory = map->getInventoryAt(MapPos(j,i));
			for(int k=0;k<nextInventory->getInventorySize();k++)
				s.append("    Item = ").append(nextInventory->getItem(k)->getName()).append("\n");
			s.append("    Band = ");
			Band* nextBand = map->getBandAt(MapPos(j,i));
			if(map->checkBandAt(MapPos(j,i)))
				s.append(map->getBandAt(MapPos(j,i))->getBandName());
			else
				s.append("None\n");
			s.append("    Visited = ");
			if(map->checkVisitedAt(MapPos(j,i)))
				s.append("Yes\n");
			else
				s.append("No\n");
			s.append("  End\n");
		}
	s.append("End\n");
	return s;
}
std::string GameCore::getSaveStringMainLocs()
{
	std::string s = "";
	s.append("WinItemLocation = ").append(Global::ntos(SODLocation.x)).append(" " + Global::ntos(SODLocation.y)).append("\n");
	s.append("WinLocRandomizer = ").append(Global::ntos(SODLocHintRandomizer.x)).append(" " + Global::ntos(SODLocHintRandomizer.y)).append("\n");
	s.append("StartingPos = ").append(Global::ntos(playerStartingPos.x)).append(" " + Global::ntos(playerStartingPos.y)).append("\n");
	return s;
}
std::string GameCore::getSaveStringCharacters()
{
	std::string s = "";
	s.append("Player\n").append(gPlayer->exportPlayerToString());
	s.append("\nEnd\nNPCs\n");
	for(int i=0;i<NPCs.size();i++)
		s.append("  ").append(NPCs.at(i)->exportToString());
	s.append("End\n");
	return s;
}
std::string GameCore::getSaveStringBands()
{
	std::string s = "";
	int bandSize = gPlayer->getBand()->getBandSize();
	s.append("Bands\n  Band = PlayerBand\n");
	for(int i=0;i<bandSize;i++)
		s.append("    Member = ").append(gPlayer->getBand()->getMember(i)->getID()).append("\n");
	s.append("  End\n");
	for(int i=0;i<bands.size();i++){
		s.append("  Band = ").append(bands.at(i)->getBandName()).append("\n");
		bandSize = bands.at(i)->getBandSize();
		for(int j=0;j<bandSize;j++){
			s.append("    Member = ").append(bands.at(i)->getMember(j)->getID()).append("\n");
		}
		s.append("  End\n");
	}
	s.append("End\n");
	return s;
}
std::string GameCore::getSaveStringStats()
{
	std::string s = "";
	s.append("Statistics = ");
	s.append("  " + Global::ntos(stats->fightsFled) + " " + Global::ntos(stats->fightsWon) + " " + Global::ntos(stats->totalAlliesKilled) + " " + Global::ntos(stats->totalEnemiesKilled)
		+ " " + Global::ntos(stats->totalFights) + " " + Global::ntos(stats->totalFoodBought) + " " + Global::ntos(stats->totalHunt) + " " + Global::ntos(stats->totalHuntAttempts)
		+ " " + Global::ntos(stats->totalMoney) + " " + Global::ntos(stats->totalRests) + " " + Global::ntos(stats->totalSearchAreas) + " " + Global::ntos(stats->totalTravel)
		+ " " + Global::ntos(stats->totalTurns) + "\n");
	return s;
}
bool GameCore::saveGame(std::string saveName, int slot)
{
	std::string path = "SaveGames";
	std::string filePath = path + "/" + saveName + "_slot" + Global::ntos(slot) + ".save";
	std::string saveContent= saveName + "\nSLOT = " + Global::ntos(slot) + "\n"; 
	if(savedGames[slot] != NULL){
		std::remove(std::string(path + "/" + savedGames[slot]->getFilePath() + ".save").c_str());
		savedGames[slot] = NULL;
	}
	saveContent.append(getSaveStringDifficulty()).append(getSaveStringStats()).append(getSaveStringMainLocs()).append(getSaveStringCharacters()).append(getSaveStringBands()).append(getSaveStringMap());
	saveContent.append("End\n");
	std::ofstream saveFile(filePath);
	saveFile << saveContent;
	saveFile.close();
	if(savedGames[slot] != NULL)
		savedGames[slot]->~GameSave();
	savedGames[slot] = new GameSave(saveName + "_slot" + Global::ntos(slot),saveName, slot);
	return true;
}
void GameCore::copyItem(int index, Inventory* newOwner)
{
	newOwner->copyItemToInventory(items->getItem(index));
}
void GameCore::copyItem(std::string itemID, Inventory* newOwner)
{
	for(int i=0;i<items->getInventorySize();i++){
		if(items->getItem(i)->getID() == itemID)
			newOwner->copyItemToInventory(items->getItem(i));
	}
}
std::string GameCore::printPlrBandInventory()
{
	std::string temp = "", allyName = "";
	temp.append("Player's Band:" + Global::printSpace(17) + "Inventory\n");
	for(int i=0;i<gPlayer->getBand()->getBandSize();i++){
		allyName = "";
		allyName.append("[" + Global::ntos(i+1) + "] ");
		if(i<9)
			allyName.append(" ");
		allyName.append(gPlayer->getBand()->getMember(i)->getName());
		temp.append(allyName + Global::printSpace(40-allyName.size()-3));
		if(gPlayer->getBand()->getMember(i)->getInventory()->getInventorySize() < 9)
			temp.append(" ");
		temp.append(Global::ntos(gPlayer->getBand()->getMember(i)->getInventory()->getInventorySize()) + "/10\n");
	}
	return temp;
}
int GameCore::getNextRandInt()
{
	return randomGenerator.nextUInt() + rand()*7 + rand();
}
void GameCore::reset()
{
	inGame = false;
	npcCounter = 0;
	lastEvent = Global::eventType::EVENT_NOTHING;
	randomGenerator.randomize();
	srand(time(NULL));
	stats = new Statistics(0,0,0,0,0,0,0,0,0,0,0,0,0);
	for(int i=0;i<bands.size();i++){
		bands[i]->~Band();
	}
	bands.resize(0);
	defragmentBands();
	if(inGame)
		gPlayer->~Player();
	gPlayer = 0;
	playerStartingPos = MapPos(-1,-1);
	SODLocation = MapPos(-1,-1);
	lastEvent = Global::eventType::EVENT_NOTHING;
	map->reset();
}
bool GameCore::checkWin()
{
	//if player has SOD, and is at the starting position
	if(gPlayer->checkWinItem() && gPlayer->getPosition().x == playerStartingPos.x && gPlayer->getPosition().y == playerStartingPos.y)
		return true;
	else
		return false;
}
void GameCore::win()
{
	display->clear();
	display->addString("////////////////////////////////////////////////////////////////////////////////",0,6,Global::colorType::cYELLOW);
	display->addString("//////////  ////////  //  //      //      //        //       //  ////  /////////",0,7,Global::colorType::cYELLOW);
	display->addString("///////////  //////  ///  //  ////////  ////  ////  //  ///  ///  //  //////////",0,8,Global::colorType::cYELLOW);
	display->addString("////////////  ////  ////  //  ////////  ////  ////  //       ////    ///////////",0,9,Global::colorType::cYELLOW);
	display->addString("/////////////  //  /////  //  ////////  ////  ////  //  //  //////  ////////////",0,10,Global::colorType::cYELLOW);
	display->addString("//////////////    //////  //      ////  ////        //  ///  /////  ////////////",0,11,Global::colorType::cYELLOW);
	display->addString("////////////////////////////////////////////////////////////////////////////////",0,12,Global::colorType::cYELLOW);
	display->addString("You have successfully retrived the Sword of Destiny! Well done!",0,display->getDisplayHeight()-1);
	display->print();
	waitForEnter();
	inGame = false;
	reset();
}
void GameCore::lose()
{
	display->clear();
	display->addString("////////////////////////////////////////////////////////////////////////////////",0,6,Global::colorType::cRED_LIGHT);
	display->addString("///////////////       ///      //      //      ///     ///      ////////////////",0,7,Global::colorType::cRED_LIGHT);
	display->addString("///////////////  ////  //  //////  //////  //////  ///  ////  //////////////////",0,8,Global::colorType::cRED_LIGHT);
	display->addString("///////////////  ////  //    ////    ////    ////       ////  //////////////////",0,9,Global::colorType::cRED_LIGHT);
	display->addString("///////////////  ////  //  //////  //////  //////  ///  ////  //////////////////",0,10,Global::colorType::cRED_LIGHT);
	display->addString("///////////////      ////      //  //////      //  ///  ////  //////////////////",0,11,Global::colorType::cRED_LIGHT);
	display->addString("////////////////////////////////////////////////////////////////////////////////",0,12,Global::colorType::cRED_LIGHT);
	display->addString("You have been defeated\nPress Enter to exit to main menu",0,display->getDisplayHeight()-1);
	display->print();
	waitForEnter();
	reset();
}