#ifndef BOARD
#define BOARD

enum class TileType {
	EMPTY = 0,
	WALL = 1,
	PLAY = 2,
	ITEM = 3,
	ENEM = 4
};

class Tile {
public:
	TileType type = TileType::EMPTY;
	std::shared_ptr<Item> item = nullptr;
	std::shared_ptr<Enemy> enemy = nullptr;

	Tile() {}
	Tile(TileType type) : type(type) {}
	Tile(std::shared_ptr<Item> item) : type(TileType::ITEM), item(item) {}
	Tile(std::shared_ptr<Enemy> enemy) : type(TileType::ENEM), enemy(enemy) {}

	void draw() {
		if (type == TileType::EMPTY)
			std::cout << ".";
		if (type == TileType::WALL)
			writeColor("#", WHITE);
		if (type == TileType::PLAY)
			writeColor("\1", YELLOW);
		if (type == TileType::ITEM)
			writeColor(item->symbol, item->color);
		if (type == TileType::ENEM)
			writeColor(enemy->symbol, enemy->color);
	}

	std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>> interacted(Player* p) {
		if (type == TileType::ITEM) {
			int res = item->picked(p);
			if (res == 1) p->addItem(item);
		}
		if (type == TileType::ENEM)
			return enemy->attacked(p);
		return std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>>();
	}
};

class Board {
public:
	std::vector<std::vector<Tile>> board;
	std::vector<std::shared_ptr<Enemy>> enemies;
	Board(size_t width, size_t height, Player& player) : width(width), height(height), p(player), board(width, std::vector<Tile>(height, Tile())) {
		for (int i = 0; i < height; i++) {
			board[0][i] = Tile(TileType::WALL);
			board[width - 1][i] = Tile(TileType::WALL);
		}
		for (int i = 0; i < width; i++) {
			board[i][0] = Tile(TileType::WALL);
			board[i][height - 1] = Tile(TileType::WALL);
		}
	};

	void boardInit() {

		board[p.x][p.y] = Tile(TileType::PLAY);
		board[5][2] = Tile(std::shared_ptr<Item>(new GoldPile(10, 250)));
		board[22][6] = Tile(std::shared_ptr<Item>(new WoodenSword()));
		enemies.push_back(std::shared_ptr<Enemy>(new Enemy(32, 3)));
		for (std::shared_ptr<Enemy> e : enemies) {
			if (board[e->x][e->y].type == TileType::EMPTY)
				board[e->x][e->y] = Tile(e);
		}
	}

	void drawBoard() {
		system("cls");
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++)
				board[j][i].draw();
			std::cout << "\n";
		}
		writeStats();
	}

	void writeStats() {
		moveCursor(width + 1, 1);
		// Remove any character atrifacts that may appear
		const char* clearBuffer = "                                ";
		write(color("Level: %%", GREEN).c_str(), p.level, clearBuffer);
		moveCursor(width + 1, 2);
		write(color("Experience: %/%%", GREEN).c_str(), p.exp, p.expForNext, clearBuffer);
		moveCursor(width + 1, 3);
		write(color("Health: %/%%", RED).c_str(), p.health, p.maxHealth, clearBuffer);
		moveCursor(width + 1, 4);
		write(color("Gold: %%", YELLOW).c_str(), p.gold, clearBuffer);
	}

	// Update board on player action
	void movePlayer(char ch) {
		int tileX = (int)p.x;
		int tileY = (int)p.y;
		TileType type = TileType::PLAY;
		bool moved = false;
		if ((ch == 'W' || ch == 'w' || ch == 72)) {
			type = board[p.x][(int)p.y - 1].type;
			tileY = p.y - 1;
			moved = true;
		}
		if ((ch == 'A' || ch == 'a' || ch == 75)) {
			type = board[(int)p.x - 1][p.y].type;
			tileX = p.x - 1;
			moved = true;
		}
		if ((ch == 'S' || ch == 's' || ch == 80)) {
			type = board[p.x][(int)p.y + 1].type;
			tileY = p.y + 1;
			moved = true;
		}
		if ((ch == 'D' || ch == 'd' || ch == 77)) {
			type = board[(int)p.x + 1][p.y].type;
			tileX = p.x + 1;
			moved = true;
		}
		// Press T to wait a turn
		if ((ch == 'T' || ch == 't')) {
			moved = true;
		}

		std::shared_ptr<Item> item = board[tileX][tileY].item;
		std::shared_ptr<Enemy> enemy = board[tileX][tileY].enemy;

		// Interactions
		switch (type) {
		case TileType::ITEM:
			board[tileX][tileY].interacted(&p);
			startInfo();
			if (item->count > 1)
				write("Picked up % %", item->count, color(item->name, item->color));
			else
				write("Picked up %", color(item->name, item->color));
			writeStats();
		case TileType::EMPTY:
			// Move player a tile
			changeTile(p.x, p.y);
			p.x = tileX;
			p.y = tileY;
			changeTile(p.x, p.y, TileType::PLAY);
			break;
		case TileType::ENEM:
			// Fighting enemy
			std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>> results = board[tileX][tileY].interacted(&p);
			std::array<int, 5> result  = results.first;
			startInfo();
			write("Dealt ");
			write(color("% damage", RED).c_str(), result[0]);
			write(" to ");
			write(color("%", enemy->nameColor).c_str(), enemy->name);
			if (enemy->health > 0) {
				write(" in % hit(s)\nRecieved ", result[1]);
				write(color("% damage", RED).c_str(), result[2]);
				write(" in % hit(s).", result[3]);
			}
			else {
				write(" in % hit(s) killing the enemy\nGained ", result[1]);
				write(color("% experience", GREEN).c_str(), result[4]);
				write(".");
				changeTile(tileX, tileY);
				board[tileX][tileY].enemy = nullptr;
				for (std::shared_ptr<Item> i : results.second)
					placeItem(i, tileX, tileY);
			}
			writeStats();
		}

		if (moved)
			moveEnemies(enemy);
	}

	int placeItem(std::shared_ptr<Item> item, int tileX, int tileY) {
		if (board[tileX][tileY].type == TileType::EMPTY) {
			changeTile(tileX, tileY, Tile(item));
			return 1;
		}
		else {
			for (int i = 0; i < 4; i++)
				if (isMoveValid(tileX, tileY, i)) {
					tileY += dy[i];
					tileX += dx[i];
					changeTile(tileX, tileY, Tile(item));
					return 1;
				}
			for (int i = 0; i < 4; i++)
				if (isTileValid(tileX + dx[i], tileY + dy[i])) {
					int res = placeItem(item, tileX + dx[i], tileY + dy[i]);
					if (res == 1) return 1;
				}
		}
		return 0;
	}

	// Enemies move AI
	void moveEnemies(std::shared_ptr<Enemy> fought) {
		for (int i = 0; i < enemies.size(); i++) {
			std::shared_ptr<Enemy> e = enemies[i];
			int dist = distance(e->x, e->y, p.x, p.y);
			if (e->health <= 0) {
				enemies.erase(enemies.begin() + i);
				continue;
			}
			if (dist > e->sight) {
				int d = rand() % 4;
				if (isMoveValid(e->x, e->y, d)) {
					swapTile(e->x, e->y, d);
					e->x += dx[d];
					e->y += dy[d];
				}
			}
			else if(dist > 1) {
				if (p.y < e->y && isMoveNotWall(e->x, e->y, UP)) {
					swapTile(e->x, e->y, UP);
					e->y += dy[UP];
				}
				else if (p.y > e->y && isMoveNotWall(e->x, e->y, DOWN)) {
					swapTile(e->x, e->y, DOWN);
					e->y += dy[DOWN];
				}
				else if (p.x < e->x && isMoveNotWall(e->x, e->y, LEFT)) {
					swapTile(e->x, e->y, LEFT);
					e->x += dx[LEFT];
				}
				else if (p.x > e->x && isMoveNotWall(e->x, e->y, RIGHT)) {
					swapTile(e->x, e->y, RIGHT);
					e->x += dx[RIGHT];
				}
			}
			else if(e != fought) {
				// Enemy attacks
				std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>> results = e->attacked(&p, true);
				std::array<int, 5> result = results.first;
				startInfo();
				write("Attacked by ");
				write(color("%", e->nameColor).c_str(), e->name);
				write("!\nRecieved ");
				write(color("% damage", RED).c_str(), result[2]);
				write(" in % hit(s)\nDealt ", result[3]);
				write(color("% damage", RED).c_str(), result[0]);
				write(" in % hit(s)", result[1]);
				if(e->health <= 0) {
					write(" killing the enemy\nGained ");
					write(color("% experience", GREEN).c_str(), result[4]);
					write(".");
					changeTile(e->x, e->y);
				}
				else 
					write(".");
				writeStats();

			}
		}
	}


private:
	size_t width;
	size_t height;
	Player& p;

	void drawTile(size_t x, size_t y) {
		moveCursor(x, y);
		board[x][y].draw();
	}

	void startInfo() {
		for(int i = 0; i < 10; i++)
			clearLine(height+i);
		moveCursor(0, height);
	}

	// Set tile as empty if tile not specified
	void changeTile(int x, int y) {
		board[x][y] = Tile();
		drawTile(x, y);
	}

	// Replace tile with a different tile
	void changeTile(int x, int y, Tile tile) {
		board[x][y] = tile;
		drawTile(x, y);
	}

	// Swap spots of two tiles
	void swapTile(int x, int y, int x2, int y2) {
		std::shared_ptr<Item> i = board[x][y].item;
		std::shared_ptr<Enemy> e = board[x][y].enemy;
		std::shared_ptr<Item> i2 = board[x2][y2].item;
		std::shared_ptr<Enemy> e2 = board[x2][y2].enemy;

		if (i != nullptr)
			board[x2][y2] = Tile(i);
		else if (e != nullptr)
			board[x2][y2] = Tile(e);
		else
			board[x2][y2] = Tile();

		if (i2 != nullptr)
			board[x][y] = Tile(i2);
		else if (e2 != nullptr)
			board[x][y] = Tile(e2);
		else
			board[x][y] = Tile();

		drawTile(x, y);
		drawTile(x2, y2);
	}

	void swapTile(int x, int y, int d) {
		int dX = x + dx[d], dY = y + dy[d];
		swapTile(x, y, dX, dY);
	}

	bool isTileValid(int x, int y) {
		return x >= 0 && y >= 0 && x < width&& y < height;
	}

	// 0 - Up, 1 - Down, 2 - Left, 3 - Right
	bool isMoveValid(int x, int y, int d) {
		return (isTileValid(x + dx[d], y + dy[d]) && board[x + dx[d]][y + dy[d]].type == TileType::EMPTY);
	}

	bool isMoveNotWall(int x, int y, int d) {
		return (isTileValid(x + dx[d], y + dy[d]) && board[x + dx[d]][y + dy[d]].type != TileType::WALL);
	}
};



#endif // !BOARD
