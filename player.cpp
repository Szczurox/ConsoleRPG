#include<functional>
#include<filesystem>
#include<algorithm>
#include<windows.h>
#include<iostream>
#include<fstream>
#include<sstream>

#include"player.hpp"
#include"effective.hpp"
#include"items.hpp"
#include"utils.hpp"
#include"enemy.hpp"
#include"menu.hpp"

void Player::save(std::wstring fileName) {
	std::ofstream playerSave(fileName.c_str());
	for (std::pair<std::wstring, std::shared_ptr<Item>> item : inv)
		item.second->save(playerSave);
	playerSave << "EndInv\n";
	for (std::pair<std::string, std::shared_ptr<Recipe>> recipe : recipes)
		recipe.second->save(playerSave);
	playerSave << "EndRecs\n";
	for (std::shared_ptr<Buff> buff : buffs)
		buff->save(playerSave);
	playerSave << "EndBuff\n";
	playerSave << seed << " " << curInvTaken << " " << health << " " << maxHealth << " " << baseDamage << " " << defence << " ";
	playerSave << baseSpeed << " " << gold << " " << xp << " " << expForNext << " " << level << " ";
	playerSave << curRoomNum << " " << curFloor << " " << curItemID << " " << x << " " << y << " " << faith << "\n";
}

unsigned int Player::load(std::wstring fileName, ItemFactory& factory) {
	std::ifstream file(fileName.c_str());
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
		std::shared_ptr<Buff> buff = std::shared_ptr<Buff>(new Buff());
		buff->load(iss);
		buffs.push_back(buff);
	}
	file >> seed >> curInvTaken >> health >> maxHealth >> baseDamage >> defence >> baseSpeed >> gold >> xp >> expForNext >> level;
	file >> curRoomNum >> curFloor >> curItemID >> x >> y >> faith;

	return seed;
}

void Player::addItem(std::shared_ptr<Item> item, bool isLoading) {
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

int Player::removeItem(std::wstring name, int count, int ID) {
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
int Player::itemChar(std::wstring& s, std::shared_ptr<Item> item, bool selected) {
	char durColor = RED;
	std::wstringstream str;
	std::wstring dur = L" (" + std::to_wstring(item->durability) + L"/" + std::to_wstring(item->maxDurability) + L")";
	if (item->durability * 100 / item->maxDurability > 30)
		durColor = YELLOW;
	if (item->durability * 100 / item->maxDurability > 60)
		durColor = GREEN;
	str << color(item->name, item->colord) << color(dur, durColor) << color(selected ? L" ▼" : L"  ", YELLOW);
	s = str.str();
	return 3;
}

int Player::itemChar(std::shared_ptr<MenuItem> m, std::shared_ptr<Item> item, bool selected) {
	itemChar(m->text, item, selected);
	return 3;
}

// Create char array for stackable item for inventory menu
int Player::itemCharStack(std::wstring& s, std::shared_ptr<Item> item) {
	std::wstringstream str;
	str << color(item->name, item->colord) << L" × " << color(std::to_wstring(item->count), WHITE);
	s = str.str();
	return 2;
}

// Remove element from inventory menu
void Player::removeElement(int idx, std::vector<std::shared_ptr<MenuItem>>& it, std::vector<std::shared_ptr<Item>>& tIt, Menu& inv) {
	it.erase(it.begin() + idx);
	tIt.erase(tIt.begin() + idx);

	inv.texts[0]->text = L"Inventory (" + std::to_wstring(tIt.size()) + L" / " + std::to_wstring(maxInvSpace) + L")";

	// Refresh inventory menu
	inv.init(it);
}

// I - show inventory
std::function<void()> Player::showInventory() {
	std::vector<std::shared_ptr<MenuItem>> items = {};
	std::vector<std::shared_ptr<Item>> trueItems = std::vector<std::shared_ptr<Item>>();
	for (auto item : inv) {
		if (item.second->count > 0) {
			trueItems.push_back(item.second);
			int count = (int)trueItems.size() - 1;
			if (item.second == armor || item.second == weapon) {
				std::wstring s;
				itemChar(s, trueItems[count], true);
				items.push_back(createMenuItem(3, s));
			}
			else {
				std::wstring s;
				int colors = 0;
				if (item.second->stackable)
					colors = itemCharStack(s, trueItems[count]);
				else
					colors = itemChar(s, trueItems[count], false);
				items.push_back(createMenuItem(colors, s));
			}
		}
	}

	std::wstring s = L"Inventory (" + std::to_wstring(items.size()) + L" / " + std::to_wstring(maxInvSpace) + L")";
	std::shared_ptr<MenuItem> title = createMenuItem(s, BRIGHT_CYAN);
	// Close inventory button
	items.push_back(createMenuItem(L"Back", WHITE));

	std::function<void()> writeMessage = []() {};

	Menu inventory(items, title);

	int choice = 0;
	while (choice != -1 && choice < trueItems.size()) {
		choice = inventory.open(choice);
		if (choice != -1 && choice < trueItems.size()) {
			std::shared_ptr<Item> item = trueItems[choice];
			// Item menu
			std::pair<int, std::function<void()>> result = item->itemMenu(this);
			// Update menu after item got destroyed
			if (result.first == -1)
				removeElement(choice, items, trueItems, inventory);
			// Equip weapon
			else if (result.first == 1) {
				// Update previously equiped
				for (int i = 0; i < trueItems.size(); i++)
					if (trueItems[i] == weapon)
						itemChar(items[i], trueItems[i], false);
				weapon = item;
				inventory.refresh();
			}
			// Equip armor
			else if (result.first == 2) {
				// Update previously equiped
				for (int i = 0; i < trueItems.size(); i++)
					if (trueItems[i] == armor)
						itemChar(items[i], trueItems[i], false);
				armor = item;
				inventory.refresh();
			}
			// Unequip weapon/armor
			else if (result.first == 3 || result.first == 4) {
				if (result.first == 3) weapon = nullptr;
				else if (result.first == 4) armor = nullptr;
				itemChar(items[choice], item, false);
			}
			else if (result.first == 5) {
				writeMessage = result.second;
				if (item->count > 0)
					itemCharStack(items[choice]->text, item);
				else
					removeElement(choice, items, trueItems, inventory);
			}

			if (item == armor || item == weapon)
				itemChar(items[choice], item, true);
			inventory.refresh();
		}
	}
	return writeMessage;
}


// C - show recipes
void Player::showCrafting() {
	std::vector<std::shared_ptr<Recipe>> recipesVec;
	std::vector<std::shared_ptr<MenuItem>> items = {};

	for (auto recipe : recipes) {
		items.push_back(createMenuItem(1, color(recipe.second->item->name.c_str(), recipe.second->item->colord).c_str()));
		recipesVec.push_back(recipe.second);
	}

	std::shared_ptr<MenuItem> title = createMenuItem(L" Recipes", BRIGHT_CYAN);

	items.push_back(createMenuItem(L"Back", WHITE));

	Menu recipesMenu(items, title);
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

void Player::levelUp() {
	level += 1;
	expForNext = (int)(expForNext * 3) / 2;
	baseDamage += 1;
	maxHealth += 10;
	health = maxHealth;
}

void Player::updateStats() {
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

void Player::checkLevelUp() {
	if (xp >= expForNext) {
		xp = xp - expForNext;
		levelUp();
	}
}

int Player::hit(int dmg) {
	if (armor != nullptr) {
		armor->durability--;
		armor->special(this, dmg);
		checkBuffs(false);
	}
	health -= std::max<int>(dmg - defence, dmg / 4);
	if (armor != nullptr)
		if (armor->durability <= 0)
			armor->onRemove(this);
	return health;
}

int Player::attack() {
	if (attackedThisTurn)
		return 0;
	if (weapon != nullptr) {
		weapon->durability--;
		weapon->special(this, 0);
		checkBuffs(false);
	}
	int dmg = randMinMax(minDamage, maxDamage);
	if (weapon != nullptr)
		if (weapon->durability <= 0)
			weapon->onRemove(this);
	return dmg;
}

std::pair<std::wstring, unsigned char> Player::giveBuff(BuffType type, float amount, int duration, bool isNegative) {
	std::shared_ptr<Buff> buff = std::shared_ptr<Buff>(new Buff(type, amount, duration, isNegative));
	buffs.push_back(buff);
	return buff->getType();
}

void Player::buffStat(bool isBuff, std::vector<int*> stats, std::shared_ptr<Buff> buff) {
	for (int* stat : stats) {
		if (*stat <= 0 && buff->isNegative)
			continue;
		if ((isBuff && !buff->isNegative) || (!isBuff && buff->isNegative)) *stat += buff->amount;
		else *stat -= buff->amount;
	}
	buff->isBuffing = true;
}

void Player::checkBuffs(bool passTurn) {
	updateStats();

	for (int i = 0; i < buffs.size(); i++) {
		std::vector<int*> stats = {};
		std::shared_ptr<Buff> buff = buffs[i];

		if (!buff->isBuffing || buff->duration <= 0) {
			switch (buff->type) {
			case BuffType::DMG:
				stats = { &buffDamage };
				buffStat(buff->duration > 0, stats, buff);
				break;
			case BuffType::SPD:
				stats = { &buffSpeed };
				buffStat(buff->duration > 0, stats, buff);
				break;
			case BuffType::PROT:
				stats = { &buffDefence };
				buffStat(buff->duration > 0, stats, buff);
				break;
			default:
				break;
			};
		};

		if (buff->duration <= 0) {
			buffs.erase(buffs.begin() + i);
			return;
		}

		if(passTurn)
			buff->tick(this);
	}

	updateStats();
}