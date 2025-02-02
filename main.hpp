#ifndef MAIN
#define MAIN

#include"menu.hpp"

bool isRunning = true;

unsigned int seed;

std::vector<int> seeds;

// Find the lowest missing directory number or next available
int findNextSaveDirectory(const std::wstring& dirPath);

int startGame(bool load = 0, int saveNum = -1);

void infoMenu(bool isMenu = false);

int chooseSave();

int drawEscMenu();

bool drawDeadMenu();

void registerItems();

void registerEnemies();

void registerNPCs();

void registerAll();

#endif // !MAIN
