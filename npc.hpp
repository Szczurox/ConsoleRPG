#ifndef NPCC
#define NPCC

#include<vector>
#include<fstream>
#include<map>
#include<memory>
#include<functional>

#include"const.hpp"
#include"menu.hpp"

class Item;
class Player;
class ItemFactory;

class SoldInfo {
public:
	std::wstring name = L"";
	unsigned char color = 0;
	int cost = 0;
	int res = 0;

	SoldInfo(std::wstring name, unsigned char color, int cost, int res) : name(name), color(color), cost(cost), res(res) {};
	SoldInfo() {};
};

class NPC {
public:
	std::wstring  name = L"NPC";
	const wchar_t* symbol = L"☻";
	unsigned char nameColor = BRIGHT_BLUE;
	unsigned char colord = BRIGHT_BLUE;
	int npcMemory = 0;
	std::vector<std::shared_ptr<Item>> inv = std::vector<std::shared_ptr<Item>>();

	virtual std::function<void()> interacted(Player* p) { return [this]() { writeMessage(-1, -1); }; }
	virtual void writeMessage(int choice, int res) {};

	NPC() {};

	// Class name of the NPC
	virtual std::string getType();

	void save(std::ostream& os);

	void load(std::ifstream& in, ItemFactory& iFactory);
};


class NPCFactory {
public:
	std::map<std::string, std::function<std::shared_ptr<NPC>()>> NPCMap;

	NPCFactory() {};

	std::shared_ptr<NPC> createNPC(const std::string& type) {
		return NPCMap[type]();
	}

	template <class T>
	void  registerNPC() {
		std::string type = typeid(T).name();
		if (type.rfind("class ", 0) == 0)
			type = type.substr(6);
		NPCMap[type] = []() -> std::shared_ptr<NPC> {
			return std::make_shared<T>();
		};
	}
};


class Shop : public NPC {
public:
	Shop() {
		name = L"Shopkeeper";
		symbol = L"☻";
		nameColor = YELLOW;
		colord = YELLOW;
	}

	Shop(int floor);

	Shop(std::vector<std::shared_ptr<Item>> inventory) {
		inv = inventory;
		name = L"Shopkeeper";
		symbol = L"☻";
		nameColor = YELLOW;
		colord = YELLOW;
	}

	virtual std::function<void()> interacted(Player* p);
	virtual void writeMessage(int choice, int res);
};

class DemonShop : public NPC {
public:
	DemonShop(int floor);

	DemonShop() {
		name = L"Demon";
		symbol = L"☻";
		nameColor = RED;
		colord = GREY;
	}

	DemonShop(std::vector<std::shared_ptr<Item>> inventory) {
		inv = inventory;
		name = L"Demon";
		symbol = L"☻";
		nameColor = RED;
		colord = GREY;
	}

	virtual std::function<void()> interacted(Player* p);
	virtual void writeMessage(int choice, int res);
};

class Beggar : public NPC {
public:
	Beggar() {
		name = L"Beggar";
		symbol = L"☻";
		nameColor = WHITE;
		colord = WHITE;
	}

	Beggar(int floor);

	virtual std::function<void()> interacted(Player* p);
	virtual void writeMessage(int choice, int res);
};

class Smith : public NPC {
public:
	Smith() {
		name = L"Smith";
		symbol = L"☻";
		nameColor = BRIGHT_WHITE;
		colord = BRIGHT_WHITE;
	}

	virtual std::function<void()> interacted(Player* p);
	virtual void writeMessage(int choice, int res);
};

#endif // !NPCC
