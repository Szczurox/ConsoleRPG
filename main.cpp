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

#define B_HEIGHT 24
#define B_WIDTH 90

#include"utils.hpp"
#include"menu.hpp"
#include"player.hpp"
#include"items.hpp"
#include"enemy.hpp"
#include"tiles.hpp"
#include"board.hpp"

bool isRunning = true;

int startGame();
int drawEscMenu();
bool drawDeadMenu();
void infoMenu();

int main() {
	// Hide the console cursor
	std::cout << "\033[?25l";
	srand((unsigned int)time(NULL));
	char s[256];
	sprintf_s(s, "MODE %d,%d", (int)B_WIDTH, (int)B_HEIGHT);
	system(s);

	// Clear the entire screen
	system("cls");

	MenuItem title("Console RPG", BRIGHT_CYAN);
	MenuItem newGame("New Game", BRIGHT_GREEN);
	MenuItem loadSave("Load Save", YELLOW);
	MenuItem info("Info", BRIGHT_BLUE);
	MenuItem exit("Exit", RED);
	std::vector<MenuItem> mainOpts = { newGame, loadSave, info, exit };
	Menu mainMenu(&mainOpts, title);

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
			infoMenu();
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
	bool isOnCurrentBoard = true;

	char s[256];
	sprintf_s(s, "MODE %d,%d", (int)B_WIDTH + 50, (int)B_HEIGHT + 10);
	system(s);

	system("cls");

	// Player Variables
	Player p;

	// Board Variables
	std::vector<Board> boards;
	
	while (isRunning) {
		Board b(B_WIDTH, B_HEIGHT, p);
		boards.push_back(b);
		boards[p.curFloor].boardInit();
		boards[p.curFloor].drawBoard();
		if (boards.size() == 1) {
			boards[p.curFloor].startInfo();
			write(color("Version: 0.0.5\nSaving system isn't functional yet.", YELLOW).c_str());
		}
		while (isOnCurrentBoard && isRunning) {
			char ch = 0;
			while (ch == 0) {
				if (_kbhit()) {
					ch = _getch();

					// Esc
					if (ch == 27) {
						sprintf_s(s, "MODE %d,%d", (int)B_WIDTH, (int)B_HEIGHT);
						system(s);
						drawEscMenu();
						sprintf_s(s, "MODE %d,%d", (int)B_WIDTH + 50, (int)B_HEIGHT + 10);
						system(s);
						boards[p.curFloor].drawBoard();
					}
					else if (ch == 'I' || ch == 'i') {
						sprintf_s(s, "MODE %d,%d", (int)B_WIDTH, (int)B_HEIGHT);
						system(s);
						p.showInventory();
						sprintf_s(s, "MODE %d,%d", (int)B_WIDTH + 50, (int)B_HEIGHT + 10);
						system(s);
						boards[p.curFloor].drawBoard();
					}
					else { 
						int res = boards[p.curFloor].movePlayer(ch); 
						if (res == 1) {
							p.curFloor++;
							isOnCurrentBoard = false;
							p.x = 2;
							p.y = 1;
							p.curRoomNum = 0;
						}
					}
					if (p.health <= 0) {
						isRunning = false;
					}
				}
			}
		}
		isOnCurrentBoard = true;
	}

	system("cls");

	if(p.health <= 0)
		return -1;
	return 0;
}

int drawEscMenu() {
	MenuItem title("", BRIGHT_CYAN);
	MenuItem back("Back To Game", BRIGHT_GREEN);
	MenuItem save("Save Game", YELLOW);
	MenuItem info("Info", BRIGHT_BLUE);
	MenuItem exit("Leave Game", RED);
	std::vector<MenuItem> mainOpts = { back, save, info, exit };
	Menu escMenu(&mainOpts, title);

	int choice = 0;
	while (choice != -1) {
		choice = escMenu.open();
		switch (choice)
		{
		case 1:
			break;
		case 2:
			infoMenu();
			break;
		case 3:
		{
			MenuItem option("Are you sure you want to leave?", RED);
			MenuItem option2("All unsaved progress will be lost!", RED);
			MenuItem no("No", WHITE);
			MenuItem yes("Yes", WHITE);
			std::vector<MenuItem> options({ no, yes });
			std::vector<MenuItem> texts({ option, option2 });
			Menu sureMenu(&options, &texts);
			int ch = sureMenu.open();
			if (ch)
				isRunning = false;
			break;
		}
		default:
			choice = -1;
			break;
		}
	}
	return 27;
}

bool drawDeadMenu() {
	MenuItem title("Game Over\n", RED);
	MenuItem back("Back To Main Menu", GREEN);
	MenuItem exit("Leave Game", RED);
	std::vector<MenuItem> mainOpts = { back, exit };
	Menu escMenu(&mainOpts, title);

	return escMenu.open();
}

void infoMenu() {
	MenuItem text("W / Up Arrow - Up", WHITE);
	MenuItem text2("S / Down Arrow - Down", WHITE);
	MenuItem text3("A / Left Arrow - Left", WHITE);
	MenuItem text4("D / Right Arrow - Right", WHITE);
	MenuItem text5("E - Inventory", WHITE);
	MenuItem text6("Esc - Back / Open Escape Menu", WHITE);
	MenuItem back("Back", WHITE);
	std::vector<MenuItem> options({ back });
	std::vector<MenuItem> texts({ text, text2, text3, text4, text5, text6 });
	Menu infoMenu(&options, &texts, true);
	infoMenu.open();
}