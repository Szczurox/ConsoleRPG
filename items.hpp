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
		if (p->weapon.get() == this)
			p->weapon = nullptr;
		p->removeItem(name, 1, ID);
	}

	virtual std::pair<int, std::function<void()>> itemMenu(Player* p) {
		MenuItem nameI(name.c_str(), colord);
		MenuItem loreI(lore.c_str(), WHITE);
		wchar_t s[256];
		wsprintf(s, L"Deals %d-%d damage and has %d speed", minDmg, maxDmg, speed);
		MenuItem damage(s, YELLOW);
		char durColor = RED;
		if (durability * 100 / maxDurability > 30)
			durColor = YELLOW;
		if (durability * 100 / maxDurability > 60)
			durColor = GREEN;
		wchar_t d[256];
		wsprintf(d, L"%d/%d", durability, maxDurability);
		wsprintf(d, L"%s %s", color(L"Durability:", BRIGHT_BLUE).c_str(), color(d, durColor).c_str());
		MenuItem dur(2, d);
		std::vector<MenuItem> texts({ nameI, loreI, damage, dur });
		if (p->level < reqLevel) {
			wchar_t e[256];
			wsprintf(e, L"Required Level: %d", reqLevel);
			texts.push_back(MenuItem(e, RED));
		}

		std::vector<MenuItem> options;
		if (p->level >= reqLevel) {
			MenuItem equip;
			if (p->weapon.get() != this) 
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
		if (p->armor.get() == this)
			p->armor = nullptr;
		p->removeItem(name, 1, ID);
	}
	virtual std::pair<int, std::function<void()>> itemMenu(Player* p) {
		MenuItem nameI(name.c_str(), colord);
		MenuItem loreI(lore.c_str(), WHITE);
		wchar_t s[256];
		wsprintf(s, L"Gives you %d defence", prot);
		MenuItem prot(s, YELLOW);
		char durColor = RED;
		if (durability * 100 / maxDurability >= 30)
			durColor = YELLOW;
		if (durability * 100 / maxDurability >= 70)
			durColor = GREEN;
		wchar_t d[256];
		wsprintf(d, L"%d/%d", durability, maxDurability);
		wsprintf(d, L"%s %s", color(L"Durability:", BRIGHT_BLUE).c_str(), color(d, durColor).c_str());
		MenuItem dur(2, d);
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
		p->removeItem(name, count);
	}
	virtual void writeMessage() { write(L"used!"); };
	virtual std::pair<int, std::function<void()>> itemMenu(Player* p) {
		MenuItem nameI(name.c_str(), colord);
		MenuItem loreI(lore.c_str(), WHITE);
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

class Resource : public Item {
public:
	Resource() {
		type = ItemType::RESOURCE;
		count = 1;
		stackable = true;
	}
	virtual int used(Player* player) {
		return 1;
	}
	virtual void onRemove(Player* p) {
		p->removeItem(name, count);
	}
	virtual std::pair<int, std::function<void()>> itemMenu(Player* p) {
		MenuItem nameI(name.c_str(), colord);
		MenuItem loreI(lore.c_str(), WHITE);
		wchar_t d[256];
		wsprintf(d, L"Amount: %d", count);
		MenuItem coun(d, BRIGHT_BLUE);
		std::vector<MenuItem> texts({ nameI, loreI, coun });

		MenuItem remove(L"Destroy", RED);
		MenuItem back(L"Back", WHITE);
		std::vector<MenuItem> options({ remove, back });

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
		stackable = false;
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
		lore = L"Simple wooden sword";
		colord = GREY;
		symbol = L"┼";
		minDmg = 3;
		maxDmg = 5;
		speed = 1;
		reqLevel = 0;
		durability = dur;
		maxDurability = 100;
		cost = 1000;
	}
};

class IronShortSword : public Weapon {
public:
	IronShortSword(int dur = 200) {
		name = L"Iron Shortsword";
		lore = L"Quick weapon, but not very accurate";
		colord = BRIGHT_BLUE;
		symbol = L"┼";
		minDmg = 0;
		maxDmg = 8;
		speed = 2;
		reqLevel = 2;
		durability = dur;
		maxDurability = 200;
		cost = 4000;
	}
};

// Armor
class Gambeson : public Armor {
public:
	Gambeson(int dur = 200) {
		name = L"Gambeson";
		lore = L"Basic padded cloth armor";
		colord = GREY;
		symbol = L"O";
		prot = 2;
		reqLevel = 0;
		durability = dur;
		maxDurability = 200;
		cost = 1000;
	}
};

class BoneArmor : public Armor {
public:
	BoneArmor(int dur = 100) {
		name = L"Bone Armor";
		lore = L"Armor made out of bones";
		colord = GREY;
		symbol = L"O";
		prot = 3;
		reqLevel = 0;
		durability = dur;
		maxDurability = 100;
		cost = 1000;
	}
};

// Consumables
class HealthPotion : public Usable {
public:
	virtual int used(Player* p) {
		p->health += 100;
		if (p->health > p->maxHealth) p->health = p->maxHealth;
		p->removeItem(name, 1);
		return 5;
	}

	virtual void writeMessage() {
		write(L"You drank %.\nHealed for 100HP!", color(name.c_str(), colord).c_str());
	};

	HealthPotion(int cnt = 1) {
		name = L"Health Potion";
		lore = L"Heals for 100 HP";
		colord = BRIGHT_BLUE;
		symbol = L"▲";
		cost = 500;
		count = cnt;
	}
};

class ZombieMeat : public Usable {
public:
	virtual int used(Player* p) {
		int random = randMinMax(0, 50);
		int buff = randMinMax(1, 3);
		if (random >= 80) {
			p->health += random;
			if (p->health > p->maxHealth) p->health = p->maxHealth;
		}
		else if (random < 60) 
			p->health -= random;
		p->giveBuff(BuffType::DMG, buff, 50, false);
		p->removeItem(name, 1);
		return 5;
	}

	virtual void writeMessage() {
		write(L"You ate %.\nYou feel strange.", color(name.c_str(), colord).c_str());
	};

	ZombieMeat(int cnt = 1) {
		name = L"Zombie Meat";
		lore = L"The odour is unbearable, but it looks somewhat edible";
		colord = GREEN;
		symbol = L"▬";
		stackable = true;
		cost = 200;
		count = cnt;
	}

private:

};

class BloodOath : public Usable {
public:
	virtual int used(Player* p) {
		int healthChange = randMinMax(5, 20);
		p->health -= healthChange;
		p->maxHealth -= healthChange;
		p->baseDamage += randMinMax(1, 3);
		p->removeItem(name, 1);
		return 5;
	}

	virtual void writeMessage() {
		write(color(L"There is no going back...", colord).c_str());
	};

	BloodOath(int cnt = 1) {
		name = L"Blood Oath";
		lore = L"You feel dark energy emanating from it";
		colord = RED;
		symbol = L"Ω";
		stackable = true;
		cost = 200;
		count = cnt;
	}
};

// Resources
class Bone : public Resource {
public:
	Bone(int cnt = 1) {
		name = L"Bone";
		lore = L"Can be obtained by killing skeletons";
		colord = GREY;
		symbol = L"/";
		cost = 100;
		count = cnt;
	}
};

#endif // !ITEMS