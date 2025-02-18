#include<functional>
#include<filesystem>
#include<algorithm>
#include<iostream>
#include<direct.h>
#include<fcntl.h>
#include<fstream>
#include<cstdarg>
#include<cstdlib>
#include<sstream>
#include<codecvt>
#include<locale>
#include<thread>
#include<vector>
#include<memory>
#include<array>
#include<regex>
#include<map>

#if defined(_WIN32) || defined(_WIN64)
#pragma comment(lib, "winmm.lib")
#include<windows.h>
#include<conio.h>
#include<wmp.h>
#include<io.h>
#else
#include<sys/ioctl.h>
#include<unistd.h>
#include<termios.h>
#include<fcntl.h>
#endif

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
#include"main.hpp"

ItemFactory iFactory;
EnemyFactory eFactory;
NPCFactory nFactory;

int main() {
#if defined(_WIN32) || defined(_WIN64)
	auto _ = _setmode(_fileno(stdout), _O_U8TEXT);
#endif
	std::setlocale(LC_ALL, "en_US.UTF-8");
	setWindow((int)B_WIDTH, (int)B_HEIGHT);
	// Hide the console cursor
	std::wcout << L"\033[?25l";
	// Clear the entire screen
	system("cls");

	std::shared_ptr<MenuItem> title = createMenuItem(L"Console RPG ", BRIGHT_CYAN);
	std::shared_ptr<MenuItem> newGame = createMenuItem(L"New Game", BRIGHT_GREEN);
	std::shared_ptr<MenuItem> loadSave = createMenuItem(L"Continue", YELLOW);
	std::shared_ptr<MenuItem> settings = createMenuItem(L"Settings", GREY);
	std::shared_ptr<MenuItem> info = createMenuItem(L"Info", BRIGHT_BLUE);
	std::shared_ptr<MenuItem> exit = createMenuItem(L"Exit", RED);
	std::vector<std::shared_ptr<MenuItem>> mainOpts = { newGame, loadSave, settings, info, exit };
	Menu mainMenu(mainOpts, title, true);

	bool end = false;
	int res = 1;

	while (!end) {
		int choice = mainMenu.open();
		switch (choice)
		{
		case 0:
			res = selectNewGame();
			break;
		case 1:
			res = chooseSave();
			break;
		case 2:
			settingsMenu();
			break;
		case 3:
			infoMenu(true);
			break;
		default:
			res = 0;
			break;
		}

#if defined(_WIN32) || defined(_WIN64)
		PlaySound(NULL, NULL, SND_PURGE);
#endif

		setWindow((int)B_WIDTH, (int)B_HEIGHT);

		if (res == -1)
			end = drawDeadMenu();
		if(res == 0)
			end = true;
	}

	return 2;
}

int selectNewGame() {
	std::vector<std::shared_ptr<MenuItem>> items = {};
	std::vector<std::shared_ptr<MenuItem>> texts = {};
	std::vector<std::wstring> characters = { L"Class: Warrior", L"Class: Mage", L"Class: Rogue", L"Class: Cultist"};
	Character character = Character::WARRIOR;

	seed = -1;

	texts.push_back(createMenuItem(L"Console RPG ", BRIGHT_CYAN));
	items.push_back(createMenuItem(L"Play", BRIGHT_GREEN));
	items.push_back(createMenuItem(characters, CYAN));
	items.push_back(createMenuItem(L"Seed: Random", YELLOW));
	items.push_back(createMenuItem(L"Back", WHITE));

	Menu menu(items, texts, true);

	bool play = false;

	while (!play) {
		int choice = menu.open();
		switch (choice)
		{
		case 0:
			play = true;
			break;
		case 1:
			break;
		case 2:
			chooseSeed();
			if(seed == -1) items[1]->texts[0] = L"Seed: Random";
			else items[1]->texts[0] = L"Seed: " + std::to_wstring(seed);
			break;
		default:
			return 1;
		}
	}

	switch (items[1]->selected) {
	case 1:
		character = Character::MAGE;
		break;
	case 2:
		character = Character::ROGUE;
		break;
	case 3:
		character = Character::CULTIST;
		break;
	default:
		character = Character::WARRIOR;
	}

	return startGame(false, -1, character);
}

void chooseSeed() {
	system("cls");
	setCursor(2, 1);
	std::wstring input;
	std::wcout << "Input seed (Press [Enter] to input): ";
	std::getline(std::wcin, input);
	try {
		seed = std::stoi(input);
	}
	catch (...) {
		seed = -1;
		return;
	}
	return;
}

int extractSaveNum(std::wstring save) {
	std::wregex saveRegex(L"save(\\d+)");
	std::wsmatch match;

	if (std::regex_search(save, match, saveRegex)) {
		OutputDebugStringW(std::to_wstring(std::stoi(match[1].str())).c_str());
		return std::stoi(match[1].str());
	}
	else
		return 0;
}


// Find the lowest missing directory number or next available
int findNextSaveDirectory(const std::wstring& dirPath) {
	std::vector<std::wstring> directories = getDirectories(dirPath);
	std::vector<int> numbers;

	std::wregex saveRegex(L"save(\\d+)");

	for (const auto& dir : directories)
		numbers.push_back(extractSaveNum(dir));

	std::sort(numbers.begin(), numbers.end());

	int nextAvailable = 0;

	for (int num : numbers) {
		if (num == nextAvailable)
			nextAvailable++;
		else
			break;
	}

	return nextAvailable;
}


int manageSaveFile(std::wstring save) {
	std::vector<std::shared_ptr<MenuItem>> items = {};
	std::vector<std::shared_ptr<MenuItem>> texts = {};

	texts.push_back(createMenuItem(save, YELLOW));
	items.push_back(createMenuItem(L"Play", BRIGHT_GREEN));
	items.push_back(createMenuItem(L"Delete", RED));
	items.push_back(createMenuItem(L"Back", WHITE));

	Menu menu(items, texts, true);

	std::shared_ptr<MenuItem> title = createMenuItem(L"Are you sure you want to delete " + save + L"?", RED);
	std::shared_ptr<MenuItem> yes = createMenuItem(L"Yes", WHITE);
	std::shared_ptr<MenuItem> no = createMenuItem(L"No", WHITE);
	std::vector<std::shared_ptr<MenuItem>> mainOpts = { yes, no };

	Menu delMenu(mainOpts, title, true);

	int choice = menu.open();

	if (choice == 0)
		return 1;

	if (choice == 1) {
		choice = delMenu.open();
		if (choice == 0)
			return 2;
	}

	return -1;
}


int chooseSave() {
	if (!directoryExists(L"./saves"))
		createDirectory(L"./saves");
	std::vector<std::shared_ptr<MenuItem>> items = {};
	std::vector<std::shared_ptr<MenuItem>> texts = {};
	std::vector<int> saveNums = {};
	std::vector<std::wstring> saves = listDirectories(L"./saves");

	for (int i = 0; i < saves.size(); i++) {
		saveNums.push_back(extractSaveNum(saves[i]));
		items.push_back(createMenuItem(L"Save " + std::to_wstring(saveNums[i]), YELLOW));
	}

	items.push_back(createMenuItem(L"Back", WHITE));
	texts.push_back(createMenuItem(L"Choose Save File", CYAN));
	if(saves.size() == 0)
		texts.push_back(createMenuItem(L"No save files available", GREY));

	Menu menu(items, texts, true);

	int res = 0;
	int choice = 0;

	while (res != 1) {
		choice = menu.open();

		if (choice == -1 || choice == saves.size())
			return 1;

		res = manageSaveFile(items[choice]->texts[0]);

		if (res == 2) {
			removeDirectory(L"./saves/" + saves[choice]);
			items.erase(items.begin() + choice);
			saveNums.erase(saveNums.begin() + choice);
			saves.erase(saves.begin() + choice);
			menu.init(items);
		}
	}

	return startGame(true, choice);
}


int startGame(bool load, int saveNum, Character character) {
	std::wstring curSavePath = L"./saves/save" + std::to_wstring(saveNum) + L"/";
	isRunning = true;
	bool isOnCurrentBoard = true;
	// Is player performing some board selectable action
	bool isUsing = false;
	// Which item is player using for board selectable action
	std::shared_ptr<Item> usedItem = nullptr;

	// TODO: More sound capabilities, with a mixer, cross-platform
#if defined(_WIN32) || defined(_WIN64)
	if(playMusic)
		PlaySound(L"./sounds/game1.wav", NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
#endif

	// Player Set-Up
	Player p;

	registerAll();
	pushRecipies(p);

	if (load)
		seed = p.load(curSavePath + L"player.sav", iFactory);
	else if(seed == -1)
		seed = (unsigned int)(time(NULL));

	if (!load && character != p.character)
		p.character = character;

	p.seed = seed;

	srand(seed);

	unsigned int boardSeeds[32];
	for (int i = 0; i < 32; i++)
		boardSeeds[i] = rand() % UINT_MAX + 1;

	// Board Set-Up
	std::vector<Board> boards;

	if(load)
		for (std::wstring file : getFilesWithPrefix(curSavePath, L"board")) {
			Board b(B_WIDTH, B_HEIGHT, p, true);
			b.load(curSavePath + file, iFactory, eFactory, nFactory);
			boards.push_back(b);
		}

	setWindow((int)B_WIDTH + M_WIDTH, (int)B_HEIGHT + M_HEIGHT);
	system("cls");

	while (isRunning) {
		if (!load) {
			Board b(B_WIDTH, B_HEIGHT, p, false, boardSeeds[p.curFloor]);
			boards.push_back(b);
			boards[p.curFloor].boardInit();
		}
		load = false;

		boards[p.curFloor].drawBoardFull();
		if (boards.size() == 1)
			write(color(L"Version: 0.3.2\nCult!", YELLOW).c_str());

		while (isOnCurrentBoard && isRunning) {
			char ch = 0;
			bool wait = false;
			setWindow((int)B_WIDTH + M_WIDTH, (int)B_HEIGHT + M_HEIGHT);
			while (ch == 0) {
				if (kbhit_cross() && !wait) {
					wait = true;
					ch = getch_cross();

					// Esc
					if (ch == 27) {
						if (!isUsing) {
							setWindow((int)B_WIDTH, (int)B_HEIGHT);
							int res = drawEscMenu();
							setWindow((int)B_WIDTH + M_WIDTH, (int)B_HEIGHT + M_HEIGHT);
							if (res == 1) {
								system("cls");
								write(L"Saving...");
								if (saveNum == -1) {
									saveNum = findNextSaveDirectory(L"./saves/");
									curSavePath = L"./saves/save" + std::to_wstring(saveNum) + L"/";
									createDirectory(curSavePath);
								}
								p.save(curSavePath + L"player.sav");
								for (int i = 0; i < boards.size(); i++)
									boards[i].save(curSavePath + L"board" + std::to_wstring(i) + L".sav");
							}
							boards[p.curFloor].drawBoardFull();
							if (res == 1) {
								boards[p.curFloor].startInfo();
								write(color(L"Game saved successfully as (Save %)", YELLOW).c_str(), saveNum);
							}
							if (res == 0)
								return 1;
						}
						else {
							boards[p.curFloor].startInfo();
							isUsing = false;
						}
					}
					else if (ch == 'I' || ch == 'i') {
						setWindow((int)B_WIDTH, (int)B_HEIGHT);
					    std::pair<std::function<void()>, std::shared_ptr<Item>> info = p.showInventory();
						setWindow((int)B_WIDTH + M_WIDTH, (int)B_HEIGHT + M_HEIGHT);
						boards[p.curFloor].drawBoardFull(info.first);
						if (info.second->name != L"") {
							write(L" (Press [Enter] to choose selected and [Esc] to cancel)");
							isUsing = true;
							usedItem = info.second;
							boards[p.curFloor].selectEnemy(ch, usedItem);
						}
					}
					else if (ch == 'C' || ch == 'c') {
						setWindow((int)B_WIDTH, (int)B_HEIGHT);
						p.showCrafting();
						setWindow((int)B_WIDTH + M_WIDTH, (int)B_HEIGHT + M_HEIGHT);
						boards[p.curFloor].drawBoardFull();
					}
					else if (isUsing) {
						int res = boards[p.curFloor].selectEnemy(ch, usedItem);
						if (res == 1)
							isUsing = false;
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
				if(!isUsing)
					std::wcout << L"\033[?25l";
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
	std::shared_ptr<MenuItem> title = createMenuItem(L"", BRIGHT_CYAN);
	std::shared_ptr<MenuItem> back = createMenuItem(L"Back To Game", BRIGHT_GREEN);
	std::shared_ptr<MenuItem> save = createMenuItem(L"Save Game", YELLOW);
	std::shared_ptr<MenuItem> info = createMenuItem(L"Info", BRIGHT_BLUE);
	std::shared_ptr<MenuItem> exit = createMenuItem(L"Leave Game", RED);
	std::vector<std::shared_ptr<MenuItem>> mainOpts = { back, save, info, exit };
	Menu escMenu(mainOpts, title);

	int choice = 0;
	while (choice != -1 && isRunning) {
		choice = escMenu.open();
		switch (choice)
		{
		case 1:
			return 1;
			break;
		case 2:
			infoMenu(false);
			break;
		case 3:
		{
			std::shared_ptr<MenuItem> option = createMenuItem(L"Are you sure you want to leave?", RED);
			std::shared_ptr<MenuItem> option2 = createMenuItem(L"All unsaved progress will be lost!", RED);
			std::shared_ptr<MenuItem> yes = createMenuItem(L"Yes", WHITE);
			std::shared_ptr<MenuItem> no = createMenuItem(L"No", WHITE);
			std::vector<std::shared_ptr<MenuItem>> options({ yes, no });
			std::vector<std::shared_ptr<MenuItem>> texts({ option, option2 });
			Menu sureMenu(options, texts, true);
			int ch = sureMenu.open();
			if (ch == 0)
				isRunning = false;
			return 0;
		}
		default:
			choice = -1;
			break;
		}
	}
	return 27;
}


bool drawDeadMenu() {
	std::shared_ptr<MenuItem> title = createMenuItem(L"Game Over\n", RED);
	std::shared_ptr<MenuItem> back = createMenuItem(L"Back To Main Menu", GREEN);
	std::shared_ptr<MenuItem> exit = createMenuItem(L"Leave Game", RED);
	std::vector<std::shared_ptr<MenuItem>> mainOpts = { back, exit };
	Menu escMenu(mainOpts, title, true);

	return escMenu.open();
}


void infoMenu(bool isMenu) {
	std::vector<std::shared_ptr<MenuItem>> texts;

	texts.push_back(createMenuItem(L"Controls ", WHITE));
	texts.push_back(createMenuItem(L" ", WHITE));
	texts.push_back(createMenuItem(L"W / Up Arrow - Up", WHITE));
	texts.push_back(createMenuItem(L"S / Down Arrow - Down", WHITE));
	texts.push_back(createMenuItem(L"A / Left Arrow - Left", WHITE));
	texts.push_back(createMenuItem(L"D / Right Arrow - Right", WHITE));
	texts.push_back(createMenuItem(L"I - Inventory", WHITE));
	texts.push_back(createMenuItem(L"C - Crafting ", WHITE));
	texts.push_back(createMenuItem(L"Esc - Back / Open Escape Menu", WHITE));

	if (!isMenu) {
		texts.push_back(createMenuItem(L" ", WHITE));
		texts.push_back(createMenuItem(L"Stats ", WHITE));
		std::wstring seedStr = L" Seed: " + std::to_wstring(seed) + L" ";
		texts.push_back(createMenuItem(seedStr, YELLOW));
	}

	std::shared_ptr<MenuItem> back = createMenuItem(L"Back", WHITE);
	std::vector<std::shared_ptr<MenuItem>> options({ back });
	Menu infoMenu(options, texts, true);
	infoMenu.open();
}

void settingsMenu() {
	std::vector<std::shared_ptr<MenuItem>> texts;

	texts.push_back(createMenuItem(L"Settings ", GREY));

	std::vector<std::wstring> toggle = { L"Music: On", L"Music: Off" };
	if (!playMusic)
		toggle = { L"Music: Off", L"Music: On" };
	std::shared_ptr<MenuItem> music = createMenuItem(toggle, WHITE);
	std::shared_ptr<MenuItem> back = createMenuItem(L"Back", WHITE);
	std::vector<std::shared_ptr<MenuItem>> options({ music, back });
	Menu infoMenu(options, texts, true);
	infoMenu.open();
	if(options[0]->selected) playMusic = !playMusic;
}

void registerItems() {
	// Tile items
	iFactory.registerItem<GoldPile>();
	// Weapons
	iFactory.registerItem<WoodenSword>();
	iFactory.registerItem<IronShortsword>();
	iFactory.registerItem<BloodyBlade>();
	// Armor
	iFactory.registerItem<Gambeson>();
	iFactory.registerItem<BoneArmor>();
	iFactory.registerItem<MageRobes>();
	iFactory.registerItem<Chasuble>();
	iFactory.registerItem<CeremonialRobes>();
	// Consumables
	iFactory.registerItem<HealthPotion>();
	iFactory.registerItem<ZombieMeat>();
	iFactory.registerItem<BloodOath>();
	iFactory.registerItem<SacramentalBread>();
	// Ranged
	iFactory.registerItem<WandOfLightning>();
	iFactory.registerItem<VampiricWand>();
	iFactory.registerItem<Shuriken>();
	iFactory.registerItem<Dart>();
	// Resources
	iFactory.registerItem<Bone>();
	iFactory.registerItem<Key>();
}


void registerEnemies() {
	eFactory.registerEnemy<Skeleton>();
	eFactory.registerEnemy<Assassin>();
	eFactory.registerEnemy<Zombie>();
	eFactory.registerEnemy<Snake>();
}


void registerNPCs() {
	nFactory.registerNPC<Shop>();
	nFactory.registerNPC<DemonShop>();
	nFactory.registerNPC<Beggar>();
	nFactory.registerNPC<Smith>();
}


void registerAll() {
	registerItems();
	registerEnemies();
	registerNPCs();
}