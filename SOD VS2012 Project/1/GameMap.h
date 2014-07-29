/*
This source file is part of Sword of Destiny Game
Copyright (c) 2014 Pawel Podzus
File:        GameMap.h
*/

#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "Global.h"
class Inventory;
class Band;

class MapTile
{
	Global::mTerrain tile;
	Inventory* items;
	//limited to one per tile
	Band* tileBand;
	bool hasBand;
	bool wasVisited;
public:
	MapTile(void);
	MapTile(Global::mTerrain tile_type);
	void setTerrain(Global::mTerrain tile_Type);
	Global::mTerrain getTerrain();
	Inventory* getInventory();
	Band* getBand();
	void setBand(Band* bandPtr);
	bool checkBand();
	void removeBand(bool killBand = false);
	void setVisited(bool status){wasVisited = status;}
	bool checkVisited(){if(wasVisited)	return true; else return false;	}
	void reset();
};

class GameMap
{
private:
	MapTile tiles[20][20];
	
public:
	GameMap(){}
	//return map tile in its standard format
	Global::mTerrain getTerrainAt(MapPos position);
	//return tile type as a string 
	std::string getTerrainStringAt(MapPos position);
	//return tile type as an it (look up mTerrain enum)
	int getTerrainIntAt(MapPos position);
	//returns a pointer to tile's inventory
	Inventory* getInventoryAt(MapPos position);
	//returns the band at specified tile
	Band* getBandAt(MapPos position);
	void setBandAt(MapPos position, Band* bandPtr);
	bool checkBandAt(MapPos position);
	//set a single map tile
	void setTerrain(MapPos position, Global::mTerrain tile_type);
	void setTerrain(MapPos position, int tile_index);
	void setTerrain(MapPos position, std::string tile_name);

	void setVisitedAt(MapPos position, bool whats_the_status);
	bool checkVisitedAt(MapPos position);

	void reset();
};

#endif // !GAMEMAP_H
