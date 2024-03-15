#ifndef ITEMS
#define ITEMS

class Weapon : public Item {
public:
	Weapon() {
		type = ItemType::WEAPON;
		count = 1;
	}
	int picked(Player* player) {
		return 1;
	}
	virtual int used(Player* player) {
		if (player->weapon == nullptr || player->weapon.get()->name != name) {
			player->minDamage = player->baseDamage + minDmg;
			player->maxDamage = player->baseDamage + maxDmg;
			return 1;
		}
		else {
			player->minDamage = player->baseDamage;
			player->maxDamage = player->baseDamage; 
			return 3;
		}
		return 5;
	}
	virtual int itemMenu(Player* p) {
		MenuItem nameI(name, BRIGHT_CYAN);
		MenuItem loreI(lore, WHITE);
		char s[256];
		sprintf_s(s, "This weapon deals %d-%d damage", minDmg, maxDmg);
		MenuItem damage(s, WHITE);
		std::vector<MenuItem*> texts({ &nameI, &loreI });

		MenuItem equip;
		if (p->weapon == nullptr || p->weapon.get()->name != name)
			equip = MenuItem("Equip", GREEN);
		else 
			equip = MenuItem("Unequip", GREEN);
		MenuItem remove("Remove", RED);
		MenuItem back("Back", WHITE);
		std::vector<MenuItem*> options({ &equip, &remove, &back });

		Menu menu(&options, &texts);
		int choice = -2;
		while (choice == -2) {
			choice = menu.open();
			if (choice == 0)
				return used(p);
			if (choice == 1) {
				MenuItem option("Are you sure you want to remove this item from your inventory?", RED);
				MenuItem no("No", WHITE);
				MenuItem yes("Yes", WHITE);
				std::vector<MenuItem*> options({ &no, &yes });
				Menu deleteMenu(&options, &option);
				int confirmation = deleteMenu.open();
				if (confirmation == 0)
					choice = -2;
				else {
					p->removeItem(name, 1);
					if (p->weapon.get() == this)
						p->weapon = nullptr;
					return -1;
				}
			}
		}
		return 0;
	}
};

class Armor : public Item {
public:
	Armor() {
		type = ItemType::ARMOR;
		count = 1;
	}
	int picked(Player* player) {
		return 1;
	}
	virtual int used(Player* player) {
		if (player->weapon == nullptr || player->weapon.get()->name != name) {
			player->defence = prot;
			return 2;
		}
		else {
			player->defence = 0;
			return 4;
		}
		return 5;
	}
	virtual int itemMenu(Player* p) {
		MenuItem nameI(name, BRIGHT_CYAN);
		MenuItem loreI(lore, WHITE);
		char s[256];
		sprintf_s(s, "This armor gives you %d defence", prot);
		MenuItem damage(s, WHITE);
		std::vector<MenuItem*> texts({ &nameI, &loreI });

		MenuItem equip("Equip", GREEN);
		MenuItem remove("Remove", RED);
		MenuItem back("Back", WHITE);
		std::vector<MenuItem*> options({ &equip, &remove, &back });

		Menu menu(&options, &texts);
		int choice = -2;
		while (choice == -2) {
			choice = menu.open();
			if (choice == 0)
				return used(p);
			if (choice == 1) {
				MenuItem option("Are you sure you want to remove this item from your inventory?", RED);
				MenuItem no("No", WHITE);
				MenuItem yes("Yes", WHITE);
				std::vector<MenuItem*> options({ &no, &yes });
				Menu deleteMenu(&options, &option);
				int confirmation = deleteMenu.open();
				if (confirmation == 0)
					choice = -2;
				else {
					p->removeItem(name, 1);
					if (p->armor.get() == this)
						p->armor = nullptr;
					return -1;
				}
			}
		}
		return 0;
	}
};

class GoldPile : public Item {
public:
	GoldPile(int minGold, int maxGold) {
		type = ItemType::RESOURCE;
		name = "gold";
		count = randMinMax(minGold, maxGold);
	}

	int picked(Player* player) {
		player->gold += count;
		return 0;
	}
};

class WoodenSword : public Weapon {
public:
	WoodenSword() {
		name = "Wooden Sword";
		lore = "Basic wooden sword.";
		color = BLUE;
		symbol = "|";
		minDmg = 3;
		maxDmg = 5;
		speed = 1;
		reqLevel = 0;
	}
};

class IronShortSword : public Weapon {
public:
	IronShortSword() {
		name = "Iron Shortsword";
		lore = "Quick weapon.";
		color = BLUE;
		symbol = "|";
		minDmg = 3;
		maxDmg = 8;
		speed = 2;
		reqLevel = 2;
	}
};


#endif // !ITEMS