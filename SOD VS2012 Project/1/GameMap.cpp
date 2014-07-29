#include "GameMap.h"
#include "Inventory.h"
#include "Character.h"

MapTile::MapTile(void)
{
	items = new Inventory();
	hasBand = false;
	wasVisited = false;
}
MapTile::MapTile(Global::mTerrain tile_type)
{
	tile = tile_type;
	hasBand = false;
	wasVisited = false;
}
void MapTile::setTerrain(Global::mTerrain tile_Type)
{
	tile = tile_Type; items = new Inventory();
}
Global::mTerrain MapTile::getTerrain()
{
	return tile;
}
Inventory* MapTile::getInventory()
{
	return items;
}
Band* MapTile::getBand()
{
	return tileBand;
}
void MapTile::setBand(Band* bandPtr)
{
	tileBand = bandPtr;
	hasBand = true;
}
bool MapTile::checkBand()
{
	if(hasBand)
		return true;
	else
		return false;
}
void MapTile::removeBand(bool killBand)
{
	hasBand = false;
}
void MapTile::reset()
{
	tile = Global::mTerrain::TILE_COUNT;
	items = new Inventory();
	tileBand = new Band();
	hasBand = false;
	wasVisited = false;
}

//Game Functions
Inventory* GameMap::getInventoryAt(MapPos position)
{
	return tiles[position.x][position.y].getInventory();
}
Band* GameMap::getBandAt(MapPos position)
{
	return tiles[position.x][position.y].getBand();
}
bool GameMap::checkBandAt(MapPos position)
{
	if(tiles[position.x][position.y].checkBand())
		return true;
	else
		return false;
}
void GameMap::setTerrain(MapPos position, Global::mTerrain tile_type)
{
	tiles[position.x][position.y].setTerrain(tile_type);
}
void GameMap::setTerrain(MapPos position, int tile_index)
{
	switch(tile_index)
	{
		case 0: tiles[position.x][position.y].setTerrain(Global::mTerrain::TILE_COUNTRYSIDE); break;
		case 1: tiles[position.x][position.y].setTerrain(Global::mTerrain::TILE_FARMLAND); break;
		case 2: tiles[position.x][position.y].setTerrain(Global::mTerrain::TILE_FOREST); break;
		case 3: tiles[position.x][position.y].setTerrain(Global::mTerrain::TILE_HILL); break;
		case 4: tiles[position.x][position.y].setTerrain(Global::mTerrain::TILE_MOUNTAIN); break;
		case 5: tiles[position.x][position.y].setTerrain(Global::mTerrain::TILE_DESERT); break;
		case 6: tiles[position.x][position.y].setTerrain(Global::mTerrain::TILE_SWAMP); break;
	}
}
void GameMap::setTerrain(MapPos position, std::string tile_name)
{
	tile_name = Global::toUpperString(tile_name);
	if(tile_name == "COUNTRYSIDE")
	{
		tiles[position.x][position.y].setTerrain(Global::mTerrain::TILE_COUNTRYSIDE);
	}
	else if(tile_name == "FARMLAND")
	{
		tiles[position.x][position.y].setTerrain(Global::mTerrain::TILE_FARMLAND);
	}
	else if(tile_name == "FOREST")
	{
		tiles[position.x][position.y].setTerrain(Global::mTerrain::TILE_HILL);
	}
	else if(tile_name == "HILL")
	{
		tiles[position.x][position.y].setTerrain(Global::mTerrain::TILE_HILL);
	}
	else if(tile_name == "MOUNTAIN")
	{
		tiles[position.x][position.y].setTerrain(Global::mTerrain::TILE_MOUNTAIN);
	}
	else if(tile_name == "DESERT")
	{
		tiles[position.x][position.y].setTerrain(Global::mTerrain::TILE_DESERT);
	}
	else if(tile_name == "SWAMP")
	{
		tiles[position.x][position.y].setTerrain(Global::mTerrain::TILE_SWAMP);
	}
}
void GameMap::setVisitedAt(MapPos position, bool whats_the_status)
{
	tiles[position.x][position.y].setVisited(whats_the_status);
}
bool GameMap::checkVisitedAt(MapPos position)
{
	if(tiles[position.x][position.y].checkVisited())
		return true;
	else
		return false;
}
std::string GameMap::getTerrainStringAt(MapPos position)
{
	std::string tileName;
	switch(tiles[position.x][position.y].getTerrain())
	{
	case Global::mTerrain::TILE_COUNTRYSIDE: tileName = "Countryside";
		break;
	case Global::mTerrain::TILE_FARMLAND: tileName = "Farmland";
		break;
	case Global::mTerrain::TILE_FOREST: tileName = "Forest";
		break;
	case Global::mTerrain::TILE_HILL: tileName = "Hill";
		break;
	case Global::mTerrain::TILE_MOUNTAIN: tileName = "Mountain";
		break;
	case Global::mTerrain::TILE_DESERT: tileName = "Desert";
		break;
	case Global::mTerrain::TILE_SWAMP: tileName = "Swamp";
		break;
	}
	return tileName;
}
int GameMap::getTerrainIntAt(MapPos position)
{
	return tiles[position.x][position.y].getTerrain();
}
Global::mTerrain GameMap::getTerrainAt(MapPos position)
{
	return tiles[position.x][position.y].getTerrain();
}
void GameMap::setBandAt(MapPos position, Band* bandPtr)
{
	tiles[position.x][position.y].setBand(bandPtr);
}
void GameMap::reset()
{
	for(int i=0;i<20;i++)
		for(int j=0;j<20;j++)
			tiles[i][j].reset();
}