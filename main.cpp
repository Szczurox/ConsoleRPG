#include<functional>
#include<filesystem>
#include<Windows.h>
#include<algorithm>
#include<iostream>
#include<minmax.h>
#include<direct.h>
#include<conio.h>
#include<fcntl.h>
#include<fstream>
#include<cstdarg>
#include<cstdlib>
#include<sstream>
#include<codecvt>
#include<vector>
#include<memory>
#include<array>
#include<io.h>
#include<map>

#define B_HEIGHT 30
#define B_WIDTH 100

#include"utils.hpp"
#include"menu.hpp"
#include"effective.hpp"
#include"player.hpp"
#include"items.hpp"
#include"crafting.hpp"
#include"npc.hpp"
#include"enemy.hpp"
#include"tiles.hpp"
#include"board.hpp"

ItemFactory iFactory;
EnemyFactory eFactory;
NPCFactory nFactory;

bool isRunning = true;

unsigned int seed;

int startGame(bool load);
int drawEscMenu();
bool drawDeadMenu();
void infoMenu();
void registerItems();
void registerEnemies();
void registerNPCs();
void registerAll();


int main() {
	_setmode(_fileno(stdout), _O_U8TEXT);
	setWindow((int)B_WIDTH, (int)B_HEIGHT);
	// Hide the console cursor
	std::wcout << L"\033[?25l";
	// Clear the entire screen
	system("cls");
	MenuItem title(L"Console RPG ", BRIGHT_CYAN);
	MenuItem newGame(L"New Game", BRIGHT_GREEN);
	MenuItem loadSave(L"Continue", YELLOW);
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
			res = startGame(false);
			break;
		case 1:
			res = startGame(true);
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


int startGame(bool load = false) {
	isRunning = true;
	bool isOnCurrentBoard = true;

	if (load == false)
		seed = (unsigned int)time(NULL);

	// Player Variables
	Player p;

	registerAll();
	pushRecipies(p);

	if (load) 
		seed = p.load(L"save/player.sav", iFactory);

	srand(seed);

	p.seed = seed;

	// Board Variables
	std::vector<Board> boards;

	if(load)
		for (std::wstring file : getFilesWithPrefix(L"save", L"board")) {
			Board b(B_WIDTH, B_HEIGHT, p, true);
			b.load(L"save/" + file, iFactory, eFactory, nFactory);
			boards.push_back(b);
		}

	setWindow((int)B_WIDTH + 50, (int)B_HEIGHT + 10);
	system("cls");

	while (isRunning) {
		if (!load) {
			Board b(B_WIDTH, B_HEIGHT, p);
			boards.push_back(b);
			boards[p.curFloor].boardInit();
		}
		load = false;

		boards[p.curFloor].drawBoardFull();
		if (boards.size() == 1)
			write(color(L"Version: 0.2.0\nSaving fully functional!", YELLOW).c_str());

		while (isOnCurrentBoard && isRunning) {
			char ch = 0;
			bool wait = false;
			while (ch == 0) {
				if (_kbhit() && !wait) {
					wait = true;
					ch = _getch();

					// Esc
					if (ch == 27) {
						setWindow((int)B_WIDTH, (int)B_HEIGHT);
						int res = drawEscMenu();
						setWindow((int)B_WIDTH + 50, (int)B_HEIGHT + 10);
						if (res == 1) {
							system("cls");
							write(L"Saving...");
							p.save(L"save/player.sav");
							for(int i = 0; i < boards.size(); i++)
								boards[i].save(L"save/board" + std::to_wstring(i) + L".sav");
						}
						boards[p.curFloor].drawBoardFull();
						if (res == 1) {
							boards[p.curFloor].startInfo();
							write(color(L"Game saved successfully.", YELLOW).c_str());
						}
					}
					else if (ch == 'I' || ch == 'i') {
						setWindow((int)B_WIDTH, (int)B_HEIGHT);
					    std::function<void()> info = p.showInventory();
						setWindow((int)B_WIDTH + 50, (int)B_HEIGHT + 10);
						boards[p.curFloor].drawBoardFull(info);
					}
					else if (ch == 'C' || ch == 'c') {
						setWindow((int)B_WIDTH, (int)B_HEIGHT);
						p.showCrafting();
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
					if (p.health <= 0)
						isRunning = false;

					wait = false;
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
			return 1;
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
	wchar_t seedStr[256];
	swprintf_s(seedStr, L" Seed: %d", seed);
	MenuItem text0(L" Controls", WHITE);
	MenuItem text1(L"W / Up Arrow - Up", WHITE);
	MenuItem text2(L"S / Down Arrow - Down", WHITE);
	MenuItem text3(L"A / Left Arrow - Left", WHITE);
	MenuItem text4(L"D / Right Arrow - Right", WHITE);
	MenuItem text5(L"I - Inventory", WHITE);
	MenuItem text6(L"C - Crafting", WHITE);
	MenuItem text7(L"Esc - Back / Open Escape Menu", WHITE);
	MenuItem text8(L" ", WHITE);
	MenuItem text9(L"Stats", WHITE);
	MenuItem text10(seedStr, YELLOW);
	MenuItem back(L"Back", WHITE);
	std::vector<MenuItem> options({ back });
	std::vector<MenuItem> texts({ text0, text1, text2, text3, text4, text5, text6, text7, text8, text9, text10 });
	Menu infoMenu(&options, &texts, true);
	infoMenu.open();
}

void registerItems() {
	// Tile items
	iFactory.registerItem<GoldPile>();
	// Weapons
	iFactory.registerItem<WoodenSword>();
	iFactory.registerItem<IronShortsword>();
	// Armor
	iFactory.registerItem<Gambeson>();
	iFactory.registerItem<BoneArmor>();
	// Consumables
	iFactory.registerItem<HealthPotion>();
	iFactory.registerItem<ZombieMeat>();
	iFactory.registerItem<BloodOath>();
	iFactory.registerItem<SacramentalBread>();
	// Resources
	iFactory.registerItem<Bone>();
}

void registerEnemies() {
	eFactory.registerEnemy<Skeleton>();
	eFactory.registerEnemy<Zombie>();
	eFactory.registerEnemy<Assassin>();
}

void registerNPCs() {
	nFactory.registerNPC<Shop>();
}

void registerAll() {
	registerItems();
	registerEnemies();
	registerNPCs();
}