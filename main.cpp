#include<vector>
#include<cstdlib>
#include<iostream>
#include<conio.h>
#include<map>
#include<minmax.h>
#include<cstdarg>
#include<sstream>
#include<array>
#include<memory>
#include<algorithm>

#include"utils.hpp"
#include"menu.hpp"
#include"player.hpp"
#include"items.hpp"
#include"enemy.hpp"
#include"board.hpp"

#define B_HEIGHT 24
#define B_WIDTH 90

bool isRunning = true;

int startGame();
int drawEscMenu();
bool drawDeadMenu();

int main() {
	// Hide the console cursor
	std::cout << "\033[?25l";
	srand((unsigned int)time(NULL));

	// Clear the entire screen
	system("cls");

	MenuItem title("Console RPG", BRIGHT_CYAN);
	MenuItem newGame("New Game", GREEN);
	MenuItem loadSave("Load Save", YELLOW);
	MenuItem info("Info", BLUE);
	MenuItem exit("Exit", RED);
	std::vector<MenuItem*> mainOpts = { &newGame, &loadSave, &info, &exit };
	Menu mainMenu(&mainOpts, &title);

	bool end = false;
	while (!end) {
		int res = 1;
		int choice = mainMenu.open();
		switch (choice)
		{
		case 0:
			res = startGame();
			break;
		case 1:
			res = startGame();
			break;
		case 2:
			break;
		default:
			res = 0;
			break;
		}
		if (res == -1)
			end = drawDeadMenu();
		if(res == 0)
			end = true;
	}

	return 2;
}


int startGame() {
	isRunning = true;

	system("cls");

	// Player Variables
	Player p;

	// Board Variables
	Board b(B_WIDTH, B_HEIGHT, p);
	
	b.boardInit();
	b.drawBoard();
	while (isRunning) {
		char ch = 0;
		while (ch == 0) {
			if (_kbhit()) {
				ch = _getch();

				// Esc
				if (ch == 27) {
					drawEscMenu();
					b.drawBoard();
				}
				else if (ch == 'I' || ch == 'i') {
					p.showInventory();
					b.drawBoard();
				}
				else b.movePlayer(ch);
				if (p.health <= 0) {
					isRunning = false;
				}
			}
		}
	}

	system("cls");

	if(p.health <= 0)
		return -1;
	return 0;
}

int drawEscMenu() {
	MenuItem title("", BRIGHT_CYAN);
	MenuItem back("Back To Game", GREEN);
	MenuItem save("Save Game", YELLOW);
	MenuItem info("Info", BLUE);
	MenuItem exit("Leave Game", RED);
	std::vector<MenuItem*> mainOpts = { &back, &save, &info, &exit };
	Menu escMenu(&mainOpts, &title);

	int choice = escMenu.open();
	switch (choice)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		isRunning = false;
		break;
	default:
		break;
	}

	return 27;
}

bool drawDeadMenu() {
	MenuItem title("Game Over\n", RED);
	MenuItem back("Back To Main Menu", GREEN);
	MenuItem exit("Leave Game", RED);
	std::vector<MenuItem*> mainOpts = { &back, &exit };
	Menu escMenu(&mainOpts, &title);

	return escMenu.open();
}