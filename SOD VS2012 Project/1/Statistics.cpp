#include "Statistics.h"

Statistics::Statistics(int tTurns, int tRests, int tFoodBought, int tMoney, int tTravel, int tSearch, int tHunt,
					   int tMoneySpent, int tEnemiesKilled, int tAlliesKilled, int tFights, int tFightsWon, int tfightsFled,
					   int tHuntAttempts)
{
	totalTurns = tTurns;
	totalRests = tRests;
	totalFoodBought = tFoodBought;
	totalMoney = tMoney;
	totalTravel = tTravel;
	totalSearchAreas = tSearch;
	totalHunt = tHunt;
	totalHuntAttempts = tHuntAttempts;
	totalEnemiesKilled = tEnemiesKilled;
	totalAlliesKilled = tAlliesKilled;
	totalFights = tFights;
	fightsWon = tFightsWon;
	fightsFled = tfightsFled;
}