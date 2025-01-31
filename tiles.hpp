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
	NPC = 9,
	SECRET_DOOR = 10,
};

struct InteractionResult {
	std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>> enemy = std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>>();
	SoldInfo soldInfo = SoldInfo();
	int result = 0;
	InteractionResult(SoldInfo info) : soldInfo(info), result(info.res) {};
	InteractionResult(std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>> enemy) : enemy(enemy) {};
	InteractionResult(int result) : result(result) {};
};

class Tile {
public:
	TileType type = TileType::EMPTY;
	std::shared_ptr<Item> item = nullptr;
	std::shared_ptr<Enemy> enemy = nullptr;
	std::shared_ptr<NPC> npc = nullptr;
	int roomNum = 0;
	bool isVisible = true;

	Tile(int roomNum) : roomNum(roomNum) {}
	Tile(TileType type, int roomNum, bool isVisible = true) : type(type), isVisible(isVisible), roomNum(roomNum) {}
	Tile(std::shared_ptr<Item> item, int roomNum, bool isVisible = true) : type(TileType::ITEM), item(item), isVisible(isVisible), roomNum(roomNum) {}
	Tile(std::shared_ptr<Enemy> enemy, int roomNum, bool isVisible = true) : type(TileType::ENEM), enemy(enemy), isVisible(isVisible), roomNum(roomNum) {}
	Tile(std::shared_ptr<NPC> npc, int roomNum, bool isVisible = true) : type(TileType::NPC), npc(npc), isVisible(isVisible), roomNum(roomNum) {}
	Tile(bool isDoor, int roomNum, int room, bool isVisible = true, bool isSecret = false) : type(!isSecret ? TileType::DOOR : TileType::SECRET_DOOR), roomNum(roomNum), isVisible(isVisible) {}

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
					writeColor(item->symbol.c_str(), item->colord);
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
			case TileType::NPC:
				if (p->curRoomNum == roomNum) {
					writeColor(npc->symbol, npc->colord);
					break;
				}
				std::wcout << L" ";
				break;
			case TileType::SECRET_DOOR:
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
		if (type == TileType::SECRET_DOOR)
			type = TileType::DOOR;
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
		if (type == TileType::NPC) {
			return InteractionResult(npc->interacted(p));
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
	SECRET = 4,
};

class Room {
public:
	RoomType type = RoomType::BASIC;
	RoomType neighboursType[4] = { RoomType::BASIC, RoomType::BASIC, RoomType::BASIC, RoomType::BASIC };
	int neighbours[4] = { -1, -1, -1, -1 };
	int doors[4] = { -1, -1, -1, -1 };
	// Top left corner of the room coordinates
	int x = 0;
	int y = 0;
	int width = 10;
	int height = 5;
	int bufferX = 0;
	int floor = 1;
	// Room number
	int num = 0;
	Room() {}
	Room(int num, int x, int y, int width, int height, int bufferX) : num(num), x(x), y(y), width(width), height(height), bufferX(bufferX) {}
	
	void init(int numb, int xR, int yR, int w, int h, int buffX) {
		num = numb;
		x = xR;
		y = yR;
		width = w;
		height = h;
		bufferX = buffX;
	}
	
	void genDoor(int d, bool isSecret = false) {
		if (d < 2)
			doors[d] = x + rand() % (width - 2) + 1;
		if (d > 1)
			doors[d] = y + rand() % (height - 2) + 1;
		if (isSecret)
			neighboursType[d] = RoomType::SECRET;
	}

	virtual std::vector<Tile> summonEntities() {
		return std::vector<Tile>();
	}

	virtual void summonSetEntities(std::vector<std::vector<Tile>>& board) {}

	// T - Enemy / Item, Args - Enemy / Item args
	// Tile / limit / probability / omega / args
	template<class T, typename ... Args>
	void randEntity(std::vector<Tile>& vec, int maxAmount, int prob, int omega, Args ... args) {
		for (int i = 0; i < maxAmount; i++)
			if (chance(prob, omega)) vec.push_back(Tile(std::shared_ptr<T>(new T(args ...)), num));
	}

	virtual void create(std::vector<std::vector<Tile>>& board, std::vector<std::shared_ptr<Enemy>>& enemies, int floorNum) {
		floor = floorNum + 1;
		int xW = x + width;
		int yH = y + height;
		for (int i = y; i < yH; i++) {
			board[x][i] = Tile(TileType::WALL, num, false);
			board[xW - 1][i] = Tile(TileType::WALL, num, false);
		}
		for (int i = x; i < xW; i++) {
			board[i][y] = Tile(TileType::WALL, num, false);
			board[i][yH - 1] = Tile(TileType::WALL, num, false);
		}
		if (doors[0] != -1)
			board[doors[0]][y] = Tile(true, num, neighbours[0], false, neighboursType[0] == RoomType::SECRET);
		if (doors[1] != -1)
			board[doors[1]][yH - 1] = Tile(true, num, neighbours[1], false, neighboursType[1] == RoomType::SECRET);
		if (doors[2] != -1)
			board[x][doors[2]] = Tile(true, num, neighbours[2], false, neighboursType[2] == RoomType::SECRET);
		if (doors[3] != -1)
			board[xW - 1][doors[3]] = Tile(true, num, neighbours[3], false, neighboursType[3] == RoomType::SECRET);

		summonSetEntities(board);

		std::vector<Tile> entities = summonEntities();

		// Append each summoned entity to a random location
		for (Tile e : entities) {
			bool valid = false;
			while (!valid) {
				int randX = randMinMax(x + 1, xW - 1);
				int randY = randMinMax(y + 1, yH - 1);
				int isNearDoor = false;
				for (int i = 0; i < 4; i++) {
					int dX = randX + dx[i];
					int dY = randY + dy[i];
					if (board[dX][dY].type == TileType::DOOR || board[dX][dY].type == TileType::SECRET_DOOR)
						isNearDoor = true;
				}
				if (!isNearDoor && board[randX][randY].type == TileType::NOTHING) {
					board[randX][randY] = e;
					if (e.type == TileType::ENEM) {
						e.enemy->x = randX;
						e.enemy->y = randY;
						enemies.push_back(e.enemy);
					}
					valid = true;
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
	EntranceRoom() {
		type = RoomType::ENTRANCE;
	}
};

class BasicRoom : public Room {
public:
	BasicRoom() {
		type = RoomType::BASIC;
	}

	virtual std::vector<Tile> summonEntities() {
		std::vector<Tile> e;
		// Items
		randEntity<GoldPile>(e, min(width * height / 10, 3), 1, 3, 1, 100);
		randEntity<HealthPotion>(e, 3, 1, 10);
		randEntity<WoodenSword>(e, 1, 1, 20 * floor, randMinMax(1, 100));
		randEntity<Gambeson>(e, 1, 1, 20 * floor, randMinMax(1, 100));

		// Enemies
		if (floor < 2)
			randEntity<Skeleton>(e, 2, 1, 3, 0, 0, num);
		else if (floor < 4) {
			randEntity<Skeleton>(e, 3, 1, 3, 0, 0, num);
			randEntity<Zombie>(e, 2, 1, 3, 0, 0, num);
			randEntity<Assassin>(e, 2, 1, 4, 0, 0, num);
		}
		else {
			randEntity<Skeleton>(e, 2, 1, 3, 0, 0, num);
			randEntity<Zombie>(e, 2, 1, 2, 0, 0, num);
			randEntity<Assassin>(e, 2, 1, 3, 0, 0, num);
		}
		return e;
	}
};

class StairRoom : public Room {
public:
	StairRoom() {
		type = RoomType::STAIR;
	}

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

	virtual std::vector<Tile> summonEntities() {
		std::vector<Tile> e;
		// Items
		randEntity<GoldPile>(e, width * height / 10, 1, 3, 100, 250);
		randEntity<HealthPotion>(e, 3, 1, 10);
		// Enemies
		randEntity<Skeleton>(e, 2, 1, 3, 0, 0, num);
		return e;
	}
};

class SecretRoom : public Room {
public:
	SecretRoom() {
		type = RoomType::SECRET;
	}

	virtual std::vector<Tile> summonEntities() {
		std::vector<Tile> e;
		int roomType = randMinMax(0, 2);
		if (roomType == 0) {
			randEntity<BloodOath>(e, 1, 1, 1);
			randEntity<IronShortSword>(e, 1, 1, 6, randMinMax(6, 66));
		}
		if (roomType == 1) {
			randEntity<SacramentalBread>(e, 1, 1, 1);
			randEntity<HealthPotion>(e, 1, 1, 3);
		}
		else {
			randEntity<HealthPotion>(e, 5, 1, 5);
			randEntity<IronShortSword>(e, 1, 1, 10, randMinMax(2, 200));
			randEntity<Gambeson>(e, 1, 1, 5 * floor, randMinMax(1, 100));
		}
		return e;
	}
};


class Tresury : public Room {
public:
	Tresury() {
		type = RoomType::TREASURE;
	}

	virtual std::vector<Tile> summonEntities() {
		std::vector<Tile> e;
		// Items
		randEntity<GoldPile>(e, 3, 1, 1, 1 * floor, 100 * floor);
		randEntity<GoldPile>(e, 5, 1, 4, 1 * floor, 100 * floor);
		randEntity<HealthPotion>(e, 5, 1, 5);
		randEntity<IronShortSword>(e, 1, 1, 10, randMinMax(20, 200));
		randEntity<Gambeson>(e, 1, 1, 5 * floor, randMinMax(1, 100));
		return e;
	}
};

#endif // !TILES