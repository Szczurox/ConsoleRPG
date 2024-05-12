#ifndef NPCC
#define NPCC

class NPC {
public:
	const wchar_t* name = L"NPC";
	const wchar_t* symbol = L"☻";
	unsigned char nameColor = BRIGHT_BLUE;
	unsigned char colord = BRIGHT_BLUE;
	std::vector<std::shared_ptr<Item>> inv;

	virtual std::pair<std::shared_ptr<Item>, int> interacted(Player* p) { return std::pair<std::shared_ptr<Item>, bool>(); }

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

	virtual std::pair<std::shared_ptr<Item>, int> interacted(Player* p) {
		MenuItem nameMenu(name, nameColor);
		MenuItem message(L"Hello there! What can I get for you?", WHITE);
		std::vector<MenuItem> texts({ nameMenu, message });
		std::vector<MenuItem> options;
		wchar_t s[33][128];
		int count = 0;
		for (std::shared_ptr<Item> it : inv) {
			count++;
			wsprintf(s[count], L"%d gold", it->cost * 3);
			wsprintf(s[count], L"%s : %s", color(it->name, it->colord).c_str(), color(s[count], YELLOW).c_str());
			MenuItem itemMenu(2, s[count]);
			options.push_back(itemMenu);
		}
		options.push_back(MenuItem(L"Back", WHITE));

		Menu menu(&options, &texts, true);

		int choice = -3;
		while (choice == -3) {
			choice = menu.open();
			if (choice >= 0 && choice < options.size() - 1) {
				if (inv[choice]->cost * 3 <= p->gold) {
					p->gold -= inv[choice]->cost * 3;
					p->addItem(inv[choice]);
					return std::make_pair(inv[choice], 1);
				}
				else
					return std::make_pair(inv[choice], -2);
			}
			return std::make_pair(inv[0], -1);
		}
	}
};

#endif // !NPCC
