#ifndef NPCC
#define NPCC

struct SoldInfo {
	const wchar_t* name;
	unsigned char color;
	int cost;
	int res;

	SoldInfo(const wchar_t* name, unsigned char color, int cost, int res) : name(name), color(color), cost(cost), res(res) {};
	SoldInfo() {};
};

class NPC {
public:
	const wchar_t* name = L"NPC";
	const wchar_t* symbol = L"☻";
	unsigned char nameColor = BRIGHT_BLUE;
	unsigned char colord = BRIGHT_BLUE;
	std::vector<std::shared_ptr<Item>> inv;

	virtual SoldInfo interacted(Player* p) { return SoldInfo(); }

	NPC() {};
};

class Shop : public NPC {
public:
	Shop(std::vector<std::shared_ptr<Item>> inventory) {
		inv = inventory;
		name = L"Shopkeeper";
		symbol = L"☻";
		nameColor = YELLOW;
		colord = YELLOW;
	}

	virtual SoldInfo interacted(Player* p) {
		MenuItem nameMenu(name, nameColor);
		MenuItem message(L"Hello there! What can I get for you?", WHITE);
		std::vector<MenuItem> texts({ nameMenu, message });
		std::vector<MenuItem> options;
		std::vector<Item> trueItems;
		wchar_t s[33][128];
		int count = 0;
		for (std::shared_ptr<Item> it : inv) {
			if (it->count > 0) {
				count++;
				wsprintf(s[count], L"%d gold", it->cost);
				wsprintf(s[count], L"%s x %d : %s", color(it->name.c_str(), it->colord).c_str(), it->count, color(s[count], YELLOW).c_str());
				MenuItem itemMenu(2, s[count]);
				options.push_back(itemMenu);
			}
		}
		options.push_back(MenuItem(L"Back", WHITE));

		Menu menu(&options, &texts, true);

		int choice = -3;
		while (choice == -3) {
			choice = menu.open();
			if (choice >= 0 && choice < options.size() - 1) {
				const wchar_t* name = inv[choice]->name.c_str();
				unsigned char col = inv[choice]->colord;
				int cost = inv[choice]->cost;
				if (cost <= p->gold) {
					p->gold -= cost;
					p->addItem(inv[choice]);
					inv.erase(inv.begin() + choice);
					return SoldInfo(name, col, cost, 1);
				}
				else
					return SoldInfo(name, col, cost, -2);
			}
			return SoldInfo(name, colord, 0, -1);
		}
	}
};

#endif // !NPCC
