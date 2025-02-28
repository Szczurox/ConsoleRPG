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

std::wstring getDurString(int dur, int maxDur);

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
	int speed = 0;
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
	virtual void special(Player* player, int dmg) {};
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

// Ranged class
class Ranged : public Item {
public:
	Ranged() {};

	int used(Player* player);
	void onRemove(Player* p);
	void writeMessage() {};
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
		speed = 0;
		reqLevel = 0;
		durability = dur;
		maxDurability = 100;
		cost = 1000;
	}
};

class IronShortsword : public Weapon {
public:
	IronShortsword(int dur = 250) {
		name = L"Iron Shortsword";
		lore = L"Quick weapon, but not very accurate";
		colord = BRIGHT_BLUE;
		symbol = L"┼";
		minDmg = 0;
		maxDmg = 8;
		speed = 1;
		reqLevel = 2;
		durability = dur;
		maxDurability = 250;
		cost = 4000;
	}
};

class BloodyBlade : public Weapon {
public:
	BloodyBlade(int dur = 666) {
		name = L"Bloody Blade";
		lore = L"Damage comes at a cost";
		colord = RED;
		symbol = L"┼";
		minDmg = 0;
		maxDmg = 6;
		speed = 6;
		reqLevel = 3;
		durability = dur;
		maxDurability = 666;
		cost = 6000;
	}

	virtual void special(Player* player, int dmg);
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
	BoneArmor(int dur = 125) {
		name = L"Bone Armor";
		lore = L"Armor made out of bones";
		colord = GREY;
		symbol = L"O";
		prot = 3;
		reqLevel = 0;
		durability = dur;
		maxDurability = 125;
		cost = 1000;
	}
};

class MageRobes : public Armor {
public:
	MageRobes(int dur = 500) {
		name = L"Mage Robes";
		lore = L"Be blessed with wisdom";
		colord = YELLOW;
		symbol = L"π";
		prot = 1;
		reqLevel = 1;
		durability = dur;
		maxDurability = 500;
		cost = 2000;
	}

	virtual void special(Player* player, int dmg);
};

class CeremonialRobes : public Armor {
public:
	CeremonialRobes(int dur = 666) {
		name = L"Ceremonial Robes";
		lore = L"Sacrifice gives power";
		colord = RED;
		symbol = L"O";
		prot = 0;
		reqLevel = 1;
		durability = dur;
		maxDurability = 666;
		cost = 3000;
	}

	virtual void special(Player* player, int dmg);
};

class Chasuble : public Armor {
public:
	Chasuble(int dur = 333) {
		name = L"Chasuble";
		lore = L"May your faith protect you";
		colord = YELLOW;
		symbol = L"O";
		prot = 3;
		reqLevel = 3;
		durability = dur;
		maxDurability = 333;
		cost = 7777;
	}

	virtual void special(Player* player, int dmg);
	int used(Player* player);
	void onRemove(Player* p);

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
	ZombieMeat(int cnt = 1) {
		name = L"Zombie Meat";
		lore = L"The odour is unbearable, but it looks somewhat edible";
		colord = GREEN;
		symbol = L"▬";
		stackable = true;
		cost = 200;
		count = cnt;
	}

	virtual int used(Player* p);

	virtual void writeMessage();
};

class BloodOath : public Usable {
public:
	BloodOath(int cnt = 1) {
		name = L"Blood Oath";
		lore = L"You feel dark energy emanating from it";
		colord = RED;
		symbol = L"Ω";
		stackable = true;
		cost = 666;
		count = cnt;
	}

	virtual int used(Player* p);

	virtual void writeMessage();
};

class SacramentalBread : public Usable {
public:
	SacramentalBread(int cnt = 1) {
		name = L"Sacramental Bread";
		lore = L"It shines with a bright light";
		colord = YELLOW;
		symbol = L"☼";
		stackable = true;
		cost = 777;
		count = cnt;
	}

	virtual int used(Player* p);

	virtual void writeMessage();
};

// Ranged
class WandOfLightning : public Ranged {
public:
	WandOfLightning(int dur = 80) {
		name = L"Wand of Lightning";
		lore = L"Zap your enemies";
		colord = YELLOW;
		symbol = L"/";
		stackable = false;
		cost = 3000;
		durability = dur;
		maxDurability = 80;
		maxDmg = 6;
		minDmg = 4;
	}

	virtual void writeMessage();
};

class VampiricWand : public Ranged {
public:
	VampiricWand(int dur = 100) {
		name = L" Vampiric Wand";
		lore = L"Steal your enemies' blood";
		colord = RED;
		symbol = L"/";
		stackable = false;
		cost = 10000;
		durability = dur;
		maxDurability = 66;
		maxDmg = 8;
		minDmg = 6;
		reqLevel = 3;
	}

	virtual void writeMessage();
	virtual void special(Player* player, int dmg);
};

class Shuriken : public Ranged {
public:
	Shuriken(int cnt = 1) {
		name = L"Shuriken";
		lore = L"Not easy to master";
		colord = GREY;
		symbol = L"•";
		stackable = true;
		cost = 20;
		count = cnt;
		maxDmg = 8;
		minDmg = 0;
	}

	virtual void writeMessage();
};

class Dart : public Ranged {
public:
	Dart(int cnt = 1) {
		name = L"Dart";
		lore = L"Very accurate but not so powerful";
		colord = GREY;
		symbol = L"·";
		stackable = true;
		cost = 10;
		count = cnt;
		maxDmg = 3;
		minDmg = 3;
	}

	virtual void writeMessage();
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

class Key : public Resource {
public:
	Key(int floor = 0, int room = 0) {
		stackable = false;
		name = L"Key";
		lore = L"Found on the floor " + std::to_wstring(floor);
		colord = GREY;
		symbol = L"⌐";
		ID = room;
		// Using count to store floor
		count = floor;
		maxDurability = 1;
		durability = 1;
	}
};


#endif // !ITEMS
