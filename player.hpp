#ifndef PLAYER
#define PLAYER

class ItemFactory;

class Player {
public:
	std::map<std::wstring, std::shared_ptr<Item>> inv;
	std::map<std::string, std::shared_ptr<Recipe>> recipes;
	std::vector<Buff> buffs = {};
	std::shared_ptr<Item> weapon = nullptr;
	std::shared_ptr<Item> armor = nullptr;
	unsigned int seed = 0;
	static const int maxInvSpace = 65;
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
	int faith = 0;
	bool attackedThisTurn = false;

	Player() {}

	Player(unsigned int seed): seed(seed) {}

	void save(std::wstring fileName) {
		std::ofstream playerSave(fileName);
		for (std::pair<std::wstring, std::shared_ptr<Item>> item : inv)
			item.second->save(playerSave);
		playerSave << "EndInv\n";
		for (std::pair<std::string, std::shared_ptr<Recipe>> recipe : recipes)
			recipe.second->save(playerSave);
		playerSave << "EndRecs\n";
		for (Buff buff : buffs)
			buff.save(playerSave);
		playerSave << "EndBuff\n";
		playerSave << seed << " " << curInvTaken << " " << health << " " << maxHealth << " " << baseDamage << " " << defence << " ";
		playerSave << baseSpeed << " " << gold << " " << xp << " " << expForNext << " " << level << " " << curItemID << " ";
		playerSave << curRoomNum << " " << curFloor << " " << curItemID << " " << x << " " << y << " " << faith << "\n";
	}

	unsigned int load(std::wstring fileName, ItemFactory& factory) {
		std::ifstream file(fileName);
		std::string line = "";
		while (std::getline(file, line) && line != "EndInv") {
			std::istringstream iss(line);
			std::string itemType;
			iss >> itemType;
			std::shared_ptr<Item> item = factory.createItem(itemType);
			item->load(iss);
			addItem(item, true);
		}
		while (std::getline(file, line) && line != "EndRecs") {
			std::istringstream iss(line);
			std::string recipeType;
			iss >> recipeType;
			auto rec = recipes.find(recipeType);
			if (rec != recipes.end())
				rec->second->load(iss);
		}
		while (std::getline(file, line) && line != "EndBuff") {
			std::istringstream iss(line);
			std::string itemType;
			Buff buff;
			buff.load(iss);
		}
		file >> seed >> curInvTaken >> health >> maxHealth >> baseDamage >> defence >> baseSpeed >> gold >> xp >> expForNext >> level;
		file >> curItemID >> curRoomNum >> curFloor >> curItemID >> x >> y >> faith;

		return seed;
	}

	template<class T>
	void addRecipe(bool unlocked = false) {
		std::shared_ptr<Recipe> recipe = std::shared_ptr<Recipe>(new T(unlocked));
		std::string type = recipe->getType();
		recipes[type] = recipe;
	};

	void addItem(std::shared_ptr<Item> item, bool isLoading = false) {
		if (item->stackable) {
			if (inv.find(item->name) == inv.end()) {
				inv.insert({ item->name, item });
				curInvTaken++;
			}
			else
				inv[item->name]->count += item->count;
		}
		else {
			if (!isLoading) {
				curItemID++;
				item->ID = curItemID;
			}
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
	void removeElement(int idx, wchar_t s[66][128], std::vector<MenuItem>& it, std::vector<std::shared_ptr<Item>>& tIt, Menu& inv) {
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
		wchar_t s[66][128];
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
		const int recipesSize = 128;
		std::vector<std::shared_ptr<Recipe>> recipesVec;
		std::vector<MenuItem> items = std::vector<MenuItem>();
		wchar_t s[recipesSize+1][128];

		for (auto recipe : recipes) {
			wsprintf(s[items.size()], L"%s", color(recipe.second->item->name.c_str(), recipe.second->item->colord).c_str());
			items.push_back(MenuItem(1, s[items.size()]));
			recipesVec.push_back(recipe.second);
		}

		wsprintf(s[recipesSize], L"Recipes ");
		MenuItem title(s[recipesSize], BRIGHT_CYAN);

		items.push_back(MenuItem(L"Back", WHITE));

		Menu recipesMenu(&items, title);
		int choice = 0;

		while (choice != -1 && choice < recipes.size()) {
			choice = recipesMenu.open(choice);
			if (choice != -1 && choice < recipes.size()) {
				std::shared_ptr<Item> res = recipesVec[choice]->craft(inv, curItemID, curInvTaken);
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
		if (attackedThisTurn)
			return 0;
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
