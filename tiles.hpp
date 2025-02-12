#ifndef TILES
#define TILES

#include<vector>
#include<array>
#include<utility>
#include<tuple>
#include<memory>
#include<functional>

class NPC;
class Player;
class Enemy;
class Item;
class ItemFactory;
class EnemyFactory;
class NPCFactory;
class SoldInfo;

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
	std::tuple<std::array<int, 5>, std::pair<std::wstring, unsigned char>> enemy = { {0, 0, 0, 0, 0}, {L"", 0} };
	std::function<void()> soldInfo = nullptr;
	int result = 0;

	InteractionResult(std::function<void()> info) : soldInfo(info), result(1) {};
	InteractionResult(std::tuple<std::array<int, 5>, std::pair<std::wstring, unsigned char>> enemy) : enemy(enemy) {};
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

	void draw(Player* p);

	InteractionResult interacted(Player* p, int par = 0);

	// Class name of the tile
	virtual int getType() const {
		return static_cast<int>(type);
	}

	void save(std::ostream& os);

	std::shared_ptr<Enemy> load(std::ifstream& in, ItemFactory& iFactory, EnemyFactory& eFactory, NPCFactory& nFactory);
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
	
	void genDoor(int d, bool isSecret = false);

	virtual std::vector<Tile> summonEntities() {
		return std::vector<Tile>();
	}

	virtual void summonSetEntities(std::vector<std::vector<Tile>>& board) {}

	// T - Enemy / Item, Args - Enemy / Item args
	// Tile / limit / probability / omega / args
	template<class T, typename ... Args>
	void randEntity(std::vector<Tile>& vec, int maxAmount, int prob, int omega, Args ... args);

	virtual void create(std::vector<std::vector<Tile>>& board, std::vector<std::shared_ptr<Enemy>>& enemies, int floorNum);
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

	virtual std::vector<Tile> summonEntities();
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

	virtual void summonSetEntities(std::vector<std::vector<Tile>>& board);

	virtual std::vector<Tile> summonEntities();
};

class SecretRoom : public Room {
public:
	SecretRoom() {
		type = RoomType::SECRET;
	}

	virtual std::vector<Tile> summonEntities();
};


class Tresury : public Room {
public:
	Tresury() {
		type = RoomType::TREASURE;
	}

	virtual std::vector<Tile> summonEntities();
};

#endif // !TILES