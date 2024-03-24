#ifndef TILES
#define TILES

// Tiles

enum class TileType {
	EMPTY = 0,
	WALL = 1,
	PLAY = 2,
	ITEM = 3,
	ENEM = 4,
	NOTHING = 5,
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
					std::wcout << L".";
					break;
				}
				std::wcout << L" ";
				break;
			case TileType::NOTHING:
				std::wcout << L" ";
				break;
			case TileType::ITEM:
				if (p->curRoomNum == roomNum) {
					writeColor(item->symbol, item->color);
					break;
				}
				std::wcout << L" ";
				break;
			case TileType::ENEM:
				if (p->curRoomNum == roomNum) {
					writeColor(enemy->symbol, enemy->color);
					break;
				}
				std::wcout << L" ";
				break;
			case TileType::STAIRS:
				if (p->curRoomNum == roomNum) {
					writeColor(L"#", YELLOW);
					break;
				}
				std::wcout << L" ";
				break;
			case TileType::WALL:
				writeColor(L"▒", BRIGHT_WHITE);
				break;
			case TileType::PATH:
				writeColor(L".", GREY);
				break;
			case TileType::PLAY:
				writeColor(L"☻", BRIGHT_YELLOW);
				break;
			case TileType::DOOR:
				writeColor(L"╫", GREY);
				break;
			default:
				break;
			}
		}
		else std::wcout << L" ";
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

enum class RoomType {
	ENTRANCE = 0,
	BASIC = 1,
	TREASURE = 2,
	STAIR = 3,
};

class Room {
public:
	RoomType type = RoomType::BASIC;
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

	virtual std::vector<std::pair<Tile, int>> summonEntities() {
		return std::vector<std::pair<Tile, int>>();
	}

	virtual void summonSetEntities(std::vector<std::vector<Tile>>& board) {}

	virtual std::pair<Tile, int> randEntity(int maxAmount, int chance, Tile tile) {
		int numOfTiles = 0;
		for (int i = 0; i < maxAmount; i++) {
			int random = rand() % chance + 1;
			if (random == 1) numOfTiles++;
		}
		return std::make_pair(tile, numOfTiles);
	}

	virtual void create(std::vector<std::vector<Tile>>& board, std::vector<std::shared_ptr<Enemy>>& enemies) {
		int xW = x + width;
		int yH = y + height;
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

		summonSetEntities(board);

		std::vector<std::pair<Tile, int>> entities = summonEntities();

		// Append each summoned entity to a random location
		for (std::pair<Tile, int> e : entities) {
			for (int i = 0; i < e.second; i++) {
				bool valid = false;
				while (!valid) {
					int randX = randMinMax(x + 1, xW - 1);
					int randY = randMinMax(y + 1, yH - 1);
					int isNearDoor = false;
					for (int i = 0; i < 4; i++) {
						int dX = randX + dx[i];
						int dY = randY + dy[i];
						if (board[dX][dY].type == TileType::DOOR)
							isNearDoor = true;
					}
					if (!isNearDoor && board[randX][randY].type == TileType::NOTHING) {
						board[randX][randY] = e.first;
						if (e.first.type == TileType::ENEM) {
							e.first.enemy->x = randX;
							e.first.enemy->y = randY;
							enemies.push_back(e.first.enemy);
						}
						valid = true;
					}
				}
			}
		}

		for (int i = x + 1; i < xW - 1; i++)
			for (int j = y + 1; j < yH - 1; j++)
				if (board[i][j].type == TileType::NOTHING)
					board[i][j] = Tile(num);
	}
};

class EntranceRoom : public Room {
public:
	EntranceRoom(int numb, int xC, int yC, int w, int h, int buffX) {
		type = RoomType::ENTRANCE;
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
		type = RoomType::BASIC;
		num = numb;
		x = xC;
		y = yC;
		width = w;
		height = h;
		bufferX = buffX;
	}

	virtual std::vector<std::pair<Tile, int>> summonEntities() {
		std::vector<std::pair<Tile, int>> e;
		// Items
		e.push_back(randEntity(width * height / 10, 3, Tile(std::shared_ptr<Item>(new GoldPile(100, 250)), num)));
		e.push_back(randEntity(3, 10, Tile(std::shared_ptr<Item>(new HealthPotion()), num)));
		e.push_back(randEntity(1, 20, Tile(std::shared_ptr<Item>(new WoodenSword(randMinMax(1, 100))), num)));
		e.push_back(randEntity(1, 20, Tile(std::shared_ptr<Item>(new Gambeson(randMinMax(1, 100))), num)));
		// Enemies
		e.push_back(randEntity(2, 3, Tile(std::shared_ptr<Enemy>(new Skeleton(0, 0, num)), num)));
		return e;
	}
};

class StairRoom : public Room {
public:
	StairRoom(int numb, int xC, int yC, int w, int h, int buffX) {
		type = RoomType::STAIR;
		num = numb;
		x = xC;
		y = yC;
		width = w;
		height = h;
		bufferX = buffX;
	}

	virtual void summonSetEntities(std::vector<std::vector<Tile>>& board) {
		int xW = x + width;
		int yH = y + height;
		board[randMinMax(x + 2, xW - 2)][randMinMax(y + 2, yH - 2)] = Tile(TileType::STAIRS, num);
	}

	virtual std::vector<std::pair<Tile, int>> summonEntities() {
		std::vector<std::pair<Tile, int>> e;
		// Items
		e.push_back(randEntity(width * height / 10, 3, Tile(std::shared_ptr<Item>(new GoldPile(100, 250)), num)));
		e.push_back(randEntity(3, 10, Tile(std::shared_ptr<Item>(new HealthPotion()), num)));
		// Enemies
		e.push_back(randEntity(2, 3, Tile(std::shared_ptr<Enemy>(new Skeleton(0, 0, num)), num)));
		return e;
	}
};

class Tresury : public Room {
public:
	Tresury(int numb, int xC, int yC, int w, int h, int buffX) {
		type = RoomType::TREASURE;
		num = numb;
		x = xC;
		y = yC;
		width = w;
		height = h;
		bufferX = buffX;
	}

	virtual std::vector<std::pair<Tile, int>> summonEntities() {
		std::vector<std::pair<Tile, int>> e;
		// Items
		e.push_back(randEntity(3, 1, Tile(std::shared_ptr<Item>(new GoldPile(100, 250)), num)));
		e.push_back(randEntity(5, 3, Tile(std::shared_ptr<Item>(new GoldPile(100, 250)), num)));
		e.push_back(randEntity(5, 5, Tile(std::shared_ptr<Item>(new HealthPotion()), num)));
		e.push_back(randEntity(1, 10, Tile(std::shared_ptr<Item>(new IronShortSword(randMinMax(20, 200))), num)));
		e.push_back(randEntity(1, 5, Tile(std::shared_ptr<Item>(new Gambeson(randMinMax(10, 100))), num)));
		return e;
	}
};

#endif // !TILES