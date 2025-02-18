#ifndef MAIN
#define MAIN

#include"menu.hpp"

enum class Character;

bool isRunning = true;

bool playMusic = true;

unsigned int seed;

std::vector<int> seeds;

// Find the lowest missing directory number or next available
int findNextSaveDirectory(const std::wstring& dirPath);

int selectNewGame();

void chooseSeed();

int startGame(bool load = 0, int saveNum = -1, Character character = Character::WARRIOR);

void infoMenu(bool isMenu = false);

void settingsMenu();

int chooseSave();

int drawEscMenu();

bool drawDeadMenu();

void registerItems();

void registerEnemies();

void registerNPCs();

void registerAll();

#endif // !MAIN
