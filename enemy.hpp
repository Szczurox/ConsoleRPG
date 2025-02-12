#ifndef ENEMY
#define ENEMY

#include<vector>
#include<memory>
#include<string>
#include<map>
#include<functional>
#include<array>

#include"const.hpp"
#include"items.hpp"

class Player;

bool chance(int prob, int omega);

class Enemy {
public:
	const wchar_t* name = L"Enemy";
	const wchar_t* symbol = L"☺";
	unsigned char nameColor = BLUE;
	unsigned char color = RED;
	int health = 10;
	int minDamage = 4;
	int maxDamage = 6;
	int defence = 0;
	int minGold = 10;
	int maxGold = 50;
	int level = 0;
	int speed = 1;
	int xp = 10;
	int sight = 8;
	int x = 1;
	int y = 1;
	int roomNum = 0;

	Enemy() {}
	Enemy(int x, int y, int roomNum) : x(x), y(y), roomNum(roomNum) {}

	// Calculate damage enemy has taken
	int hit(int dmg);

	// Calculate enemy damage
	int attack();

	// Apply any special attack effects to player
	virtual std::pair<std::wstring, unsigned char> special(Player* p) { return { L"", 0 }; };

	// Return randomised loot
	template<class T, typename ... Args>
	void randLoot(std::vector<std::shared_ptr<Item>>& vec, int maxAmount, int prob, int omega, Args ... args) {
		if (!T(args ...).stackable) {
			for (int i = 0; i < maxAmount; i++)
				if (chance(prob, omega)) vec.push_back(std::shared_ptr<Item>(new T(args ...)));
		}
		else {
			vec.push_back(std::shared_ptr<Item>(new T(args ...)));
			for (int i = 0; i < maxAmount; i++)
				if (chance(prob, omega)) vec.back()->count++;
		}
	}

	virtual std::vector<std::shared_ptr<Item>> getLoot();
	// Handle fight
	std::tuple<std::array<int, 5>, std::pair<std::wstring, unsigned char>> attacked(Player* p, bool first = false);
	// Class name of the enemy
	virtual std::string getType() const;

	virtual void save(std::ostream& os);

	virtual void load(std::istringstream& in);
};


class EnemyFactory {
public:
	std::map<std::string, std::function<std::shared_ptr<Enemy>()>> enemyMap;

	EnemyFactory();
	std::shared_ptr<Enemy> createEnemy(const std::string& type);

	template <class T>
	void registerEnemy() {
		std::string type = typeid(T).name();
		if (type.rfind("class ", 0) == 0)
			type = type.substr(6);
		enemyMap[type] = []() -> std::shared_ptr<Enemy> {
			return std::make_shared<T>();
		};
	}
};

class Skeleton : public Enemy {
public:
	Skeleton(int xC = 0, int yC = 0, int num = 0) {
		x = xC;
		y = yC;
		roomNum = num;
		name = L"Skeleton";
		nameColor = GREY;
		color = WHITE;
		health = 10;
		minDamage = 4;
		maxDamage = 6;
		xp = 10;
		minGold = 10;
		maxGold = 100;
	}

	virtual std::vector<std::shared_ptr<Item>> getLoot();
};

class Zombie : public Enemy {
public:
	Zombie(int xC = 0, int yC = 0, int num = 0) {
		x = xC;
		y = yC;
		roomNum = num;
		name = L"Zombie";
		symbol = L"☻";
		nameColor = GREY;
		color = GREEN;
		health = 30;
		minDamage = 3;
		maxDamage = 5;
		xp = 20;
		minGold = 30;
		maxGold = 150;
	}

	virtual std::pair<std::wstring, unsigned char> special(Player* player);
	virtual std::vector<std::shared_ptr<Item>> getLoot();
};

class Assassin : public Enemy {
public:
	Assassin(int xC = 0, int yC = 0, int num = 0) {
		x = xC;
		y = yC;
		roomNum = num;
		name = L"Assassin";
		symbol = L"☻";
		nameColor = BRIGHT_BLUE;
		color = RED;
		health = 20;
		minDamage = 6;
		maxDamage = 10;
		xp = 40;
		minGold = 1;
		maxGold = 256;
	}

	virtual std::pair<std::wstring, unsigned char> special(Player* player);
	virtual std::vector<std::shared_ptr<Item>> getLoot();
};

class Snake : public Enemy {
public:
	Snake(int xC = 0, int yC = 0, int num = 0) {
		x = xC;
		y = yC;
		roomNum = num;
		name = L"Snake";
		symbol = L"S";
		nameColor = GREY;
		color = GREEN;
		health = 5;
		minDamage = 3;
		maxDamage = 5;
		xp = 20;
		speed = 2;
		minGold = 0;
		maxGold = 0;
	}

	virtual std::pair<std::wstring, unsigned char> special(Player* player);
	virtual std::vector<std::shared_ptr<Item>> getLoot();
};

#endif // !ENEMY
