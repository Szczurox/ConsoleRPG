#ifndef BOARD
#define BOARD

class Player;
class Enemy;
class Tile;
class ItemFactory;
class EnemyFactory;
class NPCFactory;
class Item;

#include"const.hpp"

class Board {
public:
	std::vector<std::vector<Tile>> board;
	std::vector<std::shared_ptr<Enemy>> enemies;
	unsigned int seed;

	Board(int width, int height, Player& player, bool loading = false, unsigned int seed = 0);

	void save(std::wstring fileName);

	void load(std::wstring fileName, ItemFactory& iFactory, EnemyFactory& eFactory, NPCFactory& nFactory);

	// Run before drawing new board for the first time
	void boardInit();

	void drawBoard();

	void drawBoardFull(std::function<void()> info = []() {});

	void makeBoxRoof(int start);

	void makeBoxPiece(int start);

	void writeBuff(BuffType type);

	void writeStats();

	void writeStats2();

	void writeStats3();

	// Update board on player action
	int movePlayer(char ch);

	// Enemies move AI
	void moveEnemies(std::shared_ptr<Enemy> fought = nullptr);

	int placeItems(std::vector<std::shared_ptr<Item>> items, int tileX, int tileY);

	void startInfo();

private:
	int width;
	int height;
	int boxSize = 44;
	bool moveDone = true;
	Player& p;

	void drawTile(int  x, int y);

	// Set tile as empty if tile not specified
	void changeTile(int x, int y);

	// Replace tile with a different tile
	void changeTile(int x, int y, Tile tile);

	// Swap spots of two tiles
	void swapTile(int x, int y, int x2, int y2, bool directSwap);

	void swapTile(int x, int y, int d, int numOfTiles = 1);

	bool isTileValid(int x, int y);

	bool isTileValid(int x, int y, bool empty);

	// 0 - Up, 1 - Down, 2 - Left, 3 - Right
	bool isMoveValid(int x, int y, int d);

	bool isMoveEnemyValid(int x, int y, int d);

	void moveEntity(int& x, int& y, int d, int numOfTiles = 1);
};



#endif // !BOARD
