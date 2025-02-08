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

Shop::Shop(int floor) {
	name = L"Shopkeeper";
	symbol = L"☻";
	nameColor = YELLOW;
	colord = YELLOW;

	for (int i = 0; i < std::min(floor + 3, 12); i++) {
		int rand = randMinMax(0, 1000);
		if (rand > 950)
			inv.push_back(std::shared_ptr<Item>(new IronShortsword()));
		if (rand > 900)
			inv.push_back(std::shared_ptr<Item>(new WoodenSword()));
		else if (rand > 800)
			inv.push_back(std::shared_ptr<Item>(new Gambeson()));
		else
			inv.push_back(std::shared_ptr<Item>(new HealthPotion()));
	}
	inv.push_back(std::shared_ptr<Item>(new HealthPotion()));
}

void Shop::writeMessage(int choice, int res) {
	if (res >= 0) {
		write(L"Bought ");
		write(color(inv[choice]->name, inv[choice]->colord).c_str());
		write(L" for ");
		write(color(L"% gold", YELLOW).c_str(), inv[choice]->cost);
		inv.erase(inv.begin() + choice);
	}
	else if (res == -2)
		write(L"You don't have enough % to buy %!", color(L"gold", YELLOW), color(inv[choice]->name, inv[choice]->colord));
}

std::function<void()> Shop::interacted(Player* p) {
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
				return  [this, choice]() {  writeMessage(choice, 1); };
			}
			else
				return [this, choice]() {  writeMessage( choice, -2); };
		}
		return [this, choice]() {  writeMessage(choice, -1); };
	}

	return [this, choice]() {  writeMessage(choice, -1); };
}

DemonShop::DemonShop(int floor) {
	name = L"Demon";
	symbol = L"☻";
	nameColor = RED;
	colord = GREY;

	for (int i = 0; i < std::min(floor + 1, 6); i++) {
		int rand = randMinMax(0, 100);
		if (rand > 900)
			inv.push_back(std::shared_ptr<Item>(new IronShortsword()));
		else if (rand > 700)
			inv.push_back(std::shared_ptr<Item>(new BoneArmor()));
		else
			inv.push_back(std::shared_ptr<Item>(new BloodOath()));
	}
	inv.push_back(std::shared_ptr<Item>(new HealthPotion()));
}

void DemonShop::writeMessage(int choice, int res) {
	if (res >= 0) {
		write(L"Sacrificed ");
		write(color(L"% health", RED).c_str(), inv[choice]->cost);
		write(L" for ");
		write(color(inv[choice]->name, inv[choice]->colord).c_str());
		inv.erase(inv.begin() + choice);
	}
	else if (res == -2)
		write(L"You don't have enough % to get %!", color(L"health", RED), color(inv[choice]->name, inv[choice]->colord));
}

std::function<void()> DemonShop::interacted(Player* p) {
	std::shared_ptr<MenuItem> nameMenu = createMenuItem(name, nameColor);
	std::shared_ptr<MenuItem> message = createMenuItem(L"Welcome! : )", WHITE);
	std::vector<std::shared_ptr<MenuItem>> texts({ nameMenu, message });
	std::vector<std::shared_ptr<MenuItem>> options;
	std::vector<Item> trueItems;
	int count = 0;
	for (std::shared_ptr<Item> it : inv) {
		if (it->count > 0) {
			count++;
			std::wstringstream s;
			s << color(it->name, it->colord) << " × " << it->count << " : " << color(std::to_wstring(it->cost / 200), RED);
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
			int cost = inv[choice]->cost / 200;
			if (cost < p->maxHealth) {
				p->maxHealth -= cost;
				if (p->health > p->maxHealth)
					p->health = p->maxHealth;
				p->faith--;
				p->addItem(inv[choice]);
				return  [this, choice]() {  writeMessage(choice, 1); };
			}
			else
				return [this, choice]() {  writeMessage(choice, -2); };
		}
		return [this, choice]() {  writeMessage(choice, -1); };
	}

	return [this, choice]() {  writeMessage(choice, -1); };
}