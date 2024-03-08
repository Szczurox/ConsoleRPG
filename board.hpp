#ifndef BOARD
#define BOARD

#include<vector>
#include<cstdlib>
#include<iostream>
#include<conio.h>
#include<map>
#include<minmax.h>

void writeColor(const char* text, unsigned char color = WHITE) {
	printf("\033[97;%dm%s\033[m\033[?25l", color, text);
}

enum class TileType {
	EMPTY = 0,
	WALL = 1,
	PLAY = 2,
	ITEM = 3,
	ENEMY = 4
};

class Tile {
public:
	TileType type = TileType::EMPTY;
	Item* item = nullptr;

	Tile() {}
	Tile(TileType type) : type(type) {}
	Tile(Item* item) : type(TileType::ITEM), item(item) {}

	void draw() {
		if (type == TileType::EMPTY)
			std::cout << ".";
		if (type == TileType::WALL)
			writeColor("#", WHITE);
		if (type == TileType::PLAY)
			writeColor("\1", YELLOW);
		if (type == TileType::ITEM)
			writeColor(item->symbol, YELLOW);
	}

	int interacted(Player& p) {
		if (type == TileType::ITEM) {
			return item->picked(p);
		}
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
		board[p.y][p.x] = new Tile(TileType::PLAY);
		board[2][5] = new Tile(new GoldPile());
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
			type = board[p.x][p.x - 1]->type;
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

		switch (type) {
		case TileType::ITEM:
			board[tileY][tileX]->interacted(p);
			rewriteStats();
		case TileType::EMPTY:
			p.x = tileX;
			p.y = tileY;
			break;
		}

		board[p.y][p.x] = new Tile(TileType::PLAY);
		moveCursor(p.y, p.x);
		writeColor("\1", YELLOW);
	}

	void drawBoard() {
		system("cls");
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++)
				board[i][j]->draw();
			std::cout << "\n";
		}
		rewriteStats();
	}

	void rewriteStats() {
		moveCursor(height, 0);
		char s[256];
		sprintf_s(s, "Level: %d\r\n", p.level);
		writeColor(s, GREEN);
		sprintf_s(s, "Experience: %d/%d\r\n", p.exp, p.expForNext);
		writeColor(s, GREEN);
		sprintf_s(s, "Health: %d/%d\r\n", p.health, p.maxHealth);
		writeColor(s, RED);
		sprintf_s(s, "Gold: %d\r\n\033[?25l", p.gold);
		writeColor(s, YELLOW);
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
