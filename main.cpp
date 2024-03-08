#include<iostream>
#include<vector>
#include<cstdlib>

#include"menu.hpp"
#include"player.hpp"
#include"items.hpp"
#include"board.hpp"

#define B_HEIGHT 15
#define B_WIDTH 50

bool isRunning = true;

int startGame();
int drawEscMenu();

int main() {
	// Hide the console cursor
	std::cout << "\033[?25l";

	// Clear the entire screen
	system("cls");

    MenuItem title("Console RPG", BRIGHT_CYAN);
	MenuItem newGame("New Game", GREEN);
	MenuItem loadSave("Load Save", YELLOW);
	MenuItem info("Info", BLUE);
	MenuItem exit("Exit", RED);
    std::vector<MenuItem> mainOpts = {newGame, loadSave, info, exit};
    Menu mainMenu(mainOpts, title);

	int choice = mainMenu.open();
	switch (choice)
	{
	case 0: 
		startGame();
		break;
	case 1:
		startGame();
		break;
	default:
		break;
	}

	return 2;
}


int startGame() {
	system("cls");

	// Player Variables
	Player p;

	// Board Variables
	Board b(B_WIDTH, B_HEIGHT, p);

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
				};
				b.movePlayer(ch);
			}
		}
	}

	system("cls");

	return 0;
}

int drawEscMenu() {
	system("cls");
	MenuItem title("  ", BRIGHT_CYAN);
	MenuItem back("Back To Game", GREEN);
	MenuItem save("Save Game", YELLOW);
	MenuItem info("Info", BLUE);
	MenuItem exit("Leave Game", RED);
	std::vector<MenuItem> mainOpts = { back, save, info, exit };
	Menu escMenu(mainOpts, title);

	int choice = escMenu.open();
	switch (choice)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	default:
		isRunning = false;
		break;
	}

	return 27;
}