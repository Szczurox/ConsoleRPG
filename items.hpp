#ifndef ITEMS
#define ITEMS

class Weapon : public Item {
public:
	Weapon() {
		type = ItemType::WEAPON;
		count = 1;
		stackable = false;
	}
	virtual int used(Player* player) {
		if (player->weapon.get() != this) {
			player->minDamage = player->baseDamage + minDmg;
			player->maxDamage = player->baseDamage + maxDmg;
			return 1;
		}
		else {
			player->minDamage = player->baseDamage;
			player->maxDamage = player->baseDamage; 
			return 3;
		}
	}
	virtual void onRemove(Player* p) {
		p->removeItem(name, 1);
		if (p->weapon.get() == this)
			p->weapon = nullptr;
	}
	virtual int itemMenu(Player* p) {
		MenuItem nameI(name, color);
		MenuItem loreI(lore, WHITE);
		wchar_t s[256];
		wsprintf(s, L"Deals %d-%d damage and has %d speed", minDmg, maxDmg, speed);
		MenuItem damage(s, YELLOW);
		wchar_t d[256];
		wsprintf(d, L"Durability: %d/%d", durability, maxDurability);
		MenuItem dur(d, BRIGHT_BLUE);
		std::vector<MenuItem> texts({ nameI, loreI, damage, dur });
		if (p->level < reqLevel) {
			wchar_t e[256];
			wsprintf(e, L"Required Level: %d", reqLevel);
			texts.push_back(MenuItem(e, RED));
		}

		std::vector<MenuItem> options;
		if (p->level >= reqLevel) {
			MenuItem equip;
			if (p->armor.get() != this) 
				equip = MenuItem(L"Equip", BRIGHT_GREEN);
			else 
				equip = MenuItem(L"Unequip", BRIGHT_GREEN);
			options.push_back(equip);
		}
		options.push_back(MenuItem(L"Destroy", RED));
		options.push_back(MenuItem(L"Back", WHITE));

		return menuHandle(p, options, texts);
	}
};

class Armor : public Item {
public:
	Armor() {
		type = ItemType::ARMOR;
		count = 1;
		stackable = false;
		maxDurability = durability;
	}
	virtual int used(Player* player) {
		if (player->armor.get() != this) {
			player->defence = prot;
			return 2;
		}
		else {
			player->defence = 0;
			return 4;
		}
	}
	virtual void onRemove(Player* p) {
		p->removeItem(name, 1);
		if (p->armor.get() == this)
			p->armor = nullptr;
	}
	virtual int itemMenu(Player* p) {
		MenuItem nameI(name, color);
		MenuItem loreI(lore, WHITE);
		wchar_t s[256];
		wsprintf(s, L"Gives you %d defence", prot);
		MenuItem prot(s, YELLOW);
		wchar_t d[256];
		wsprintf(d, L"Durability: %d/%d", durability, maxDurability);
		MenuItem dur(d, BRIGHT_BLUE);
		std::vector<MenuItem> texts({ nameI, loreI, prot, dur });
		if (p->level < reqLevel) {
			wchar_t e[256];
			wsprintf(e, L"Required Level: %d", reqLevel);
			texts.push_back(MenuItem(e, RED));
		}

		std::vector<MenuItem> options;
		if (p->level >= reqLevel) {
			MenuItem equip;
			if (p->armor.get() != this)
				equip = MenuItem(L"Equip", BRIGHT_GREEN);
			else
				equip = MenuItem(L"Unequip", BRIGHT_GREEN);
			options.push_back(equip);
		}
		options.push_back(MenuItem(L"Destroy", RED));
		options.push_back(MenuItem(L"Back", WHITE));

		return menuHandle(p, options, texts);
	}
};

class Usable : public Item {
public:
	Usable() {
		type = ItemType::USABLE;
		count = 1;
		stackable = true;
	}
	virtual int used(Player* player) {
		return 5;
	}
	virtual void onRemove(Player* p) {
		p->removeItem(name, 1);
		if (p->armor.get() == this)
			p->armor = nullptr;
	}
	virtual int itemMenu(Player* p) {
		MenuItem nameI(name, color);
		MenuItem loreI(lore, WHITE);
		wchar_t d[256];
		wsprintf(d, L"Amount: %d", count);
		MenuItem coun(d, BRIGHT_BLUE);
		std::vector<MenuItem> texts({ nameI, loreI, coun });

		MenuItem use(L"Use", BRIGHT_GREEN);
		MenuItem remove(L"Destroy", RED);
		MenuItem back(L"Back", WHITE);
		std::vector<MenuItem> options({ use, remove, back });

		return menuHandle(p, options, texts);
	}
};

// Tile items

class GoldPile : public Item {
public:
	GoldPile(int minGold, int maxGold) {
		type = ItemType::RESOURCE;
		name = L"gold";
		count = randMinMax(minGold, maxGold);
	}

	int picked(Player* player) {
		player->gold += count;
		return 0;
	}
};

// Weapons

class WoodenSword : public Weapon {
public:
	WoodenSword(int dur = 100) {
		name = L"Wooden Sword";
		lore = L"Basic wooden sword";
		color = GREY;
		symbol = L"┼";
		minDmg = 3;
		maxDmg = 5;
		speed = 1;
		reqLevel = 0;
		durability = dur;
		maxDurability = 100;
	}
};

class IronShortSword : public Weapon {
public:
	IronShortSword(int dur = 200) {
		name = L"Iron Shortsword";
		lore = L"Quick weapon";
		color = BRIGHT_BLUE;
		symbol = L"┼";
		minDmg = 3;
		maxDmg = 8;
		speed = 2;
		reqLevel = 2;
		durability = dur;
		maxDurability = 200;
	}
};

// Armor

class Gambeson : public Armor {
public:
	Gambeson(int dur) {
		name = L"Gambeson";
		lore = L"Basic padded cloth armor";
		color = GREY;
		symbol = L"O";
		prot = 2;
		reqLevel = 0;
		durability = dur;
		maxDurability = 200;
	}
};

// Usables

class HealthPotion : public Usable {
public:
	virtual int used(Player* p) {
		p->health += 100;
		if (p->health > p->maxHealth) p->health = p->maxHealth;
		p->removeItem(name, 1);
		return 5;
	}

	HealthPotion() {
		name = L"Health Potion";
		lore = L"Heals 100 HP";
		color = BRIGHT_BLUE;
		symbol = L"▲";
		reqLevel = 0;
	}
};


#endif // !ITEMS