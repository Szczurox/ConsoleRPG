#include<fstream>
#include<sstream>

#include"npc.hpp"
#include"effective.hpp"
#include"items.hpp"
#include"player.hpp"
#include"menu.hpp"
#include"utils.hpp"

// Class name of the NPC
std::string NPC::getType() {
	std::string typeName = typeid(*this).name();
	if (typeName.rfind("class ", 0) == 0)
		typeName = typeName.substr(6);
	return typeName;
}

void NPC::save(std::ostream& os) {
	os << getType() << "\n";
	for (std::shared_ptr<Item> it : inv)
		it->save(os);
	os << "EndNPC\n";
}

void NPC::load(std::ifstream& in, ItemFactory& iFactory) {
	std::string line;

	while (std::getline(in, line) && line != "EndNPC") {
		std::istringstream iss(line);
		std::string itemType;
		iss >> itemType;
		std::shared_ptr<Item> item = iFactory.createItem(itemType);
		item->load(iss);
		inv.push_back(item);
	}
}

std::shared_ptr<SoldInfo> Shop::interacted(Player* p) {
	std::shared_ptr<MenuItem> nameMenu = createMenuItem(name, nameColor);
	std::shared_ptr<MenuItem> message = createMenuItem(L"Hello there! What can I get for you?", WHITE);
	std::vector<std::shared_ptr<MenuItem>> texts({ nameMenu, message });
	std::vector<std::shared_ptr<MenuItem>> options;
	std::vector<Item> trueItems;
	int count = 0;
	for (std::shared_ptr<Item> it : inv) {
		if (it->count > 0) {
			count++;
			std::wstringstream s;
			s << color(it->name, it->colord) << " × " << it->count << " : " << color(std::to_wstring(it->cost), YELLOW);
			std::shared_ptr<MenuItem> itemMenu = createMenuItem(2, s.str());
			options.push_back(itemMenu);
		}
	}
	options.push_back(createMenuItem(L"Back", WHITE));

	Menu menu(options, texts, true);

	int choice = -3;
	while (choice == -3) {
		choice = menu.open();
		if (choice >= 0 && choice < options.size() - 1) {
			std::wstring name = inv[choice]->name;
			unsigned char col = inv[choice]->colord;
			int cost = inv[choice]->cost;
			if (cost <= p->gold) {
				p->gold -= cost;
				p->addItem(inv[choice]);
				inv.erase(inv.begin() + choice);
				return  std::shared_ptr<SoldInfo>(new SoldInfo(name, col, cost, 1));
			}
			else
				return std::shared_ptr<SoldInfo>(new SoldInfo(name, col, cost, -2));
		}
		return std::shared_ptr<SoldInfo>(new SoldInfo(name, 0, 0, -1));
	}
}
