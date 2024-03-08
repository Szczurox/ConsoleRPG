#ifndef PLAYER
#define PLAYER

#include<vector>
#include<cstdlib>
#include<iostream>
#include<conio.h>
#include<map>
#include<random>

class Player;

enum class ItemType {
	RESOURCE = 0,
	WEAPON = 1,
	ARMOR = 2,
	USABLE = 3
};

class Item {
public:
	const char* name = "";
	const char* symbol = "@";
	ItemType type = ItemType::RESOURCE;
	int count = 0;
	unsigned char color = YELLOW;
	Item() {}
	Item(const char* name, ItemType type, const char* symbol = "@", unsigned char color = YELLOW) : name(name), type(type) {}
	virtual int picked(Player& player) { return 0; }
};

class Player {
public:
	std::map<std::string, Item> inv;
	int health = 100;
	int maxHealth = 100;
	int damage = 5;
	int defence = 0;
	int gold = 0;
	int exp = 0;
	int expForNext = 100;
	int level = 0;
	size_t x = 1;
	size_t  y = 1;

	void addItem(Item item) {
		if (inv.find(item.name) == inv.end())
			inv.insert({ item.name, item });
		else
			inv[item.name] = item;
	};

	int removeItem(std::string name, int count) {
		if (inv.find(name) == inv.end()) {
			if (inv[name].count > 0)
				inv[name].count -= count;
			else
				inv.erase(name);
		}
		else
			return -1;
	};

	int hit(int dmg) {
		health -= std::max<int>(dmg - defence, dmg / 4);
		return health;
	}
	
};



#endif // !PLAYER
