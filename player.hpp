#ifndef PLAYER
#define PLAYER

class Player;

enum class ItemType {
	RESOURCE = 0,
	WEAPON = 1,
	ARMOR = 2,
	USABLE = 3
};

class Item : std::enable_shared_from_this<Item> {
public:
	const wchar_t* name = L"";
	const wchar_t* symbol = L"@";
	const wchar_t* lore = L"";
	ItemType type = ItemType::RESOURCE;
	int count = 0;
	int minDmg = 0;
	int maxDmg = 0;
	int prot = 0;
	int speed = 1;
	int reqLevel = 0;
	int durability = 0;
	int maxDurability = 0;
	int ID = 0;
	unsigned char color = YELLOW;
	bool stackable = true;
	Item() {}
	Item(const wchar_t* name, ItemType type, const wchar_t* symbol = L"@", unsigned char color = YELLOW) : name(name), type(type) {}
	// 1 - Add to inventory
	virtual int picked(Player* player) { return 1; }
	// 1 - Weapon, 2 - Armor
	virtual int used(Player* player) { return 0; }
	virtual int menuHandle(Player* p, std::vector<MenuItem>& options, std::vector<MenuItem>& texts) {
		Menu menu(&options, &texts, true);
		int choice = -2;
		while (choice == -2) {
			choice = menu.open();
			if (choice == 0 && options.size() == 3)
				return used(p);
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
					return -1;
				}
			}
		}
		return 0;
	}
	virtual int itemMenu(Player* p) { return 0; }
	virtual void onRemove(Player* p) { }
};

class Player {
public:
	std::map<std::wstring, std::shared_ptr<Item>> inv;
	std::shared_ptr<Item> weapon = nullptr;
	std::shared_ptr<Item> armor = nullptr;
	const int maxInvSpace = 32;
	int curInvTaken = 0;
	int health = 100;
	int maxHealth = 100;
	int baseDamage = 1;
	int minDamage = 1;
	int maxDamage = 1;
	int defence = 0;
	int gold = 0;
	int xp = 0;
	int expForNext = 100;
	int level = 2;
	int curRoomNum = 0;
	int curFloor = 0;
	int curItemID = 0;
	int x = 1;
	int y = 1;

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
			if (inv[realName]->count - count < 0) {
				inv.erase(realName);
				curInvTaken--;
			}
			return 0;
		}
		else
			return -1;
	};

	void showInventory() {
		std::vector<MenuItem> items;
		std::vector<std::shared_ptr<Item>> trueItems;
		// Indicator that the armor/weapon is currently being worn by the player
		const wchar_t* selected = L"▼";
		wchar_t s[33][128];
		for (auto item : inv) {
			if (item.second->count > 0) {
				trueItems.push_back(item.second);
				int count = trueItems.size() - 1;
				if (item.second == armor || item.second == weapon) {
					wsprintf(s[count], L"%s (%d/%d) %s", trueItems[count]->name, trueItems[count]->durability, trueItems[count]->maxDurability, selected);
					items.push_back(MenuItem(s[count], trueItems[count]->color));
				}
				else {
					if(item.second->stackable)
						wsprintf(s[count], L"%s x %d", trueItems[count]->name, trueItems[count]->count);
					else
						wsprintf(s[count], L"%s (%d/%d)", trueItems[count]->name, trueItems[count]->durability, trueItems[count]->maxDurability);
					items.push_back(MenuItem(s[count], trueItems[count]->color));
				}
			}
		}

		int itemCount = (int)items.size();
		wsprintf(s[maxInvSpace], L"Inventory (%d / %d)", (int)trueItems.size(), 32);
		MenuItem title(s[maxInvSpace], BRIGHT_CYAN);

		// Close inventory button
		items.push_back(MenuItem(L"Back", WHITE));

		Menu inventory(&items, title);
		int choice = 0;
		while (choice != -1 && choice < trueItems.size()) {
			choice = inventory.open(choice);
			if (choice != -1 && choice < trueItems.size()) {
				std::shared_ptr<Item> item = trueItems[choice];
				// Item menu
				int result = item->itemMenu(this);
				// Update menu after item got destroyed
				if (result == -1) {
					items.erase(items.begin() + choice);
					trueItems.erase(trueItems.begin() + choice);
				}
				// Equip weapon
				else if (result == 1) weapon = item;
				// Equip armor
				else if (result == 2) armor = item;
				// Unequip weapon/armor
				else if (result == 3 || result == 4) {
					if (result == 3) weapon = nullptr;
					else if (result == 4) armor = nullptr;
					wsprintf(s[choice], L"%s (%d/%d)", item->name, item->durability, item->maxDurability);
				}
				else if (result == 5) {
					if (item->count > 0) 
						wsprintf(s[choice], L"%s x %d", item->name, item->count);
					else {
						items.erase(items.begin() + choice);
						trueItems.erase(trueItems.begin() + choice);
					}
				}

				// Update selected weapon/armor
				if (result == 1 || result == 2)
					for (int i = 0; i < trueItems.size(); i++) {
						std::shared_ptr<Item> curIt = trueItems[i];
						if ((curIt->type == ItemType::ARMOR && armor != curIt) || (curIt->type == ItemType::WEAPON && weapon != curIt))
							wsprintf(s[i], L"%s (%d/%d)", trueItems[i]->name, trueItems[i]->durability, trueItems[i]->maxDurability);
					}

				if (item == armor || item == weapon)
					wsprintf(s[choice], L"%s (%d/%d) %s", item->name, item->durability, item->maxDurability, selected);
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
};



#endif // !PLAYER
