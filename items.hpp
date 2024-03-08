#ifndef ITEMS
#define ITEMS

#include<vector>
#include<cstdlib>
#include<iostream>
#include<conio.h>

class GoldPile : public Item {
public:
	GoldPile() {
		type = ItemType::RESOURCE;
		name = "gold";
	}
	int picked(Player& player) {
		srand((unsigned int)time(NULL));
		count = rand() % 250 + 1;
		player.gold += count;
		return 1;
	}
};

class WoodenSword : public Item {
public:
	WoodenSword() {
		type = ItemType::WEAPON;
		name = "Wooden Sword";
		count = 1;
	}
	int picked(Player& player) {
		return 1;
	}
};


#endif // !ITEMS