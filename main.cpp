#include<Windows.h>
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
#include<fcntl.h>
#include<io.h>

#define B_HEIGHT 23
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
	_setmode(_fileno(stdout), _O_U8TEXT);
	srand((unsigned int)time(NULL));
	setWindow((int)B_WIDTH, (int)B_HEIGHT);
	// Hide the console cursor
	std::wcout << L"\033[?25l";
	// Clear the entire screen
	system("cls");

	MenuItem title(L"Console RPG", BRIGHT_CYAN);
	MenuItem newGame(L"New Game", BRIGHT_GREEN);
	MenuItem loadSave(L"Load Save", YELLOW);
	MenuItem info(L"Info", BRIGHT_BLUE);
	MenuItem exit(L"Exit", RED);
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

	setWindow((int)B_WIDTH + 50, (int)B_HEIGHT + 10);
	system("cls");

	// Player Variables
	Player p;

	// Board Variables
	std::vector<Board> boards;
	
	while (isRunning) {
		Board b(B_WIDTH, B_HEIGHT, p);
		boards.push_back(b);
		boards[p.curFloor].boardInit();
		boards[p.curFloor].drawBoardFull();
		if (boards.size() == 1) {
			write(color(L"Version: 0.0.5\nSaving system isn't functional yet.", YELLOW).c_str());
		}
		while (isOnCurrentBoard && isRunning) {
			char ch = 0;
			while (ch == 0) {
				if (_kbhit()) {
					ch = _getch();

					// Esc
					if (ch == 27) {
						setWindow((int)B_WIDTH, (int)B_HEIGHT);
						drawEscMenu();
						setWindow((int)B_WIDTH + 50, (int)B_HEIGHT + 10);
						boards[p.curFloor].drawBoardFull();
					}
					else if (ch == 'I' || ch == 'i') {
						setWindow((int)B_WIDTH, (int)B_HEIGHT);
						p.showInventory();
						setWindow((int)B_WIDTH + 50, (int)B_HEIGHT + 10);
						boards[p.curFloor].drawBoardFull();
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
	MenuItem title(L"", BRIGHT_CYAN);
	MenuItem back(L"Back To Game", BRIGHT_GREEN);
	MenuItem save(L"Save Game", YELLOW);
	MenuItem info(L"Info", BRIGHT_BLUE);
	MenuItem exit(L"Leave Game", RED);
	std::vector<MenuItem> mainOpts = { back, save, info, exit };
	Menu escMenu(&mainOpts, title);

	int choice = 0;
	while (choice != -1 && isRunning) {
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
			MenuItem option(L"Are you sure you want to leave?", RED);
			MenuItem option2(L"All unsaved progress will be lost!", RED);
			MenuItem no(L"No", WHITE);
			MenuItem yes(L"Yes", WHITE);
			std::vector<MenuItem> options({ no, yes });
			std::vector<MenuItem> texts({ option, option2 });
			Menu sureMenu(&options, &texts);
			int ch = sureMenu.open();
			if (ch == 1)
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
	MenuItem title(L"Game Over\n", RED);
	MenuItem back(L"Back To Main Menu", GREEN);
	MenuItem exit(L"Leave Game", RED);
	std::vector<MenuItem> mainOpts = { back, exit };
	Menu escMenu(&mainOpts, title);

	return escMenu.open();
}

void infoMenu() {
	MenuItem text(L"W / Up Arrow - Up", WHITE);
	MenuItem text2(L"S / Down Arrow - Down", WHITE);
	MenuItem text3(L"A / Left Arrow - Left", WHITE);
	MenuItem text4(L"D / Right Arrow - Right", WHITE);
	MenuItem text5(L"E - Inventory", WHITE);
	MenuItem text6(L"Esc - Back / Open Escape Menu", WHITE);
	MenuItem back(L"Back", WHITE);
	std::vector<MenuItem> options({ back });
	std::vector<MenuItem> texts({ text, text2, text3, text4, text5, text6 });
	Menu infoMenu(&options, &texts, true);
	infoMenu.open();
}