/*
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

*/

#include "GameCore.h"
#include "Display.h"

int main()
{
	srand(time(NULL));
	ConsoleDisplay display(80,25);
	GameCore game(&display);
	try{
		game.initialSetup();
	}
	catch(std::string s){
		MessageBox(NULL, s.c_str(), "Error occured during game data loading", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}
	SetConsoleTitle("Sword of Destiny");
	system("mode 80, 26"); //25+input line
	HWND getConsole = GetConsoleWindow();
	RECT consoleRect, screenRect;
	const HWND screen = GetDesktopWindow();
	GetWindowRect(screen,&screenRect);
	GetWindowRect(getConsole, &consoleRect);
	SetWindowPos(getConsole,0,screenRect.right/2-consoleRect.right/2,screenRect.bottom/2-consoleRect.bottom/2,0,0,SWP_NOSIZE);
	game.go();
	return 0;
}
