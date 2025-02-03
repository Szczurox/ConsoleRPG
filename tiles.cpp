#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

#include"tiles.hpp"
#include"items.hpp"
#include"utils.hpp"
#include"npc.hpp"
#include"player.hpp"
#include"enemy.hpp"
#include"effective.hpp"

void Tile::draw(Player* p) {
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

InteractionResult::InteractionResult(std::shared_ptr<SoldInfo> info) : soldInfo(info), result(info->res) {};

InteractionResult Tile::interacted(Player* p, int par) {
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


void Tile::save(std::ostream& os) {
	os << getType() << " " << roomNum << " " << isVisible << "\n";
	if (item != nullptr) {
		os << "Item ";
		item->save(os);
	}
	else if (enemy != nullptr) {
		os << "Enemy ";
		enemy->save(os);
	}
	else if (npc != nullptr) {
		os << "NPC ";
		npc->save(os);
	}
	os << "NextTile\n";
}

std::shared_ptr<Enemy> Tile::load(std::ifstream& in, ItemFactory& iFactory, EnemyFactory& eFactory, NPCFactory& nFactory) {
	int tempType;
	in >> tempType >> roomNum >> isVisible;
	type = static_cast<TileType>(tempType);
	std::string line = "";
	std::shared_ptr<Enemy> e = nullptr;

	while (std::getline(in, line) && line != "NextTile") {
		std::istringstream iss(line);
		std::string type;
		std::string objectType;

		iss >> type >> objectType;

		if (type == "Item") {
			item = iFactory.createItem(objectType);
			item->load(iss);
		}
		if (type == "NPC") {
			npc = nFactory.createNPC(objectType);
			npc->load(in, iFactory);
		}
		if (type == "Enemy") {
			enemy = eFactory.createEnemy(objectType);
			enemy->load(iss);
			e = enemy;
		}
	}

	return enemy;
}

void Room::genDoor(int d, bool isSecret) {
	if (d < 2)
		doors[d] = x + rand() % (width - 2) + 1;
	if (d > 1)
		doors[d] = y + rand() % (height - 2) + 1;
	if (isSecret)
		neighboursType[d] = RoomType::SECRET;
}

// T - Enemy / Item, Args - Enemy / Item args
// Tile / limit / probability / omega / args
template<class T, typename ... Args>
void Room::randEntity(std::vector<Tile>& vec, int maxAmount, int prob, int omega, Args ... args) {
	for (int i = 0; i < maxAmount; i++)
		if (chance(prob, omega)) vec.push_back(Tile(std::shared_ptr<T>(new T(args ...)), num));
}

void Room::create(std::vector<std::vector<Tile>>& board, std::vector<std::shared_ptr<Enemy>>& enemies, int floorNum) {
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
		board[doors[0]][y] = Tile(true, num, neighbours[0], false, (neighboursType[0] == RoomType::SECRET || type == RoomType::SECRET));
	if (doors[1] != -1)
		board[doors[1]][yH - 1] = Tile(true, num, neighbours[1], false, (neighboursType[1] == RoomType::SECRET || type == RoomType::SECRET));
	if (doors[2] != -1)
		board[x][doors[2]] = Tile(true, num, neighbours[2], false, (neighboursType[2] == RoomType::SECRET || type == RoomType::SECRET));
	if (doors[3] != -1)
		board[xW - 1][doors[3]] = Tile(true, num, neighbours[3], false, (neighboursType[3] == RoomType::SECRET || type == RoomType::SECRET));

	summonSetEntities(board);

	std::vector<Tile> entities = summonEntities();

	// Append each summoned entity to a random location
	for (Tile e : entities) {
		int dx[4] = { 0, 0, -1, 1 };
		int dy[4] = { -1, 1, 0, 0 };
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


std::vector<Tile> BasicRoom::summonEntities() {
	std::vector<Tile> e;
	// Items
	randEntity<GoldPile>(e, std::min(width * height / 10, 3), 1, 3, 1, 100);
	randEntity<HealthPotion>(e, 3, 1, 10);
	randEntity<WoodenSword>(e, 1, 1, 20 * floor, randMinMax(1, 100));
	randEntity<Gambeson>(e, 1, 1, 20 * floor, randMinMax(1, 100));

	// Enemies
	if (floor < 2) {
		randEntity<Skeleton>(e, 2, 1, 3, 0, 0, num);
		randEntity<Snake>(e, 2, 1, 6, 0, 0, num);
	}
	else if (floor < 4) {
		randEntity<Skeleton>(e, 3, 1, 3, 0, 0, num);
		randEntity<Zombie>(e, 2, 1, 3, 0, 0, num);
		randEntity<Assassin>(e, 2, 1, 4, 0, 0, num);
		randEntity<Snake>(e, 2, 1, 6, 0, 0, num);
	}
	else {
		randEntity<Skeleton>(e, 2, 1, 3, 0, 0, num);
		randEntity<Zombie>(e, 2, 1, 2, 0, 0, num);
		randEntity<Assassin>(e, 2, 1, 3, 0, 0, num);
	}
	return e;
}

void StairRoom::summonSetEntities(std::vector<std::vector<Tile>>& board) {
	int xW = x + width;
	int yH = y + height;
	board[randMinMax(x + 2, xW - 2)][randMinMax(y + 2, yH - 2)] = Tile(TileType::STAIRS, num);
}

std::vector<Tile> StairRoom::summonEntities() {
	std::vector<Tile> e;
	// Items
	randEntity<GoldPile>(e, width * height / 10, 1, 3, 100, 250);
	randEntity<HealthPotion>(e, 3, 1, 10);
	// Enemies
	randEntity<Skeleton>(e, 2, 1, 3, 0, 0, num);
	return e;
}


std::vector<Tile> SecretRoom::summonEntities() {
	std::vector<Tile> e;
	int roomType = randMinMax(0, 2);
	if (roomType == 0) {
		randEntity<BloodOath>(e, 1, 1, 1);
		randEntity<IronShortsword>(e, 1, 1, 6, randMinMax(6, 66));
	}
	if (roomType == 1) {
		randEntity<SacramentalBread>(e, 1, 1, 1);
		randEntity<HealthPotion>(e, 1, 1, 3);
	}
	else {
		randEntity<HealthPotion>(e, 5, 1, 5);
		randEntity<IronShortsword>(e, 1, 1, 10, randMinMax(2, 200));
		randEntity<Gambeson>(e, 1, 1, 5 * floor, randMinMax(1, 100));
	}
	return e;
}


std::vector<Tile> Tresury::summonEntities() {
	std::vector<Tile> e;
	// Items
	randEntity<GoldPile>(e, 3, 1, 1, 1 * floor, 100 * floor);
	randEntity<GoldPile>(e, 5, 1, 4, 1 * floor, 100 * floor);
	randEntity<HealthPotion>(e, 5, 1, 5);
	randEntity<IronShortsword>(e, 1, 1, 10, randMinMax(20, 200));
	randEntity<Gambeson>(e, 1, 1, 5 * floor, randMinMax(1, 100));
	return e;
}