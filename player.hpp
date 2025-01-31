#ifndef PLAYER
#define PLAYER

class Player;

enum class ItemType {
	RESOURCE = 0,
	WEAPON = 1,
	ARMOR = 2,
	USABLE = 3
};

enum class BuffType {
	DMG = 0,
	SPD = 1,
	PROT = 2,
};

class Item {
public:
	std::wstring name = L"";
	std::wstring symbol = L"@";
	std::wstring lore = L"";
	unsigned char colord = YELLOW;
	ItemType type = ItemType::RESOURCE;
	bool stackable = true;
	int count = 0;
	int minDmg = 0;
	int maxDmg = 0;
	int prot = 0;
	int speed = 1;
	int reqLevel = 0;
	int durability = 0;
	int maxDurability = 0;
	int cost = 0;
	int ID = 0;

	Item() {}
	Item(const wchar_t* name, ItemType type, const wchar_t* symbol = L"@", unsigned char color = YELLOW) : name(name), type(type) {}
	// 1 - Add to inventory
	virtual int picked(Player* player) { return 1; }
	virtual int used(Player* player) { return 0; }
	virtual void writeMessage() { return; };
	virtual std::pair<int, std::function<void()>> menuHandle(Player* p, std::vector<MenuItem>& options, std::vector<MenuItem>& texts) {
		Menu menu(&options, &texts, true);
		int choice = -2;
		while (choice == -2) {
			choice = menu.open();
			if (choice == 0 && options.size() == 3)
				return { used(p), [this]() { this->writeMessage(); } };
			if ((choice == 1 && options.size() == 3) || (choice == 0 && options.size() == 2)) {
				MenuItem option(L"Are you sure you want to destroy this item?", RED);
				MenuItem no(L"No", WHITE);
				MenuItem yes(L"Yes", WHITE);
				std::vector<MenuItem> options({ no, yes });
				Menu deleteMenu(&options, option);
				int confirmation = deleteMenu.open();
				if (confirmation != 1)
					choice = -2;
				else {
					onRemove(p);
					return { -1, [this]() { this->writeMessage(); } };
				}
			}
		}
		return { 0, [this]() { this->writeMessage(); } };
	}

	virtual std::pair<int, std::function<void()>> itemMenu(Player* p) { return {}; }
	virtual void onRemove(Player* p) { }
};

class Recipe {
public:
	std::shared_ptr<Item> item = std::shared_ptr<Item>(new Item());
	std::vector<std::shared_ptr<Item>> items;
	Recipe(std::vector<std::shared_ptr<Item>> items) : items(items) {};
	Recipe() {};

	virtual std::shared_ptr<Item> getItem() {
		return std::shared_ptr<Item>(new Item);
	}

	virtual std::shared_ptr<Item> craft(std::map<std::wstring, std::shared_ptr<Item>>& inv, int& curID, int& invTaken) {
		std::vector<MenuItem> texts = std::vector<MenuItem>();
		std::vector<MenuItem> options = std::vector<MenuItem>();
		texts.push_back(MenuItem(item->name.c_str(), item->colord));
		texts.push_back(MenuItem(item->lore.c_str(), WHITE));
		// Max 10 different ingredients
		wchar_t s[10][128];
		int check = 0;
		int count = 0;
		for (auto req : items) {
			wsprintf(s[count], L"(0/%d)", req->count);
			wsprintf(s[count], L"%s %s", color(req->name.c_str(), req->colord).c_str(), color(s[count], RED).c_str());
			texts.push_back(MenuItem(2, s[count]));
			for (auto item : inv) {
				std::shared_ptr<Item> it = item.second;
				std::wstring nameReq = it->ID == 0 ? req->name : req->name + std::to_wstring(it->ID);
				std::wstring nameIt = it->ID == 0 ? it->name : it->name + std::to_wstring(it->ID);
				if (nameReq == nameIt) {
					bool enough = it->count >= req->count;
					wsprintf(s[count], L"(%d/%d)", it->count, req->count);
					wsprintf(s[count], L"%s %s", color(req->name.c_str(), req->colord).c_str(), color(s[count], enough ? GREEN : RED).c_str());
					count++;
					if (enough)
						check++;
					break;
				}
			}
		}

		if (check >= items.size()) 
			options.push_back(MenuItem(L"Craft", GREEN));
		else
			texts.push_back(MenuItem(L"Insufficient resources", RED));

		options.push_back(MenuItem(L"Back", WHITE));

		Menu recipesMenu(&options, &texts);
		int choice = recipesMenu.open();

		if (check >= items.size() && choice == 0) {
			for (auto req : items)
				for (auto item : inv) {
					std::shared_ptr<Item> it = item.second;
					std::wstring nameReq = it->ID == 0 ? req->name : req->name + std::to_wstring(it->ID);
					std::wstring nameIt = it->ID == 0 ? it->name : it->name + std::to_wstring(it->ID);
					if (nameReq == nameIt && it->count >= req->count)
						inv[nameIt]->count -= req->count;
				}
			
			return getItem();
		}

		return std::shared_ptr<Item>(new Item());
	};
};

class Buff {
public:
	BuffType type;
	int amount;
	int duration;
	bool isMultiplier;
	bool isBuffing = false;

	Buff(BuffType type, float amount, int duration, int isMultiplier = false) : type(type), amount(amount), duration(duration), isMultiplier(isMultiplier) {};
	void tick() { duration--;  };
};

class Player {
public:
	std::map<std::wstring, std::shared_ptr<Item>> inv;
	std::vector<std::shared_ptr<Recipe>> recipes;
	std::vector<Buff> buffs = {};
	std::shared_ptr<Item> weapon = nullptr;
	std::shared_ptr<Item> armor = nullptr;
	const int maxInvSpace = 37;
	int curInvTaken = 0;
	int health = 100;
	int maxHealth = 100;
	int baseDamage = 1;
	int minDamage = 1;
	int maxDamage = 1;
	int buffDamage = 0;
	int defence = 0;
	int buffDefence = 0;
	int baseSpeed = 1;
	int buffSpeed = 0;
	int gold = 0;
	int xp = 0;
	int expForNext = 100;
	int level = 1;
	int curRoomNum = 0;
	int curFloor = 0;
	int curItemID = 0;
	int x = 1;
	int y = 1;
	bool attackedThisTurn = false;

	void addItem(std::shared_ptr<Item> item) {
		if (item->stackable) {
			if (inv.find(item->name) == inv.end()) {
				inv.insert({ item->name, item });
				curInvTaken++;
			}
			else
				inv[item->name]->count += item->count;
		}
		else {
			curItemID++;
			item->ID = curItemID;
			std::wstring s = item->name + std::to_wstring(curItemID);
			inv.insert({ s, item });
			curInvTaken++;
		}
	};

	int removeItem(std::wstring name, int count, int ID = 0) {
		std::wstring realName;
		if (ID == 0)
			realName = name;
		else
			realName = name + std::to_wstring(ID);


		if (inv.find(realName) != inv.end()) {
			inv[realName]->count -= count;
			return 0;
		}
		else
			return -1;
	};

	// Create char array for equipable item for inventory menu
	int itemChar(wchar_t s[128], std::shared_ptr<Item> item, bool selected = false) {
		char durColor = RED;
		wsprintf(s, L"%d/%d", item->durability, item->maxDurability);
		if(item->durability * 100 / item->maxDurability > 30)
			durColor = YELLOW;
		if (item->durability * 100 / item->maxDurability > 60)
			durColor = GREEN;
		wsprintf(s, L"%ls (%ls)%ls", color(item->name.c_str(), item->colord).c_str(), color(s, durColor).c_str(), color(selected ? L" ▼" : L"  ", YELLOW).c_str());
		return 3;
	}

	// Create char array for stackable item for inventory menu
	int itemCharStack(wchar_t s[128], std::shared_ptr<Item> item) {
		wsprintf(s, L"%d", item->count);
		wsprintf(s, L"%s x %s", color(item->name.c_str(), item->colord).c_str(), color(s, WHITE).c_str());
		return 2;
	}

	// Remove element from inventory menu
	void removeElement(int idx, wchar_t s[38][128], std::vector<MenuItem>& it, std::vector<std::shared_ptr<Item>>& tIt, Menu& inv) {
		it.erase(it.begin() + idx);
		tIt.erase(tIt.begin() + idx);

		// Shift s elements to the left, don't move the last one (inventory count)
		for (int i = idx; i < maxInvSpace - 1; ++i)
			std::memmove(s[i], s[i + 1], 128 * sizeof(wchar_t));

		// Remove the "back" MenuItem so that it doesn't get affected by the s shift
		it.pop_back();

		// Update MenuItems after the shift
		for (int i = idx; i < it.size(); ++i)
			it[i].text = s[i];

		// Add back "back"
		MenuItem back(L"Back", WHITE);
		it.push_back(back);

		std::memset(s[maxInvSpace - 1], 0, 128 * sizeof(wchar_t));

		wsprintf(s[maxInvSpace], L"Inventory (%d / %d)", (int)tIt.size(), maxInvSpace);

		// Refresh inventory menu
		inv.init(&it);
	}

	// I - show inventory
	std::function<void()> showInventory() {
		std::vector<MenuItem> items = std::vector<MenuItem>();
		std::vector<std::shared_ptr<Item>> trueItems = std::vector<std::shared_ptr<Item>>();
		wchar_t s[38][128];
		for (auto item : inv) {
			if (item.second->count > 0) {
				trueItems.push_back(item.second);
				int count = (int)trueItems.size() - 1;
				if (item.second == armor || item.second == weapon) {
					itemChar(s[count], trueItems[count], true);
					items.push_back(MenuItem(3, s[count]));
				}
				else {
					int colors = 0;
					if (item.second->stackable)
						colors = itemCharStack(s[count], trueItems[count]);
					else
						colors = itemChar(s[count], trueItems[count]);
					items.push_back(MenuItem(colors, s[count]));
				}
			}
		}

		wsprintf(s[maxInvSpace], L"Inventory (%d / %d)", (int)trueItems.size(), maxInvSpace);
		MenuItem title(s[maxInvSpace], BRIGHT_CYAN);
		// Close inventory button
		items.push_back(MenuItem(L"Back", WHITE));

		std::function<void()> writeMessage = [](){};

		Menu inventory(&items, title);
		int choice = 0;
		while (choice != -1 && choice < trueItems.size()) {
			choice = inventory.open(choice);
			if (choice != -1 && choice < trueItems.size()) {
				std::shared_ptr<Item> item = trueItems[choice];
				// Item menu
				std::pair<int, std::function<void()>> result = item->itemMenu(this);
				// Update menu after item got destroyed
				if (result.first == -1)
					removeElement(choice, s, items, trueItems, inventory);
				// Equip weapon
				else if (result.first == 1) {
					// Update previously equiped
					for (int i = 0; i < trueItems.size(); i++)
						if (trueItems[i] == weapon)
							itemChar(s[i], trueItems[i], false);
					weapon = item;
				}
				// Equip armor
				else if (result.first == 2) {
					// Update previously equiped
					for (int i = 0; i < trueItems.size(); i++)
						if (trueItems[i] == armor)
							itemChar(s[i], trueItems[i], false);
					armor = item;
				}
				// Unequip weapon/armor
				else if (result.first == 3 || result.first == 4) {
					if (result.first == 3) weapon = nullptr;
					else if (result.first == 4) armor = nullptr;
					itemChar(s[choice], item, false);
				}
				else if (result.first == 5) {
					writeMessage = result.second;
					if (item->count > 0) 
						itemCharStack(s[choice], item);
					else
						removeElement(choice, s, items, trueItems, inventory);
				}

				if (item == armor || item == weapon)
					itemChar(s[choice], item, true);
			}
		}
		return writeMessage;
	}


	// C - show recipes
	void showCrafting() {
		const int recipesSize = 50;
		std::vector<MenuItem> items = std::vector<MenuItem>();
		wchar_t s[recipesSize+1][128]; 

		for (auto recipe : recipes) {
			wsprintf(s[items.size()], L"%s", color(recipe->item->name.c_str(), recipe->item->colord).c_str());
			items.push_back(MenuItem(1, s[items.size()]));
		}

		wsprintf(s[recipesSize], L"Recipes ");
		MenuItem title(s[recipesSize], BRIGHT_CYAN);

		items.push_back(MenuItem(L"Back", WHITE));

		Menu recipesMenu(&items, title);
		int choice = 0;

		while (choice != -1 && choice < recipes.size()) {
			choice = recipesMenu.open(choice);
			if (choice != -1 && choice < recipes.size()) {
				std::shared_ptr<Item> res = recipes[choice]->craft(inv, curItemID, curInvTaken);
				if (res->count != 0)
					addItem(res);
			}
		}
	}

	void levelUp() {
		level += 1;
		expForNext = (int)(expForNext*3)/2;
		baseDamage += 1;
		maxHealth += 10;
		health = maxHealth;
	}

	void updateStats() {
		if (armor != nullptr) 
			defence = armor->prot + buffDefence;
		else
			defence = buffDefence;
		if (weapon != nullptr) {
			minDamage = weapon->minDmg + baseDamage + buffDamage;
			maxDamage = weapon->maxDmg + baseDamage + buffDamage;
		}
		else {
			minDamage = baseDamage + buffDamage;
			maxDamage = baseDamage + buffDamage;
		}
	}

	void checkLevelUp() {
		if (xp >= expForNext) {
			xp = xp - expForNext;
			levelUp();
		}
	}

	int hit(int dmg) {
		if (armor != nullptr) {
			armor->durability--;
			if (armor->durability <= 0) {
				removeItem(armor->name, 1, armor->ID);
				armor = nullptr;
				defence = 0;
			}
		}
		health -= std::max<int>(dmg - defence, dmg / 4);
		return health;
	}
	
	int attack() {
		if (weapon != nullptr) {
			weapon->durability--;
			if (weapon->durability <= 0) {
				removeItem(weapon->name, 1, weapon->ID);
				weapon = nullptr;
				maxDamage = baseDamage;
				minDamage = baseDamage;
			}
		}
		return randMinMax(minDamage, maxDamage);
	}

	void giveBuff(BuffType type, float amount, int duration, int isMultiplier = false) {
		buffs.push_back(Buff(type, amount, duration, isMultiplier));
	}
    
	void buffStat(bool isBuff, std::vector<int*>& stats, Buff& buff) {
		for (int* stat : stats) {
			if (buff.isMultiplier) {
				if (isBuff) *stat *= buff.amount;
				else *stat /= buff.amount;
			}
			else {
				if (isBuff) *stat += buff.amount;
				else *stat -= buff.amount;
			}
		}
		buff.isBuffing = true;
	}

	void checkBuffs() {
		updateStats();

		for (int i = 0; i < buffs.size(); i++) {
			std::vector<int*> stats = {};
			Buff& buff = buffs[i];

			if (!buff.isBuffing || buff.duration <= 0) {
				switch (buff.type) {
				case BuffType::DMG:
					stats = { &buffDamage };
					buffStat(buff.duration > 0, stats, buff);
					break;
				case BuffType::SPD:
					stats = { &buffSpeed };
					buffStat(buff.duration > 0, stats, buff);
					break;
				case BuffType::PROT:
					stats = { &buffDefence };
					buffStat(buff.duration > 0, stats, buff);
					break;
				default:
					break;
				}
			}

			updateStats();

			if (buff.duration <= 0) {
				buffs.erase(buffs.begin() + i);
				return;
			}

			buff.tick();
		}
	}
};


#endif // !PLAYER
