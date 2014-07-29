////////////////////////////////////////////////////////////////////
//      //  //////  //       //       //      /////       //      //
//  //////  //////  //  ///  //  ///  //  ///  ////  ///  //  //////
//      //  //  //  //  ///  //       //  ///  ////  ///  //     ///
//////  //  //  //  //  ///  //  //  ///  ///  ////  ///  //  //////
//      //          //       //  ///  //      /////       //  //////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//      ///      //      //      //      //  ////  //  ////  ///////
//  ///  //  //////  ////////  //////  ////    //  //   //  ////////
//  ///  //    ////      ////  //////  ////        ////    /////////
//  ///  //  //////////  ////  //////  ////  //    /////  //////////
//      ///      //      ////  ////      //  ////  /////  //////////
////////////////////////////////////////////////////////////////////
-------------------------------------------------
-------------------------------------------------
-------------------------------------------------
Table of Contents
1. About the game
1. Installation
2. Game Help
3. Modding
4. License
-------------------------------------------------
---------------1. About the game-----------------
-------------------------------------------------

Sword of Destiny is a simple turn-based adventure game that runs
in console.
The player is tasked with retrieving the mysterious 
weapon while fighting randomly encountered opponents, strategically
planning his adventure and managing his band to survive.
He can also decide to break the balance of the game completely by 
changing some of the game mechanics and creating your own items for
the game, so feel free to mod and edit the game in any way you want.

-------------------------------------------------
-----------------2. Installation-----------------
-------------------------------------------------

None. The game runs from .exe provided

-------------------------------------------------
-------------------3. Game Help------------------
-------------------------------------------------

Note: Help is also available in Main Menu in Pause Menu during gameplay
Help Table of Contents
 - Round summary
 - Round Action: Rest
 - Round Action: Travel
 - Round Action: Travel Event: Lost
 - Round Action: Travel Event: Encounter
 - Round Action: Travel Event: Encounter: Summary
 - Round Action: Travel Event: Encounter: Persuade
 - Round Action: Travel Event: Encounter: Fight
 - Round Action: Travel Event: Encounter: Run
 - Round Action: Travel Event: Encounter: Drink Potion
 - Round Action: Search Area
 - Round Action: Manage Player Inventory
 - Round Action: Manage Player Band
 - Round Action: Map preview
 - Round Action: Hunting
 - Round Action: Buying Food

-------------------------------------------------
-------------------------------------------------

Help - Round Summary

Round Summary is the per-round preview of current game status.
A typical summary inlcudes any infromation about events that happened last
round, such as hunting or travel results.
Below that you will find the following:
Current Position - x/y position of the Player on the map
Day - the number of days/turns the Player has played so far
Currently on - the type of terrain Player is currently on
Band Status - tells Player if the Player's team is rested and healthy or 
              wounded
Food remaining - the amount of food the band currently has, displays low and
                 critial states
Current Objectve - objectives the Player currently has to complete in order
                   to progress/win

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Rest

Resting heals wounds and gives a boost to Player's abilities,
but only if none of the Player's allies are currently starving

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Travel

If Player decides to travel, there's a chance a random event will happen 
before he reaches his destination. This includes getting lost and
encountering a band of enemies on your path. Player has no influence
on these events happening.
[1] Travel Event: Encounter
[2] Travel Event: Lost

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Travel Event: Lost

Every time the Player travels, depending on the type of terrain there's
a chance he and his band will get lost, resulting in not reaching target 
destination, but consuming day's worth of food.

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Travel Event: Encounter

When traveling, there's a chance to enounter one of three types of enemies 
on your path:
Swordmen, Dwarfs or Mercenaries
They don't differ in power, but appear in different terrain types and 
in size between 1 and 10.
Upon encountering an enemy band, Player can choose to fight them, 
or try persuading them to join the band.
During the fight, Player has option to run or drink potions if he has any.
 - Encounter: Summary
 - Encounter: Persuade
 - Encounter: Fight
 - Encounter: Run
 - Encounter: Drink Potion

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Travel Event: Encounter: Summary

After each fight turn, Player will be presented with a summary including:
Preview of both bands:
////////////////////////////////////////////////////////////////////////////////
Band1                    Endurance/Wounds/Combat Strength                  Band2
Members                    x/x/x      ||     x/x/x                       Members
...                                                                          ...

Average Endurance
Average Combat Strength
////////////////////////////////////////////////////////////////////////////////
As well as list of damaged and killed characters during this round

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Travel Event: Encounter: Persuade

When an enemy is encountered, Player has a choice of persuading them to join
his band, at risk of being attacked first if he fails.
This can be used even if Player's band is full to try avoiding a fight.
This chance is dependant on Player's wit and game difficulty, so keeping wit
items might be good tactic if you don't have a strong team.

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Travel Event: Encounter: Fight

When Player decides to fight or continue fighting, the game will calculate
damage to deal to the enemy band based on band's combat strength and wounds,
dealing up to 3 damage. It will then switch to enemy's turn deal damage
to Player.
If any characters are wounded or killed, a fight summary will be updated and
Player will be returned to Encounter Summary in order to take next action.

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Travel Event: Encounter: Run

If for any reason Player decides he has no chance to win the fight, he can
attempt running from the fight. This is decided by random chance and Player's
wit.
If Player fails to run, the enemy takes the turn to attack.
If he succeeds however, he will be moved in a random direction.

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Travel Event: Encounter: Drink Potion

During a combat, if Player's wounds are too high, he can decide to drink 
potions from his inventory. This process however is automated and might
result in drinking a posion instead.

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Search Area

When chosen, Player will search current position for items that might be
around. The chance of finding non-quest items is based on band's average
wit and random chance.
Quest items are always found.

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Manage Team

This round action does not advance to next turn upon finishing,
and can be used to optimize the band's performance during the adventure.
You can use this action to dismiss any members of the team, or trade items
with them.

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Manage Inventory

This round action does not advance to next turn upon finishing,
and is useful if your inventory gets filled with items you don't need 
during the adventure, or when you want to look at any of your items in detail.
This can be useful to avoid keeping items with negative effects.

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Map

This simple round action does not advance to next turn upon finishing,
as it allows Player to look up his current position in the world, decide
where to travel next and help visualizing the current state of the game.

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Hunt

Player can decide to hunt in order to get ready for traveling longer distances
in one go. In order to do so, Player cannot be on a desert. 
Otherwise, the chance of successfully hunting is based on the band's wit, 
combat strength and wounds.
Player must be careful however, as hunting in farmland may cause local farmers
to attack you.

-------------------------------------------------
-------------------------------------------------

Help - Round Action: Buy Food

When on farmlands, Player can decide to buy food for the band to improve
survivability of the group, and allow longer travels without need to 
re-supply. In order to buy food, Player, or any other band member needs
gold. 
Supplies for 1 day cost 1 gold.

-------------------------------------------------
--------------------4. Modding-------------------
-------------------------------------------------
Sword of Destiny can be have parts of its mechanics modified.
Main 3 areas include:
 - Items
 - Functions
 - Game Difficulty Settings

-------------------------------------------------
-------------------------------------------------

Modding: Items

You can freely add, edit or remove items to the game, with the exception of
items inside Data/Items CoreItems.ini (ItemIDs "SOD", "SODClue")
All items must be inside the Data/Items folder in a .ini file.
Basic Item Strucure:
Item = ItemName - displayed in inventory
  ItemID = ID - unique identifier
  Description = text - short description of the item
  Weight = integer - how much weight the item weighs
  DropData - data containing drop chance details
    DropChance = 0-100%
    TerrainModifier = COUNTRYSIDE/FARMLAND/FOREST/HILL/MOUNTAIN/DESERT/SWAMP
      ModifierValue = 0-100% - overrides default drop chance
    End
  End
  Function = FunctionName - name of one of the functions in Functions.ini
End

-------------------------------------------------
-------------------------------------------------

Modding: Functions

All items in game have some sort of functionality which is defined in the
Functions.ini file. They use the following structure:
Function = FunctionID - unique identifier
  Type = WinItem/HintItem/Wound/heal/ModifyCombatStrength/ModifyCharisma/
         ModifyWit/ModifyEndurance/DropMoney
  Type: WinItem
    Condition = StartReturn - requires player to return to start tile
                Pickup - wins the game on pickup
  Type: HintItem
    Hint = WintItemLocation - displays approximate location of the 
                              Sword of Destiny on the map
           "DescriptionContent" - additional info displayed in item details
  Type: ModifyX OR Heal OR Wound OR Food
    Amount = Integer
  Type: DropMoney
    AmountMin = Integer - minimum drop amount
    AmountMax = Integer - maximum drop amount
Example:
Function = BroadSwordFunction
  Type = ModifyCombatStrength
    Amount = 3
  End
End

-------------------------------------------------
-------------------------------------------------

Modding: Game Difficulty Settings

For each game difficulty level (easy/medium/hard) there are few settings 
that can be changed to improve or completely distrupt the balance and
meaning of these difficulty levels.
The structure is simple:
Difficulty = EASY/MEDIUM/HARD
  PLAYER/ALLY/ENEMY
    Stats = Integer Integer Integer Integer 
        (Endurance CombatStrength Wit Charisma)
    Item = ItemID (up to 10 Item statements, anything over 10 is ignored)
  End
End
The structure requires all difficulties with all character types in, 
even if they are empty (i.e. immediately followed by End statement)

-------------------------------------------------
--------------------5. License-------------------
-------------------------------------------------

Sword of Destiny 1.0
Copyright (C) 2014  Pawel Podzus

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Author:      Pawel Podzus
mail:        podzus.pawel@gmail.com