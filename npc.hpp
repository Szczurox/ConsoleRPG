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
	std::vector<std::shared_ptr<Item>> inv = std::vector<std::shared_ptr<Item>>();

	virtual std::shared_ptr<SoldInfo> interacted(Player* p) { return std::shared_ptr<SoldInfo>(new SoldInfo()); }

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

	Shop(std::vector<std::shared_ptr<Item>> inventory) {
		inv = inventory;
		name = L"Shopkeeper";
		symbol = L"☻";
		nameColor = YELLOW;
		colord = YELLOW;
	}

	virtual std::shared_ptr<SoldInfo> interacted(Player* p);
};

#endif // !NPCC
