#ifndef PLAYER
#define PLAYER

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
	const char* lore = "";
	ItemType type = ItemType::RESOURCE;
	int count = 0;
	int minDmg = 0;
	int maxDmg = 0;
	int prot = 0;
	int speed = 1;
	int reqLevel = 0;
	unsigned char color = YELLOW;
	Item() {}
	Item(const char* name, ItemType type, const char* symbol = "@", unsigned char color = YELLOW) : name(name), type(type) {}
	virtual int picked(Player* player) { return 0; }
	virtual int used(Player* player) { return 0; }
	virtual int itemMenu(Player* p) { return 0; };
};

class Player {
public:
	std::map<std::string, Item*> inv;
	int health = 100;
	int maxHealth = 100;
	int baseDamage = 1;
	int minDamage = 1;
	int maxDamage = 3;
	int defence = 0;
	int gold = 0;
	int exp = 0;
	int expForNext = 100;
	int level = 0;
	Item* weapon = nullptr;
	Item* armor = nullptr;
	size_t x = 1;
	size_t  y = 1;

	void addItem(Item* item) {
		if (inv.find(item->name) == inv.end())
			inv.insert({ item->name, item });
		else
			inv[item->name] = item;
	};

	int removeItem(std::string name, int count) {
		if (inv.find(name) != inv.end()) {
			if (inv[name]->count > 0)
				inv[name]->count -= count;
			else
				inv.erase(name);
			return 0;
		}
		else
			return -1;
	};

	void showInventory() {
		system("cls");
		MenuItem title("Inventory", BRIGHT_CYAN);
		std::vector<MenuItem*> items;
		std::vector<Item*> trueItems;
		// Indicator that the armor/weapon is currently being worn by the player
		const char* selected = "\37";
		for (auto item : inv) {
			if (item.second->count > 0) {
				trueItems.push_back(item.second);
				if (item.second == armor || item.second == weapon) {
					char s[256];
					sprintf_s(s, "%s x %d %s", item.second->name, item.second->count, selected);
					items.push_back(new MenuItem(s, WHITE));
				}
				else {
					char s[256];
					sprintf_s(s, "%s x %d", item.second->name, item.second->count);
					MenuItem itemMenu(s, WHITE);
					items.push_back(&itemMenu);
				}
			}
		}

		Menu inventory(&items, &title);
		int choice = 0;
		while (choice != -1) {
			choice = inventory.open();
			if (choice != -1) {
				Item* item = trueItems[choice];
				// Item menu
				int result = item->itemMenu(this);
				if (result == -1) {
					items.erase(items.begin()+choice);
					trueItems.erase(trueItems.begin() + choice);
				}
				if (item == armor || item == weapon) {
					char s[256];
					sprintf_s(s, "%s x %d %s", item->name, item->count, selected);
					items[choice]->text = s;
				}
			}
		}
	}

	void levelUp() {
		level += 1;
		exp = 0;
		expForNext = (int)(expForNext*3)/2;
		baseDamage += 1;
		maxHealth += 10;
		health = maxHealth;
	}

	int hit(int dmg) {
		health -= std::max<int>(dmg - defence, dmg / 4);
		return health;
	}
	
	int attack() {
		return rand() % (maxDamage-minDamage+1) + minDamage;
	}

	~Player() {
		delete weapon;
		delete armor;
	}
};



#endif // !PLAYER
