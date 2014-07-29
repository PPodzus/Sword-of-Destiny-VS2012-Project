#include "GameHelp.h"
#include "GameCore.h"
#include "Display.h"

GameHelp::GameHelp(ConsoleDisplay* display)
{
	this->display = display;
	std::ifstream file;
	int currentLine = 0;
	file.open("Data/HelpStrings.ini");
	std::string temp = "", name = "", thisContent = "";
	while(std::getline(file,temp)){ //not using getLeLine to include spacing on all lines
		currentLine++;
		if(Global::getLeVarName(temp) == "STRING"){
			thisContent = "";
			name = Global::getLeVarVal(temp);
			for(int i=0;i<helpData.size();i++)
				if(helpData[i].first == name)
					throw std::string("Duplicate help string found in Data/HelpStrings.ini at line " + Global::ntos(currentLine) + "\nString: '" + name + "'");
			while(std::getline(file,temp)){
				if(Global::getLeVarName(temp) == "END" && Global::getLeVarVal(temp) == name)
					break;
				thisContent.append(temp).append("\n");
				currentLine++;
			}
		}
		helpData.push_back(std::make_pair(name,thisContent));
	}
	if(!(Global::getLeVarName(temp) == "END" && Global::getLeVarVal(temp) == name))
		throw std::string("Unexpected end of file in Data/HelpStrings.ini at line " + Global::ntos(currentLine));
}
bool GameHelp::getHelpString(std::string ID, std::string &s)
{
	for(int i=0;i<helpData.size();i++)
		if(helpData[i].first == ID){
			s = helpData[i].second;
			return true;
		}
	s = std::string("MISSING:'" + ID + "'");
	return false;
}
void GameHelp::prepareHelpString(std::string ID)
{
	display->clear();
	std::string s = "";
	if(getHelpString(ID,s)){
		display->addString(s,0,0);
		display->setLineColor(0,Global::colorType::cCYAN_LIGHT);
	}
	else{
		display->addString(s,0,0);
		display->setLineColor(0,Global::colorType::cRED_LIGHT);
	}
}
int GameHelp::callHelpDisplayString(std::string ID, int choiceMax, int choiceMin)
{
	prepareHelpString(ID);
	if(choiceMax != -1){
		display->addString("N to Cancel",0,display->getDisplayHeight()-1);
		return GameCore::getPlayerInput(choiceMin,choiceMax, true,display);
	}
	display->addString("Press Enter to go back.",0,display->getDisplayHeight()-1);
	display->print();
	GameCore::waitForEnter();
	return -1;
}
void GameHelp::callHelp()
{
	bool inMain = true, inTravel = true, inEncounter = true, inModding = true;
	int mainHelpIn = -123, travelHelpIn = -123, encounterHelpIn = -123, moddingHelpIn = -123;
	while(inMain){
		mainHelpIn = callHelpDisplayString("HELPMAIN",10);
		switch(mainHelpIn){
		case 1: callHelpDisplayString("HELPROUND",-1); break;
		case 2: callHelpDisplayString("HELPREST",-1);break;
		case 3: inTravel = true;
			while(inTravel){
				travelHelpIn = callHelpDisplayString("HELPTRAVEL",2); 
				switch(travelHelpIn){
				case 1: inEncounter = true;
					while(inEncounter){
						encounterHelpIn = callHelpDisplayString("HELPENCOUNTER",5);
						switch(encounterHelpIn){
						case 1: callHelpDisplayString("HELPENCOUNTERSUMMARY",-1); break;
						case 2: callHelpDisplayString("HELPENCOUNTERPERSUADE",-1); break;
						case 3: callHelpDisplayString("HELPENCOUNTERFIGHT",-1); break;
						case 4: callHelpDisplayString("HELPENCOUNTERRUN",-1); break;
						case 5: callHelpDisplayString("HELPENCOUNTERDRINK",-1); break;
						case -1: inEncounter = false; break;
						}
					}break;
				case 2: callHelpDisplayString("HELPTRAVELLOST",-1);break;
				case-1: inTravel = false;break;
				}
			}break;
		case 4: callHelpDisplayString("HELPSEARCHAREA",-1); break;
		case 5: callHelpDisplayString("HELPMANAGEINVENTORY",-1); break;
		case 6: callHelpDisplayString("HELPMANAGETEAM",-1); break;
		case 7: callHelpDisplayString("HELPMAP",-1); break;
		case 8: callHelpDisplayString("HELPHUNT",-1); break;
		case 9: callHelpDisplayString("HELPBUYFOOD",-1); break;
		case 10: inModding = true;
			while(inModding){
				moddingHelpIn = callHelpDisplayString("HELPMODDING",3);
				switch(moddingHelpIn){
				case 1: callHelpDisplayString("HELPMODDINGITEMS",-1); break;
				case 2: callHelpDisplayString("HELPMODDINGFUNCTIONS",-1); break;
				case 3: callHelpDisplayString("HELPMODDINGDIFFICULTY",-1); break;
				}break;
			}
		case -1: inMain = false;break;
		}
	}
}