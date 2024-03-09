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
	Item* item = nullptr;
	Enemy* enemy = nullptr;

	Tile() {}
	Tile(TileType type) : type(type) {}
	Tile(Item* item) : type(TileType::ITEM), item(item) {}
	Tile(Enemy* enemy) : type(TileType::ENEM), enemy(enemy) {}

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

	std::array<int, 4> interacted(Player* p) {
		if (type == TileType::ITEM) {
			item->picked(p);
		}
		if (type == TileType::ENEM) {
			return enemy->attacked(p);
		}
		return std::array<int, 4>();
	}
};

class Board {
public:
	std::vector<std::vector<Tile*>> board;
	Board(size_t width, size_t height, Player& player) : width(width), height(height), p(player), board(height, std::vector<Tile*>(width, new Tile())) {
		for (int i = 0; i < height; i++) {
			board[i][0] = new Tile(TileType::WALL);
			board[i][width - 1] = new Tile(TileType::WALL);
		}
		for (int i = 0; i < width; i++) {
			board[0][i] = new Tile(TileType::WALL);
			board[height - 1][i] = new Tile(TileType::WALL);
		}
	};

	void movePlayer(char ch) {
		board[p.y][p.x] = new Tile();
		moveCursor(p.y, p.x);
		board[p.y][p.x]->draw();
		size_t tileX = p.x;
		size_t tileY = p.y;
		TileType type = TileType::PLAY;
		if ((ch == 'W' || ch == 'w' || ch == 72)) {
			type = board[p.y - 1][p.x]->type;
			tileY = p.y - 1;
		}
		if ((ch == 'A' || ch == 'a' || ch == 75)) {
			type = board[p.y][p.x - 1]->type;
			tileX = p.x - 1;
		}
		if ((ch == 'S' || ch == 's' || ch == 80)) {
			type = board[p.y + 1][p.x]->type;
			tileY = p.y + 1;
		}
		if ((ch == 'D' || ch == 'd' || ch == 77)) {
			type = board[p.y][p.x + 1]->type;
			tileX = p.x + 1;
		}

		Item* item = board[tileY][tileX]->item;

		// Interactions
		switch (type) {
		case TileType::ITEM:
			board[tileY][tileX]->interacted(&p);
			moveCursor(height, 0);
			clearLine();
			if (item->count > 1)
				write("Picked up % %", item->count, color(item->name, item->color));
			else
				write("Picked up %", color(item->name, item->color));
			writeStats();
		case TileType::EMPTY:
			p.x = tileX;
			p.y = tileY;
			break;
		case TileType::ENEM:
			std::array<int, 4> result = board[tileY][tileX]->interacted(&p);
			moveCursor(height, 0);
			clearLine();
			write("Dealt ");
			write(color("% damage", RED).c_str(), result[0]);
			if (result[2] > 0) {
				write(" in % hits, recieved ", result[1]);
				write(color("% damage", RED).c_str(), result[2]);
				write(" in % hits.", result[3]);
			}
			else {
				write(" in % hits.", result[1]);
			}
			writeStats();
			if (board[tileY][tileX]->enemy->health <= 0) {
				board[tileY][tileX] = new Tile();
				p.x = tileX;
				p.y = tileY;
			}
		}

		board[p.y][p.x] = new Tile(TileType::PLAY);
		moveCursor(p.y, p.x);
		writeColor("\1", YELLOW);
	}

	void boardInit() {
		board[p.y][p.x] = new Tile(TileType::PLAY);
		board[2][5] = new Tile(new GoldPile());
		board[6][22] = new Tile(new WoodenSword());
		board[4][28] = new Tile(new Enemy());
	}

	void drawBoard() {
		system("cls");
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++)
				board[i][j]->draw();
			std::cout << "\n";
		}
		writeStats();
	}

	void writeStats() {
		moveCursor(height + 1, 0);
		write(color("\x1b[2KLevel: %\n", GREEN).c_str(), p.level);
		write(color("\x1b[2KExperience: %/%\n", GREEN).c_str(), p.exp, p.expForNext);
		write(color("\x1b[2KHealth: %/%\n", RED).c_str(), p.health, p.maxHealth);
		write(color("\x1b[2KGold: %\n", YELLOW).c_str(), p.gold);
		write(color("\x1b[2KDamage: %-%\n", YELLOW).c_str(), p.minDamage, p.maxDamage);
	}

	void clearLine() {
		std::cout << "\x1b[2K";
	}

private:
	size_t width;
	size_t height;
	Player& p;

	void moveCursor(size_t row, size_t col) {
		printf("\033[%d;%dH\033[?25l", (int)row + 1, (int)col + 1);
	}
};



#endif // !BOARD
