#ifndef ITEMS
#define ITEMS

#include<string>
#include<vector>
#include<functional>
#include<utility>
#include<map>
#include<memory>

#include"const.hpp"
#include"menu.hpp"

class MenuItem;
class Player;
class Menu;
class Item;

enum class ItemType {
	RESOURCE = 0,
	WEAPON = 1,
	ARMOR = 2,
	USABLE = 3
};

class ItemFactory {
public:
	std::map<std::string, std::function<std::shared_ptr<Item>()>> itemMap;
	ItemFactory() {};
	std::shared_ptr<Item> createItem(const std::string& type);
	template <class T>
	void registerItem() {
		std::string type = typeid(T).name();
		if (type.rfind("class ", 0) == 0)
			type = type.substr(6);
		itemMap[type] = []() -> std::shared_ptr<Item> {
			return std::make_shared<T>();
		};
	}
};

class Item {
public:
	std::wstring name = L"";
	std::wstring symbol = L"@";
	std::wstring lore = L"";
	unsigned char colord = YELLOW;
	ItemType type = ItemType::RESOURCE;
	bool stackable = true;
	int count = 1;
	int minDmg = 0;
	int maxDmg = 0;
	int prot = 0;
	int speed = 1;
	int reqLevel = 0;
	int durability = 0;
	int maxDurability = 0;
	int cost = 0;
	int ID = 0;
	int messageType = 0; // Helper variable for different messages in writeMessage

	Item() {}
	Item(const wchar_t* name, ItemType type) {};
	virtual int picked(Player* player) {
		return 1;
	}
	virtual int used(Player* player) {
		return 1;
	}
	virtual void writeMessage() {};
	virtual std::pair<int, std::function<void()>> menuHandle(Player* p, std::vector<std::shared_ptr<MenuItem>>& options, std::vector<std::shared_ptr<MenuItem>>& texts);
	virtual std::pair<int, std::function<void()>> itemMenu(Player* p) {
		return { 0, std::function<void()>() };
	}
	virtual void onRemove(Player* p) {};
	virtual std::string getType() const;
	virtual void save(std::ostream& os);
	virtual void load(std::istringstream& in);
};

// Weapon class
class Weapon : public Item {
public:
	Weapon() {
		stackable = false;
	};
    int used(Player* player);
    void onRemove(Player* p);
	void writeMessage() {};
    std::pair<int, std::function<void()>> itemMenu(Player* p);
};

// Armor class
class Armor : public Item {
public:
	Armor() {
		stackable = false;
	};
    int used(Player* player);
    void onRemove(Player* p);
	void writeMessage() {};
    std::pair<int, std::function<void()>> itemMenu(Player* p);
};

// Usable class
class Usable : public Item {
public:
	Usable() {};
    int used(Player* player);
    void onRemove(Player* p);
    virtual void writeMessage();
    std::pair<int, std::function<void()>> itemMenu(Player* p);
};

// Resource class
class Resource : public Item {
public:
	Resource() {};
	int used(Player* player);
	void onRemove(Player* p);
	std::pair<int, std::function<void()>> itemMenu(Player* p);
};

// Specific item classes
class GoldPile : public Item {
public:
	GoldPile(int minGold = 1, int maxGold = 1);

    int picked(Player* player);
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

class IronShortsword : public Weapon {
public:
	IronShortsword(int dur = 200) {
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
	virtual int used(Player* p);

	virtual void writeMessage();

	HealthPotion(int cnt = 1) {
		name = L"Health Potion";
		lore = L"Heals for 100 HP";
		colord = BRIGHT_BLUE;
		symbol = L"▲";
		stackable = true;
		cost = 500;
		count = cnt;
	}
};

class ZombieMeat : public Usable {
public:
	virtual int used(Player* p);

	virtual void writeMessage();

	ZombieMeat(int cnt = 1) {
		name = L"Zombie Meat";
		lore = L"The odour is unbearable, but it looks somewhat edible";
		colord = GREEN;
		symbol = L"▬";
		stackable = true;
		cost = 200;
		count = cnt;
	}
};

class BloodOath : public Usable {
public:
	virtual int used(Player* p);

	virtual void writeMessage();

	BloodOath(int cnt = 1) {
		name = L"Blood Oath";
		lore = L"You feel dark energy emanating from it";
		colord = RED;
		symbol = L"Ω";
		stackable = true;
		cost = 6666;
		count = cnt;
	}
};

class SacramentalBread : public Usable {
public:
	virtual int used(Player* p);

	virtual void writeMessage();

	SacramentalBread(int cnt = 1) {
		name = L"Sacramental Bread";
		lore = L"It shines with a bright light";
		colord = YELLOW;
		symbol = L"☼";
		stackable = true;
		cost = 7777;
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
