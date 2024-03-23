#ifndef TILES
#define TILES

// Tiles

enum class TileType {
	EMPTY = 0,
	WALL = 1,
	PLAY = 2,
	ITEM = 3,
	ENEM = 4,
	VOID = 5,
	DOOR = 6,
	PATH = 7,
	STAIRS = 8,
};

struct InteractionResult {
	std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>> enemy = std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>>();
	int result = 0;
	InteractionResult(std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>> enemy) : enemy(enemy) {};
	InteractionResult(int result) : result(result) {};
};

class Tile {
public:
	TileType type = TileType::EMPTY;
	std::shared_ptr<Item> item = nullptr;
	std::shared_ptr<Enemy> enemy = nullptr;
	int roomNum = 0;
	bool isVisible = true;

	Tile(int roomNum) : roomNum(roomNum) {}
	Tile(TileType type, int roomNum, bool isVisible = true) : type(type), isVisible(isVisible), roomNum(roomNum) {}
	Tile(std::shared_ptr<Item> item, int roomNum, bool isVisible = true) : type(TileType::ITEM), item(item), isVisible(isVisible), roomNum(roomNum) {}
	Tile(std::shared_ptr<Enemy> enemy, int roomNum, bool isVisible = true) : type(TileType::ENEM), enemy(enemy), isVisible(isVisible), roomNum(roomNum) {}
	Tile(bool isDoor, int roomNum, int room, bool isVisible = true) : type(TileType::DOOR), roomNum(roomNum), isVisible(isVisible) {}

	void draw(Player* p) {
		if (roomNum == p->curRoomNum && type != TileType::PATH) {
			isVisible = true;
		}
		if (isVisible) {
			switch (type)
			{
			case TileType::EMPTY:
				if (p->curRoomNum == roomNum) {
					std::cout << ".";
					break;
				}
				std::cout << " ";
				break;
			case TileType::VOID:
				std::cout << " ";
				break;
			case TileType::ITEM:
				if (p->curRoomNum == roomNum) {
					writeColor(item->symbol, item->color);
					break;
				}
				std::cout << " ";
				break;
			case TileType::ENEM:
				if (p->curRoomNum == roomNum) {
					writeColor(enemy->symbol, enemy->color);
					break;
				}
				std::cout << " ";
				break;
			case TileType::STAIRS:
				if (p->curRoomNum == roomNum) {
					writeColor("#", YELLOW);
					break;
				}
				std::cout << " ";
				break;
			case TileType::WALL:
				writeColor("#", WHITE);
				break;
			case TileType::PATH:
				writeColor(".", GREY);
				break;
			case TileType::PLAY:
				writeColor("\1", BRIGHT_YELLOW);
				break;
			case TileType::DOOR:
				writeColor("#", GREY);
				break;
			default:
				break;
			}
		}
		else std::cout << " ";
	}

	InteractionResult interacted(Player* p, int par = 0) {
		if (type == TileType::DOOR) {
			if (par == 0)
				p->curRoomNum = roomNum;
			else
				p->curRoomNum = -1;
		}
		if (type == TileType::ITEM) {
			int res = item->picked(p);
			if (p->curInvTaken < p->maxInvSpace && res == 1)
				p->addItem(item);
			else if (res == 1)
				return InteractionResult(1);
		}
		// Player dmg, Player hits, Enemy dmg, Enemy hits, xp
		if (type == TileType::ENEM)
			return InteractionResult(enemy->attacked(p));
		return InteractionResult(0);
	}
};


// Rooms

class Room {
public:
	int doors[4] = { -1, -1, -1, -1 };
	int neighbours[4] = { -1, -1, -1, -1 };
	// Top left corner of the room coordinates
	int x = 0;
	int y = 0;
	int width = 10;
	int height = 5;
	int bufferX = 0;
	// Room number
	int num = 0;
	Room() {}
	Room(int num, int x, int y, int width, int height, int bufferX) : num(num), x(x), y(y), width(width), height(height), bufferX(bufferX) {}
	void genDoor(int d) {
		if (d < 2)
			doors[d] = x + rand() % (width - 2) + 1;
		if (d > 1)
			doors[d] = y + rand() % (height - 2) + 1;
	}

	virtual Tile summonEntities() {
		return Tile(num);
	}

	virtual void create(std::vector<std::vector<Tile>>& board, std::vector<std::shared_ptr<Enemy>>& enemies) {
		int xW = x + width;
		int yH = y + height;
		int randomX, randomY;
		// Coordinates of staircase
		if (num == 100) {
			randomX = randMinMax(x + 2, xW - 2);
			randomY = randMinMax(y + 2, yH - 2);
		}
		for (int i = y; i < yH; i++) {
			board[x][i] = Tile(TileType::WALL, num, false);
			board[(int)xW - 1][i] = Tile(TileType::WALL, num, false);
		}
		for (int i = x; i < xW; i++) {
			board[i][y] = Tile(TileType::WALL, num, false);
			board[i][(int)yH - 1] = Tile(TileType::WALL, num, false);
		}
		if (doors[0] != -1)
			board[doors[0]][y] = Tile(true, num, neighbours[0], false);
		if (doors[1] != -1)
			board[doors[1]][(int)yH - 1] = Tile(true, num, neighbours[1], false);
		if (doors[2] != -1)
			board[x][doors[2]] = Tile(true, num, neighbours[2], false);
		if (doors[3] != -1)
			board[(int)xW - 1][doors[3]] = Tile(true, num, neighbours[3], false);
		for (int i = x + 1; i < xW - 1; i++)
			for (int j = y + 1; j < yH - 1; j++) {
				Tile entity = Tile(num);
				if (num == 100 && i == randomX && j == randomY)
					entity = Tile(TileType::STAIRS, num);
				else
					entity = summonEntities();
				if (entity.type == TileType::ENEM) {
					entity.enemy->x = i;
					entity.enemy->y = j;
					enemies.push_back(entity.enemy);
				}
				if (board[i][j].type == TileType::VOID)
					board[i][j] = entity;
			}
	}
};

class EntranceRoom : public Room {
public:
	EntranceRoom(int numb, int xC, int yC, int w, int h, int buffX) {
		num = numb;
		x = xC;
		y = yC;
		width = w;
		height = h;
		bufferX = buffX;
	}
};

class BasicRoom : public Room {
public:
	BasicRoom(int numb, int xC, int yC, int w, int h, int buffX) {
		num = numb;
		x = xC;
		y = yC;
		width = w;
		height = h;
		bufferX = buffX;
	}

	virtual Tile summonEntities() {
		int rng = rand() % 1000;
		if (rng < 20) return Tile(std::shared_ptr<Item>(new GoldPile(100, 250)), num);
		else if (rng < 30) return Tile(std::shared_ptr<Item>(new HealthPotion()), num);
		else if (rng < 50) return Tile(std::shared_ptr<Enemy>(new Skeleton(0, 0, num)), num);
		else if (rng < 51) return Tile(std::shared_ptr<Item>(new WoodenSword(rand() % 100 + 10)), num);
		else if (rng < 52) return Tile(std::shared_ptr<Item>(new Gambeson(rand() % 100 + 10)), num);
		else return Tile(num);
	}
};

class Tresury : public Room {
public:
	Tresury(int numb, int xC, int yC, int w, int h, int buffX) {
		num = numb;
		x = xC;
		y = yC;
		width = w;
		height = h;
		bufferX = buffX;
	}

	virtual Tile summonEntities() {
		int rng = rand() % 1000;
		if (rng < 100) return Tile(std::shared_ptr<Item>(new GoldPile(100, 250)), num);
		else if (rng < 120) return Tile(std::shared_ptr<Item>(new HealthPotion()), num);
		else if (rng < 121) return Tile(std::shared_ptr<Item>(new IronShortSword(rand() % 200 + 10)), num);
		else return Tile(num);
	}
};

#endif // !TILES