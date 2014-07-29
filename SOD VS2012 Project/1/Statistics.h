/*
This source file is part of Sword of Destiny Game
Copyright (c) 2014 Pawel Podzus
File:        Statistics.h
*/

#ifndef STATISTICS_H
#define STATISTICS_H

class Statistics
{
public:
	int totalTurns;
	int totalRests;
	int totalFoodBought;
	int totalMoney;
	int totalTravel;
	int totalSearchAreas;
	int totalHunt;
	int totalHuntAttempts;
	int totalEnemiesKilled;
	int totalAlliesKilled;
	int totalFights;
	int fightsWon;
	int fightsFled;

	Statistics(int tTurns = 0, int tRests = 0, int tFoodBought = 0, int tMoney = 0, int tTravel = 0, int tSearch = 0, int tHunt = 0,
		int tMoneySpent = 0, int tEnemiesKilled = 0, int tAlliesKilled = 0, int tFights = 0, int tFightsWon = 0, int tfightsFled = 0,
		int tHuntAttempts = 0);
};

#endif //STATISTICS_H