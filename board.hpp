#ifndef BOARD
#define BOARD

class Board {
public:
	std::vector<std::vector<Tile>> board;
	std::vector<std::shared_ptr<Enemy>> enemies;
	Board(int width, int height, Player& player) : width(width), height(height), p(player), board(width, std::vector<Tile>(height, Tile(TileType::NOTHING, 0))) {
		std::wcout << L"loading...";
		std::vector<std::vector<std::shared_ptr<Room>>> rooms;
		std::vector<std::shared_ptr<Room>> curFloor;
		int lastX = 0;
		int lastY = 0;
		int lowY = 0;
		p.curRoomNum = 0;
		// Add data for creating random amount of random rooms
		for (int i = 0; i < 20; i++) {
			int roomType = randMinMax(1, 100);
			int roomWidth = randMinMax(7, 20);
			int roomHeight = randMinMax(4, 7);
			// Space between rooms horizontally
			int bufferX = randMinMax(1, 6);
			int yRoom = lastY + roomHeight; 
			int xRoom = lastX + roomWidth;
			if (yRoom >= height) break;
			if (xRoom < width) {
				if (i > 0) {
					if (roomType < 10)
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
						rooms[i][j]->genDoor(d);
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

	// Run before drawing new board for the first time
	void boardInit() {
		board[p.x][p.y] = Tile(TileType::PLAY, p.curRoomNum, true);
		if (p.curFloor == 0) {
			std::vector<std::shared_ptr<Item>> shop;
			shop.push_back(std::shared_ptr<Item>(new WoodenSword()));
			shop.push_back(std::shared_ptr<Item>(new Gambeson()));
			shop.push_back(std::shared_ptr<Item>(new HealthPotion()));
			p.addItem(std::shared_ptr<Item>(new Gambeson(10)));
			p.addItem(std::shared_ptr<Item>(new WoodenSword(10)));
			board[3][2] = Tile(std::shared_ptr<Item>(new HealthPotion()), 0);
			board[4][2] = Tile(std::shared_ptr<NPC>(new Shop(shop)), 0);
		}
		else {
			std::vector<std::shared_ptr<Item>> shop;
			shop.push_back(std::shared_ptr<Item>(new WoodenSword()));
			shop.push_back(std::shared_ptr<Item>(new Gambeson()));
			shop.push_back(std::shared_ptr<Item>(new HealthPotion()));
			shop.push_back(std::shared_ptr<Item>(new HealthPotion()));
			shop.push_back(std::shared_ptr<Item>(new HealthPotion()));
			shop.push_back(std::shared_ptr<Item>(new HealthPotion()));
			shop.push_back(std::shared_ptr<Item>(new HealthPotion()));
			shop.push_back(std::shared_ptr<Item>(new HealthPotion()));
			shop.push_back(std::shared_ptr<Item>(new HealthPotion()));
			shop.push_back(std::shared_ptr<Item>(new HealthPotion()));
			board[4][2] = Tile(std::shared_ptr<NPC>(new Shop(shop)), 0);
		}
	}

	void drawBoard() {
		setCursor(0, 0);
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++)
				board[j][i].draw(&p);
			std::wcout << L"\n";
		}
	}

	void drawBoardFull() {
		drawBoard();
		writeStats();
		writeStats2();
		writeStats3();
		startInfo();
	}

	void writeStats() {
		setCursor(width + 1, 1);
		// Remove any character atrifacts that may appear
		std::wcout << L"============================";
		setCursor(width + 1, 2);
		std::wcout << L"|                          |";
		setCursor(width + 3, 2);
		write(color(L"Level: %\n", GREEN).c_str(), p.level);
		setCursor(width + 1, 3);
		std::wcout << L"|                          |";
		setCursor(width + 3, 3);
		write(color(L"Experience: %/%\n", GREEN).c_str(), p.xp, p.expForNext);
		setCursor(width + 1, 4);
		std::wcout << L"|                          |";
		setCursor(width + 3, 4);
		write(color(L"Health: %/%\n", RED).c_str(), p.health, p.maxHealth);
		setCursor(width + 1, 5);
		std::wcout << L"|                          |";
		setCursor(width + 3, 5);
		write(color(L"Gold: %\n", YELLOW).c_str(), p.gold);
		setCursor(width + 1, 6);
		std::wcout << L"============================";
	}

	void writeStats2() {
		setCursor(width + 1, 8);
		// Remove any character atrifacts that may appear
		std::wcout << L"============================";
		setCursor(width + 1, 9);
		std::wcout << L"|                          |";
		setCursor(width + 3, 9);
		write(color(L"Damage: %-%\n", RED).c_str(), p.minDamage, p.maxDamage);
		setCursor(width + 1, 10);
		std::wcout << L"|                          |";
		setCursor(width + 3, 10);
		write(color(L"Defence: %\n", BLUE).c_str(), p.defence);
		setCursor(width + 1, 11);
		std::wcout << L"|                          |";
		setCursor(width + 3, 11);
		write(color(L"Speed: %\n", YELLOW).c_str(), (p.weapon != nullptr ? p.weapon->speed : 1));
		setCursor(width + 1, 12);
		std::wcout << L"============================";
	}

	void writeStats3() {
		setCursor(width + 1, 14);
		// Remove any character atrifacts that may appear
		std::wcout << L"============================";
		setCursor(width + 1, 15);
		std::wcout << L"|                          |";
		setCursor(width + 3, 15);
		write(color(L"Floor: %\n", YELLOW).c_str(), p.curFloor);
		setCursor(width + 1, 16);
		std::wcout << L"|                          |";
		setCursor(width + 3, 16);
		write(color(L"X: %\n", WHITE).c_str(), p.x);
		setCursor(width + 1, 17);
		std::wcout << L"|                          |";
		setCursor(width + 3, 17);
		write(color(L"Y: %\n", WHITE).c_str(), p.y);
		setCursor(width + 1, 18);
		std::wcout << L"============================";
	}

	// Update board on player action
	int movePlayer(char ch) {
		TileType type = TileType::PLAY;
		int move = -1;
		if ((ch == 'W' || ch == 'w')) move = UP;
		else if ((ch == 'S' || ch == 's')) move = DOWN;
		else if ((ch == 'A' || ch == 'a')) move = LEFT;
		else if ((ch == 'D' || ch == 'd')) move = RIGHT;
		else if ((ch == 'T' || ch == 't'))
			move = -2;
		else
			switch (_getch()) {
			case 72:
				move = UP;
				break;
			case 80:
				move = DOWN;
				break;
			case 77:
				move = RIGHT;
				break;
			case 75:
				move = LEFT;
				break;
			}
		int tileX = p.x + dx[move];
		int tileY = p.y + dy[move];
		type = board[tileX][tileY].type;
		// Press T to wait a turn

		std::shared_ptr<Enemy> enemy = board[tileX][tileY].enemy;
		if (move != -2) {
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
						write(L"Picked up % %", item->count, color(item->name, item->colord));
					else
						write(L"Picked up %", color(item->name, item->colord));
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
				InteractionResult res = board[tileX][tileY].interacted(&p);
				SoldInfo info = res.soldInfo;
				drawBoardFull();
				if (res.result >= 0) {
					startInfo();
					write(L"Bought ");
					write(color(info.name, info.color).c_str());
					write(L" for ");
					write(color(L"% gold", YELLOW).c_str(), info.cost);
				}
				else if(res.result == -2) {
					startInfo();
					write(L"You don't have enough % to buy %!", color(L"gold", YELLOW), color(info.name, info.color));
				}
				break;
			}
			case TileType::PATH:
			case TileType::EMPTY:
				// Move player a tile
				moveEntity(p.x, p.y, move);
				writeStats3();
				break;
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
				MenuItem option(L"Are you sure you want to go lower? You can't go back.", RED);
				MenuItem yes(L"Yes", WHITE);
				MenuItem no(L"No", WHITE);
				std::vector<MenuItem> options({ yes, no });
				Menu stairsMenu(&options, option);
				int ch = stairsMenu.open();
				if (ch == 0)
					return 1;
				drawBoard();
				break;
			}
			case TileType::ENEM:
				// Fighting enemy
				std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>> results = board[tileX][tileY].interacted(&p).enemy;
				std::array<int, 5> result = results.first;
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
					changeTile(tileX, tileY);
					auto it = std::find(enemies.begin(), enemies.end(), enemy);
					if (it != enemies.end()) { enemies.erase(it); }
					board[tileX][tileY].enemy = nullptr;
					placeItems(results.second, tileX, tileY);
				}
				writeStats();
				writeStats2();
			}
		}

		for (int i = 0; i < 4; i++) {
			int dX2 = p.x + dx[i];
			int dY2 = p.y + dy[i];
			if (isTileValid(dX2, dY2)) {
				if (board[dX2][dY2].type == TileType::PATH || board[dX2][dY2].type == TileType::DOOR) {
					board[dX2][dY2].isVisible = true;
					drawTile(dX2, dY2);
				}
			}
		}

		changeTile(p.x, p.y, Tile(TileType::PLAY, p.curRoomNum));

		if (move != -1)
			moveEnemies(enemy);
		
		return 0;
	}

	int placeItems(std::vector<std::shared_ptr<Item>> items, int tileX, int tileY) {
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

					if(x != y)
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
	void moveEnemies(std::shared_ptr<Enemy> fought) {
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
					std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>> results = e->attacked(&p, true);
					std::array<int, 5> result = results.first;
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
						changeTile(e->x, e->y);
						enemies.erase(enemies.begin() + i);
						placeItems(results.second, e->x, e->y);
					}
					else {
						write(color(L"\n%", e->nameColor).c_str(), e->name);
						write(L" has ");
						write(color(L"% health", RED).c_str(), e->health);
						write(L" left");
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

	void startInfo() {
		for (int i = 0; i < 10; i++)
			clearLine(height + i);
		setCursor(0, height);
		for (int i = 0; i < width; i++)
			std::wcout << L"=";
		setCursor(0, height + 1);
	}

private:
	int width;
	int height;
	Player& p;

	void drawTile(int  x, int y) {
		setCursor(x, y);
		board[x][y].draw(&p);
	}

	// Set tile as empty if tile not specified
	void changeTile(int x, int y) {
		changeTile(x, y, Tile(-1));
	}

	// Replace tile with a different tile
	void changeTile(int x, int y, Tile tile) {
		int num = tile.roomNum;
		if (tile.type != TileType::PLAY && tile.roomNum == -1) {
			num = board[x][y].roomNum;
			tile.roomNum = num;
		}
		board[x][y] = tile;
		drawTile(x, y);
	}

	// Swap spots of two tiles
	void swapTile(int x, int y, int x2, int y2, bool directSwap) {
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

	void swapTile(int x, int y, int d, int numOfTiles = 1) {
		int dX = x + dx[d] * numOfTiles, dY = y + dy[d] * numOfTiles;
		swapTile(x, y, dX, dY, true);
	}

	bool isTileValid(int x, int y) {
		return x >= 0 && y >= 0 && x < width && y < height;
	}

	bool isTileValid(int x, int y, bool empty) {
		return x >= 0 && y >= 0 && x < width && y < height && board[x][y].type == TileType::EMPTY;
	}

	// 0 - Up, 1 - Down, 2 - Left, 3 - Right
	bool isMoveValid(int x, int y, int d) {
		int dX = x + dx[d];
		int dY = y + dy[d];
		return (isTileValid(dX, dY) && board[dX][dY].type == TileType::EMPTY);
	}

	bool isMoveEnemyValid(int x, int y, int d) {
		int dX = x + dx[d];
		int dY = y + dy[d];
		TileType type = board[dX][dY].type;
		bool isTypeValid = (type != TileType::DOOR && type != TileType::WALL && type != TileType::NOTHING && type != TileType::ENEM);
		for (int i = 0; i < 4; i++) {
			int dX2 = dX + dx[i];
			int dY2 = dY + dy[i];
			if (isTileValid(dX2, dY2) && board[dX2][dY2].type == TileType::DOOR)
				isTypeValid = false;
		}
		return (isTypeValid && isTileValid(dX, dY) && type != TileType::NPC);
	}

	void moveEntity(int& x, int& y, int d, int numOfTiles = 1) {
		swapTile(x, y, d, numOfTiles);
		x += dx[d] * numOfTiles;
		y += dy[d] * numOfTiles;
	}
};



#endif // !BOARD
