#ifndef PLAYER
#define PLAYER

#include<map>
#include<vector>
#include<sstream>
#include<fstream>

#include"crafting.hpp"
#include"effective.hpp"
#include"menu.hpp"

class NPC;
class Item;
class Recipe;
class Buff;
class ItemFactory;
class MenuItem;
class Menu;
enum class BuffType;

class Player {
public:
	std::map<std::wstring, std::shared_ptr<Item>> inv;
	std::map<std::string, std::shared_ptr<Recipe>> recipes;
	std::vector<std::shared_ptr<Buff>> buffs;
	std::shared_ptr<Item> weapon = nullptr;
	std::shared_ptr<Item> armor = nullptr;
	unsigned int seed = 0;
	static const int maxInvSpace = 32;
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

	void save(std::wstring fileName);

	unsigned int load(std::wstring fileName, ItemFactory& factory);

	template<class T>
	void addRecipe(bool unlocked) {
		std::shared_ptr<Recipe> recipe = std::shared_ptr<Recipe>(new T(unlocked));
		std::string type = recipe->getType();
		recipes[type] = recipe;
	};

	void addItem(std::shared_ptr<Item> item, bool isLoading = false);

	int removeItem(std::wstring name, int count, int ID = 0);

	// Create char array for equipable item for inventory menu
	int itemChar(std::wstring& s, std::shared_ptr<Item> item, bool selected = false);
	int itemChar(std::shared_ptr<MenuItem> m, std::shared_ptr<Item> item, bool selected = false);

	// Create char array for stackable item for inventory menu
	int itemCharStack(std::wstring& s, std::shared_ptr<Item> item);

	// Remove element from inventory menu 
	void removeElement(int idx, std::vector<std::shared_ptr<MenuItem>>& it, std::vector<std::shared_ptr<Item>>& tIt, Menu& inv);

	// I - show inventory
	std::function<void()> showInventory();

	// C - show recipes
	void showCrafting();

	void levelUp();

	void updateStats();

	void checkLevelUp();

	// Calculate damage player got dealt
	int hit(int dmg);
	
	// Calculate player's damage
	int attack();

	std::pair<std::wstring, unsigned char> giveBuff(BuffType type, float amount, int duration, bool isNegative = false);
    
	void buffStat(bool isBuff, std::vector<int*> stats, std::shared_ptr<Buff> buff);

	void checkBuffs();
};


#endif // !PLAYER
