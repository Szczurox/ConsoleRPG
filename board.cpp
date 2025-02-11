#include<functional>
#include<filesystem>
#include<algorithm>
#include<iostream>
#include<direct.h>
#include<iterator>
#include<fcntl.h>
#include<fstream>
#include<cstdarg>
#include<cstdlib>
#include<sstream>
#include<codecvt>
#include<locale>
#include<vector>
#include<memory>
#include<regex>
#include<array>
#include<map>

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

Board::Board(int width, int height, Player& player, bool loading, unsigned int seed) : width(width), height(height), p(player), board(width, std::vector<Tile>(height, Tile(TileType::NOTHING, 0))), seed(seed) {
	if (loading) return;
	srand(seed);
	system("cls");
	std::wcout << L"loading...";
	std::vector<std::vector<std::shared_ptr<Room>>> rooms;
	std::vector<std::shared_ptr<Room>> curFloor;
	int lastX = 0;
	int lastY = 0;
	int lowY = 0;
	bool secret = false;
	p.curRoomNum = 0;
	// Add data for creating random amount of random rooms
	for (int i = 0; i < 30; i++) {
		int roomType = randMinMax(0, 100);
		int roomWidth = randMinMax(10, 20);
		int roomHeight = randMinMax(5, 7);
		// Space between rooms horizontally
		int bufferX = randMinMax(1, 6);
		int yRoom = lastY + roomHeight;
		int xRoom = lastX + roomWidth;
		if (yRoom >= height) break;
		if (xRoom < width) {
			if (i > 0) {
				if (roomType < 5 && !secret) {
					curFloor.push_back(std::shared_ptr<Room>(new SecretRoom()));
					secret = true;
				}
				else if (roomType < 10)
					curFloor.push_back(std::shared_ptr<Room>(new Tresury()));
				else
					curFloor.push_back(std::shared_ptr<Room>(new BasicRoom()));
			}
			else
				curFloor.push_back(std::shared_ptr<Room>(new EntranceRoom()));
			curFloor.back()->init(i, lastX, lastY, roomWidth, roomHeight, bufferX);
			lastX += roomWidth + bufferX;
			if (lowY < yRoom + 1) lowY = yRoom + 1;
		}
		else if ((int)yRoom + 1 < height) {
			lastY = lowY;
			lastX = 0;
			rooms.push_back(curFloor);
			curFloor = std::vector<std::shared_ptr<Room>>();
		}
	}

	// Summon stair room
	unsigned int hSize = (int)rooms.size();
	unsigned int lastRoomSize = (int)rooms[hSize - 1].size() - 1;
	bool canBeLastRoom = false;
	Room r = *rooms[hSize - 1][lastRoomSize].get();
	rooms[hSize - 1][lastRoomSize] = std::shared_ptr<Room>(new StairRoom(r.num, r.x, r.y, r.width, r.height, r.bufferX));

	// Create all the rooms on the board
	for (int i = 0; i < hSize; i++) {
		int dx[4] = { 0, 0, -1, 1 };
		int dy[4] = { -1, 1, 0, 0 };
		int wSize = (int)rooms[i].size();
		int wSize2 = -1;
		for (int j = 0; j < wSize; j++) {
			for (int d = 0; d < 4; d++) {
				if (d == 0 && i - 1 >= 0 && j >= rooms[(size_t)i - 1].size()) continue;
				if (d == 1 && i + 1 < hSize && j >= rooms[(size_t)i + 1].size()) continue;
				int dI = i + dy[d];
				int dJ = j + dx[d];
				if (dI >= 0 && dJ >= 0 && dI < hSize && dJ < wSize) {
					rooms[i][j]->neighbours[d] = rooms[dI][dJ]->num;
					rooms[i][j]->genDoor(d, rooms[dI][dJ]->type == RoomType::SECRET);
				}
			}
			rooms[i][j]->create(board, enemies, p.curFloor);
		}
	}
	// Add paths between rooms
	for (int i = 0; i < hSize; i++) {
		int wSize = (int)rooms[i].size();
		int wSize2 = -1;
		if (i + 1 < hSize)
			wSize2 = (int)rooms[i + 1].size();
		for (int j = 0; j < wSize; j++) {
			std::shared_ptr<Room> r1 = rooms[i][j];
			if (rooms[i][j]->doors[1] != -1 && i + 1 < hSize && j < wSize2) {
				std::shared_ptr<Room> r2 = rooms[i + 1][j];
				int r1x = r1->doors[1];
				int r1y = r1->y + r1->height - 1;
				while (r2->y - 1 > r1y || r1x != r2->doors[0]) {
					if (r2->y - 1 > r1y && board[r1x][r1y + 1].type != TileType::WALL)
						r1y += 1;
					else if (r2->doors[0] < r1x && board[r1x - 1][r1y].type != TileType::WALL)
						r1x -= 1;
					else if (r2->doors[0] > r1x && board[r1x + 1][r1y].type != TileType::WALL)
						r1x += 1;
					changeTile(r1x, r1y, Tile(TileType::PATH, r2->num, false));
				}
			}
			if (rooms[i][j]->doors[3] != -1 && j + 1 < wSize) {
				std::shared_ptr<Room> r2 = rooms[i][j + 1];
				int r1x = r1->x + r1->width - 1;
				int r1y = r1->doors[3];
				while (r2->x - 1 > r1x || r1y != r2->doors[2]) {
					if (r2->x - 1 > r1x && board[r1x + 1][r1y].type != TileType::WALL)
						r1x += 1;
					else if (r2->doors[2] > r1y && board[r1x][r1y + 1].type != TileType::WALL)
						r1y += 1;
					else if (r2->doors[2] < r1y && board[r1x][r1y - 1].type != TileType::WALL)
						r1y -= 1;
					changeTile(r1x, r1y, Tile(TileType::PATH, r2->num, false));
				}
			}
		}
	}
};

void Board::save(std::wstring fileName) {
	std::ofstream file(fileName.c_str());
	file << seed << "\n";
	for (int i = 0; i < board.size(); i++) {
		for (int j = 0; j < board[0].size(); j++)
			board[i][j].save(file);
	}
}

void Board::load(std::wstring fileName, ItemFactory& iFactory, EnemyFactory& eFactory, NPCFactory& nFactory) {
	std::ifstream file(fileName.c_str());
	file >> seed;
	for (int i = 0; i < board.size(); i++) {
		for (int j = 0; j < board[0].size(); j++) {
			std::shared_ptr<Enemy> enemy = board[i][j].load(file, iFactory, eFactory, nFactory);
			if (enemy != nullptr)
				enemies.push_back(enemy);
		}
	}
}

// Run before drawing new board for the first time
void Board::boardInit() {
	board[p.x][p.y] = Tile(TileType::PLAY, p.curRoomNum, true);
	if (p.curFloor == 0) {
		std::vector<std::shared_ptr<Item>> shop;
		shop.push_back(std::shared_ptr<Item>(new WoodenSword()));
		shop.push_back(std::shared_ptr<Item>(new Gambeson()));
		shop.push_back(std::shared_ptr<Item>(new HealthPotion()));
		p.addItem(std::shared_ptr<Item>(new Gambeson(25)));
		p.addItem(std::shared_ptr<Item>(new WoodenSword(25)));
		board[3][1] = Tile(std::shared_ptr<Item>(new HealthPotion()), 0);
		board[4][1] = Tile(std::shared_ptr<NPC>(new Shop(shop)), 0);
	}
	else
		board[4][1] = Tile(std::shared_ptr<NPC>(new Shop(p.curFloor)), 0);
}

void Board::drawBoard() {
	setCursor(0, 0);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++)
			board[j][i].draw(&p);
		std::wcout << L"\n";
	}
}

void Board::drawBoardFull(std::function<void()> info) {
	p.checkBuffs();
	drawBoard();
	makeBoxRoof(1);
	writeStats();
	makeBoxRoof(7);
	makeBoxRoof(8);
	writeStats2();
	makeBoxRoof(12);
	makeBoxRoof(13);
	writeStats3();
	makeBoxRoof(17);
	startInfo();
	info();
}

void Board::makeBoxRoof(int start) {
	setCursor(width + 1, start);
	for (int i = 0; i < boxSize; i++)
		std::wcout << L"=";
}

void Board::makeBoxPiece(int start) {
	setCursor(width + 1, start);
	std::wcout << "|";
	for (int i = 0; i < boxSize - 2; i++)
		std::wcout << L" ";
	std::wcout << "|";
	setCursor(width + 3, start);
}

void Board::writeBuff(BuffType type) {
	std::vector<std::shared_ptr<Buff>> matchingBuffs;
	std::copy_if(p.buffs.begin(), p.buffs.end(), std::back_inserter(matchingBuffs),
		[type](std::shared_ptr<Buff> b) { return b->type == type; });

	if (matchingBuffs.size() <= 0) return;

	int power = 0;
	int duration = 0;

	std::pair<std::wstring, unsigned int> buffType = matchingBuffs[0]->getType();

	for (std::shared_ptr<Buff> buff : matchingBuffs) {
		if (buff->duration > duration) 
			duration = buff->duration;
		power += buff->isNegative ? -buff->amount : buff->amount;
	}

	std::wstring powerS = (power < 0 ? color(L"-", RED) : color(L"+", GREEN)) + color(std::to_wstring(power), power < 0 ? RED : GREEN);
	write(L" (% [%] for %)", color(buffType.first, buffType.second), power, std::to_wstring(duration));
}

void Board::writeStats() {
	makeBoxPiece(2);
	write(color(L"Level: %", GREEN).c_str(), p.level);
	makeBoxPiece(3);
	write(color(L"Experience: %/%", GREEN).c_str(), p.xp, p.expForNext);
	makeBoxPiece(4);
	write(color(L"Health: %/%", RED).c_str(), p.health, p.maxHealth);
	writeBuff(BuffType::REG);
	makeBoxPiece(5);
	write(color(L"Faith: %", p.faith < 0 ? RED : (p.faith > 0 ? YELLOW : GREY)).c_str(), p.faith);
	makeBoxPiece(6);
	write(color(L"Gold: %", YELLOW).c_str(), p.gold);
}

void Board::writeStats2() {
	makeBoxPiece(9);
	write(color(L"Damage: %-%", RED).c_str(), p.minDamage, p.maxDamage);
	writeBuff(BuffType::DMG);
	makeBoxPiece(10);
	write(color(L"Defence: %", BLUE).c_str(), p.defence);
	writeBuff(BuffType::PROT);
	makeBoxPiece(11);
	write(color(L"Speed: %", YELLOW).c_str(), (p.weapon != nullptr ? p.weapon->speed : 1));
	writeBuff(BuffType::SPD);
}

void Board::writeStats3() {
	makeBoxPiece(14);
	write(color(L"Floor: %", YELLOW).c_str(), p.curFloor);
	makeBoxPiece(15);
	write(color(L"X: %", WHITE).c_str(), p.x);
	makeBoxPiece(16);
	write(color(L"Y: %", WHITE).c_str(), p.y);
}

// Update board on player action
int Board::movePlayer(char ch) {
	while (!moveDone) {};

	moveDone = false;
	int dx[4] = { 0, 0, -1, 1 };
	int dy[4] = { -1, 1, 0, 0 };

	TileType type = TileType::PLAY;
	p.attackedThisTurn = false;
	int move = -1;
	if ((ch == 'W' || ch == 'w')) move = UP;
	else if ((ch == 'S' || ch == 's')) move = DOWN;
	else if ((ch == 'A' || ch == 'a')) move = LEFT;
	else if ((ch == 'D' || ch == 'd')) move = RIGHT;
	else if ((ch == 'T' || ch == 't')) move = -2;
	else if (ch == 72) move = UP;
	else if (ch == 80) move = DOWN;
	else if (ch == 75) move = LEFT;
	else if (ch == 77) move = RIGHT;
	else move = -1;
	// Press T to wait a turn
	if (move != -2 && move != -1) {
		int tileX = p.x + dx[move];
		int tileY = p.y + dy[move];
		if (move != -2)
			type = board[tileX][tileY].type;

		std::shared_ptr<Enemy> enemy = board[tileX][tileY].enemy;
		std::shared_ptr<Item> item = board[tileX][tileY].item;

		// Interactions
		switch (type) {
		case TileType::ITEM:
		{
			int res = board[tileX][tileY].interacted(&p).result;
			startInfo();
			// Pick up item if player has free inventory space
			if (res == 0) {
				if (item->count > 1)
					write(L"Picked up % %", item->count, color(item->name.c_str(), item->colord));
				else
					write(L"Picked up %", color(item->name.c_str(), item->colord));
				changeTile(tileX, tileY, Tile(p.curRoomNum));
				writeStats();
				writeStats3();
			}
			else {
				write(color(L"Not enough inventory space.", RED).c_str());
				changeTile(p.x, p.y, Tile(item, p.curRoomNum));
				moveEntity(p.x, p.y, move);
			}
			break;
		}
		case TileType::NPC:
		{
			setWindow((int)B_WIDTH, (int)B_HEIGHT);
			InteractionResult res = board[tileX][tileY].interacted(&p);
			setWindow((int)B_WIDTH + M_WIDTH, (int)B_HEIGHT + M_HEIGHT);
			drawBoardFull();
			startInfo();
			res.soldInfo();
			break;
		}
		case TileType::PATH:
		case TileType::EMPTY:
			// Move player a tile
			moveEntity(p.x, p.y, move);
			writeStats3();
			break;
		case TileType::SECRET_DOOR:
		case TileType::DOOR:
			p.x += dx[move] * 2;
			p.y += dy[move] * 2;
			if (board[p.x][p.y].type == TileType::PATH) {
				board[tileX - dx[move]][tileY - dy[move]] = Tile(p.curRoomNum);
				board[tileX][tileY].interacted(&p, 1);
			}
			else {
				board[tileX - dx[move]][tileY - dy[move]] = Tile(TileType::PATH, p.curRoomNum, true);
				board[tileX][tileY].interacted(&p);
			}
			drawBoard();
			writeStats3();
			break;
		case TileType::STAIRS:
		{
			// Move player a tilea
			std::shared_ptr<MenuItem> option = createMenuItem(L"Are you sure you want to go lower? You can't go back.", RED);
			std::shared_ptr<MenuItem> yes = createMenuItem(L"Yes", WHITE);
			std::shared_ptr<MenuItem> no = createMenuItem(L"No", WHITE);
			std::vector<std::shared_ptr<MenuItem>> options({ yes, no });
			Menu stairsMenu(options, option, true);
			int ch = stairsMenu.open();
			if (ch == 0)
				return 1;
			drawBoardFull();
			break;
		}
		case TileType::ENEM:
			// Fighting enemy
			std::tuple<std::array<int, 5>, std::vector<std::shared_ptr<Item>>, std::pair<std::wstring, unsigned char>> results = board[tileX][tileY].interacted(&p).enemy;
			std::array<int, 5> result = std::get<0>(results);
			std::pair<std::wstring, unsigned char> effect = std::get<2>(results);
			startInfo();
			write(L"Dealt ");
			write(color(L"% damage", RED).c_str(), result[0]);
			write(L" to ");
			write(color(L"%", enemy->nameColor).c_str(), enemy->name);
			if (enemy->health > 0) {
				write(L" in % hit(s)\nRecieved ", result[1]);
				write(color(L"% damage", RED).c_str(), result[2]);
				write(L" in % hit(s).\n", result[3]);
				write(color(L"%", enemy->nameColor).c_str(), enemy->name);
				write(L" has ");
				write(color(L"% health", RED).c_str(), enemy->health);
				write(L" left");
			}
			else {
				write(L" in % hit(s) killing the enemy\nGained ", result[1]);
				write(color(L"% experience", GREEN).c_str(), result[4]);
			}
			if (effect.first != L"") {
				write(color(L"\n%", enemy->nameColor).c_str(), enemy->name);
				write(L" inflicted ");
				write(color(L"%", effect.second).c_str(), effect.first);
				write(L" on you!");
			}

			writeStats();
			writeStats2();

			if (enemy->health <= 0) {
				changeTile(tileX, tileY);
				auto it = std::find(enemies.begin(), enemies.end(), enemy);
				if (it != enemies.end()) { enemies.erase(it); }
				board[tileX][tileY].enemy = nullptr;
				placeItems(std::get<1>(results), tileX, tileY);
			}

			p.attackedThisTurn = true;
		}

		for (int i = 0; i < 4; i++) {
			int dX2 = p.x + dx[i];
			int dY2 = p.y + dy[i];
			if (isTileValid(dX2, dY2)) {
				if (board[dX2][dY2].type == TileType::PATH || board[dX2][dY2].type == TileType::DOOR || board[dX2][dY2].type == TileType::SECRET_DOOR) {
					board[dX2][dY2].isVisible = true;
					drawTile(dX2, dY2);
				}
			}
		}

		changeTile(p.x, p.y, Tile(TileType::PLAY, p.curRoomNum));

		p.checkBuffs();
		writeStats();
		writeStats2();

		moveEnemies(enemy);
	}
	else if(move != -1)
		moveEnemies();

	moveDone = true;

	return 0;
}

int Board::placeItems(std::vector<std::shared_ptr<Item>> items, int tileX, int tileY) {
	int depth = 1;
	int count = items.size() - 1;

	if (items.size() == 1)
		changeTile(tileX, tileY, Tile(items[0], -1));
	else
		while (count >= 0) {
			int x = depth;
			int y = 0;
			int p = 1 - depth;

			while (x > y) {
				y++;
				if (p <= 0)
					p = p + 2 * y + 1;
				else {
					x--;
					p = p + 2 * y - 2 * x + 1;
				}

				if (x < y)
					break;

				int dirsX[8] = { x, -x, x, -x, y, -y, y, -y };
				int dirsY[8] = { y, y, -y, -y, x, x, -x, -x };

				for (int i = 0; i < 4 && count >= 0; i++) {
					if (isTileValid(dirsX[i] + tileX, dirsY[i] + tileY, true)) {
						changeTile(dirsX[i] + tileX, dirsY[i] + tileY, Tile(items[count--], -1));
					}
					if (count < 0)
						break;
				}

				if (x != y)
					for (int i = 3; i < 8 && count >= 0; i++) {
						if (isTileValid(dirsX[i] + tileX, dirsY[i] + tileY, true)) {
							changeTile(dirsX[i] + tileX, dirsY[i] + tileY, Tile(items[count--], -1));
						}
						if (count < 0)
							break;
					}

			}
			depth++;
		}

	return 0;
}

// Enemies move AI
void Board::moveEnemies(std::shared_ptr<Enemy> fought) {
	for (int i = 0; i < enemies.size(); i++) {
		std::shared_ptr<Enemy> e = enemies[i];
		if (e->health > 0) {
			int dist = distance(e->x, e->y, p.x, p.y);
			int move = -1;
			if (dist > e->sight || p.curRoomNum != e->roomNum) {
				int d = rand() % 4;
				if (isMoveEnemyValid(e->x, e->y, d))
					move = d;
			}
			else if (dist > 1 && p.curRoomNum == e->roomNum) {
				if (p.y < e->y && isMoveEnemyValid(e->x, e->y, UP))
					move = UP;
				else if (p.y > e->y && isMoveEnemyValid(e->x, e->y, DOWN))
					move = DOWN;
				else if (p.x < e->x && isMoveEnemyValid(e->x, e->y, LEFT))
					move = LEFT;
				else if (p.x > e->x && isMoveEnemyValid(e->x, e->y, RIGHT))
					move = RIGHT;
			}
			else if (e != fought && p.curRoomNum == e->roomNum) {
				move = -1;
				// Enemy attacks
				std::tuple<std::array<int, 5>, std::vector<std::shared_ptr<Item>>, std::pair<std::wstring, unsigned char>> results = e->attacked(&p, true);
				std::array<int, 5> result = std::get<0>(results);
				std::pair<std::wstring, unsigned char> effect = std::get<2>(results);
				startInfo();
				write(L"Attacked by ");
				write(color(L"%", e->nameColor).c_str(), e->name);
				write(L"!\nRecieved ");
				write(color(L"% damage", RED).c_str(), result[2]);
				write(L" in % hit(s)\nDealt ", result[3]);
				write(color(L"% damage", RED).c_str(), result[0]);
				write(L" in % hit(s)", result[1]);
				if (e->health <= 0) {
					write(L" killing the enemy\nGained ");
					write(color(L"% experience", GREEN).c_str(), result[4]);
				}
				else {
					write(color(L"\n%", e->nameColor).c_str(), e->name);
					write(L" has ");
					write(color(L"% health", RED).c_str(), e->health);
					write(L" left");
				}
				if (effect.first != L"") {
					setCursor(0, height);
					write(color(L"%", e->nameColor).c_str(), e->name);
					write(L" inflicted ");
					write(color(L"%", effect.second).c_str(), effect.first);
					write(L" on you!");
				}
				if (!p.attackedThisTurn) p.attackedThisTurn = true;
				else write(L"\nMultiple enemies are attacking you!");
				if (e->health <= 0) {
					changeTile(e->x, e->y);
					enemies.erase(enemies.begin() + i);
					placeItems(std::get<1>(results), e->x, e->y);
				}
				writeStats();
				writeStats2();
			}
			if (move != -1)
				swapTile(e->x, e->y, move);
		}
		else changeTile(e->x, e->y);
	}
}

void Board::startInfo() {
	for (int i = 0; i < 10; i++)
		clearLine(height + i);
	setCursor(0, height);
	for (int i = 0; i < width; i++)
		std::wcout << L"=";
	setCursor(0, height + 1);
}

void Board::drawTile(int  x, int y) {
	setCursor(x, y);
	board[x][y].draw(&p);
}

// Set tile as empty if tile not specified
void Board::changeTile(int x, int y) {
	changeTile(x, y, Tile(-1));
}

// Replace tile with a different tile
void Board::changeTile(int x, int y, Tile tile) {
	int num = tile.roomNum;
	if (tile.type != TileType::PLAY && tile.roomNum == -1) {
		num = board[x][y].roomNum;
		tile.roomNum = num;
	}
	board[x][y] = tile;
	drawTile(x, y);
}

// Swap spots of two tiles
void Board::swapTile(int x, int y, int x2, int y2, bool directSwap) {
	Tile t1 = board[x][y];
	Tile t2 = board[x2][y2];
	std::shared_ptr<Item> i = board[x][y].item;
	std::shared_ptr<Enemy> e = board[x][y].enemy;
	std::shared_ptr<Item> i2 = board[x2][y2].item;
	std::shared_ptr<Enemy> e2 = board[x2][y2].enemy;

	if (i != nullptr)
		board[x2][y2] = Tile(i, t1.roomNum);
	else if (e != nullptr) {
		board[x2][y2] = Tile(e, t1.roomNum);
		e->x = x2;
		e->y = y2;
	}
	else
		board[x2][y2] = t1;

	if (i2 != nullptr)
		board[x][y] = Tile(i2, t2.roomNum);
	else if (e2 != nullptr) {
		board[x][y] = Tile(e2, t2.roomNum);
		e2->x = x;
		e2->y = y;
	}
	else
		board[x][y] = t2;

	drawTile(x, y);
	drawTile(x2, y2);
}

void Board::swapTile(int x, int y, int d, int numOfTiles) {
	int dx[4] = { 0, 0, -1, 1 };
	int dy[4] = { -1, 1, 0, 0 };
	int dX = x + dx[d] * numOfTiles, dY = y + dy[d] * numOfTiles;
	swapTile(x, y, dX, dY, true);
}

bool Board::isTileValid(int x, int y) {
	return x >= 0 && y >= 0 && x < width&& y < height;
}

bool Board::isTileValid(int x, int y, bool empty) {
	return x >= 0 && y >= 0 && x < width&& y < height&& board[x][y].type == TileType::EMPTY;
}

// 0 - Up, 1 - Down, 2 - Left, 3 - Right
bool Board::isMoveValid(int x, int y, int d) {
	int dx[4] = { 0, 0, -1, 1 };
	int dy[4] = { -1, 1, 0, 0 };
	int dX = x + dx[d];
	int dY = y + dy[d];
	return (isTileValid(dX, dY) && board[dX][dY].type == TileType::EMPTY);
}

bool Board::isMoveEnemyValid(int x, int y, int d) {
	int dx[4] = { 0, 0, -1, 1 };
	int dy[4] = { -1, 1, 0, 0 };
	int dX = x + dx[d];
	int dY = y + dy[d];
	TileType type = board[dX][dY].type;
	bool isTypeValid = (type != TileType::DOOR && type != TileType::WALL && type != TileType::NOTHING && type != TileType::ENEM && type != TileType::SECRET_DOOR);
	for (int i = 0; i < 4; i++) {
		int dX2 = dX + dx[i];
		int dY2 = dY + dy[i];
		if (isTileValid(dX2, dY2) && (board[dX2][dY2].type == TileType::DOOR || board[dX2][dY2].type == TileType::SECRET_DOOR))
			isTypeValid = false;
	}
	return (isTypeValid && isTileValid(dX, dY) && type != TileType::NPC);
}

void Board::moveEntity(int& x, int& y, int d, int numOfTiles) {
	int dx[4] = { 0, 0, -1, 1 };
	int dy[4] = { -1, 1, 0, 0 };
	swapTile(x, y, d, numOfTiles);
	x += dx[d] * numOfTiles;
	y += dy[d] * numOfTiles;
}

