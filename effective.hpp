#ifndef EFFECTIVE
#define EFFECTIVE

class Player;

enum class ItemType {
	RESOURCE = 0,
	WEAPON = 1,
	ARMOR = 2,
	USABLE = 3
};


enum class BuffType {
	DMG = 0,
	SPD = 1,
	PROT = 2,
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
	Item(const wchar_t* name, ItemType type) : name(name), type(type) {}
	// 1 - Add to inventory
	virtual int picked(Player* player) { return 1; }
	virtual int used(Player* player) { return 0; }
	virtual void writeMessage() { return; };
	virtual std::pair<int, std::function<void()>> menuHandle(Player* p, std::vector<MenuItem>& options, std::vector<MenuItem>& texts) {
		OutputDebugStringA(getType().c_str());
		Menu menu(&options, &texts, true);
		int choice = -2;
		while (choice == -2) {
			choice = menu.open();
			if (choice == 0 && options.size() == 3)
				return { used(p), [this]() { this->writeMessage(); } };
			if ((choice == 1 && options.size() == 3) || (choice == 0 && options.size() == 2)) {
				MenuItem option(L"Are you sure you want to destroy this item?", RED);
				MenuItem no(L"No", WHITE);
				MenuItem yes(L"Yes", WHITE);
				std::vector<MenuItem> options({ no, yes });
				Menu deleteMenu(&options, option);
				int confirmation = deleteMenu.open();
				if (confirmation != 1)
					choice = -2;
				else {
					onRemove(p);
					return { -1, [this]() { this->writeMessage(); } };
				}
			}
		}
		return { 0, [this]() { this->writeMessage(); } };
	}

	virtual std::pair<int, std::function<void()>> itemMenu(Player* p) { return {}; }
	virtual void onRemove(Player* p) { }

	// Class name of the item
	virtual std::string getType() const {
		std::string typeName = typeid(*this).name();
		if (typeName.rfind("class ", 0) == 0)
			typeName = typeName.substr(6);
		return typeName;
	}

	virtual void save(std::ostream& os) {
		os << getType() << " " << ID << " " << count << " " << durability << "\n";
	}

	virtual void load(std::istringstream& in) {
		int temp;
		if(!stackable)
			in >> ID >> count >> durability;
		else {
			in >> temp;
			in >> count >> durability;
		}
	}
};


class ItemFactory {
public:
	std::map<std::string, std::function<std::shared_ptr<Item>()>> itemMap;

	ItemFactory() {};

	std::shared_ptr<Item> createItem(const std::string& type) {
		return itemMap[type]();
	}

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


class Recipe {
public:
	std::shared_ptr<Item> item = std::shared_ptr<Item>(new Item());
	std::vector<std::shared_ptr<Item>> items;
	bool unlocked = false;
	Recipe(std::vector<std::shared_ptr<Item>> items) : items(items) {};
	Recipe() {};

	virtual std::shared_ptr<Item> getItem() {
		return std::shared_ptr<Item>(new Item);
	}

	virtual std::shared_ptr<Item> craft(std::map<std::wstring, std::shared_ptr<Item>>& inv, int& curID, int& invTaken) {
		std::vector<MenuItem> texts = std::vector<MenuItem>();
		std::vector<MenuItem> options = std::vector<MenuItem>();
		texts.push_back(MenuItem(item->name.c_str(), item->colord));
		texts.push_back(MenuItem(item->lore.c_str(), WHITE));
		// Max 10 different ingredients
		wchar_t s[10][128];
		int check = 0;
		int count = 0;
		for (auto req : items) {
			wsprintf(s[count], L"(0/%d)", req->count);
			wsprintf(s[count], L"%s %s", color(req->name.c_str(), req->colord).c_str(), color(s[count], RED).c_str());
			texts.push_back(MenuItem(2, s[count]));
			for (auto item : inv) {
				std::shared_ptr<Item> it = item.second;
				std::wstring nameReq = it->ID == 0 ? req->name : req->name + std::to_wstring(it->ID);
				std::wstring nameIt = it->ID == 0 ? it->name : it->name + std::to_wstring(it->ID);
				if (nameReq == nameIt) {
					bool enough = it->count >= req->count;
					wsprintf(s[count], L"(%d/%d)", it->count, req->count);
					wsprintf(s[count], L"%s %s", color(req->name.c_str(), req->colord).c_str(), color(s[count], enough ? GREEN : RED).c_str());
					count++;
					if (enough)
						check++;
					break;
				}
			}
		}

		if (check >= items.size())
			options.push_back(MenuItem(L"Craft", GREEN));
		else
			texts.push_back(MenuItem(L"Insufficient resources", RED));

		options.push_back(MenuItem(L"Back", WHITE));

		Menu recipesMenu(&options, &texts);
		int choice = recipesMenu.open();

		if (check >= items.size() && choice == 0) {
			for (auto req : items)
				for (auto item : inv) {
					std::shared_ptr<Item> it = item.second;
					std::wstring nameReq = it->ID == 0 ? req->name : req->name + std::to_wstring(it->ID);
					std::wstring nameIt = it->ID == 0 ? it->name : it->name + std::to_wstring(it->ID);
					if (nameReq == nameIt && it->count >= req->count)
						inv[nameIt]->count -= req->count;
				}

			return getItem();
		}

		return std::shared_ptr<Item>(new Item());
	};

	virtual std::string getType() const {
		std::string typeName = typeid(*this).name();
		if (typeName.rfind("class ", 0) == 0)
			typeName = typeName.substr(6);
		return typeName;
	}

	virtual void save(std::ostream& os) {
		os << getType() << " " << unlocked << "\n";
	}

	virtual void load(std::istringstream& in) {
		in >> unlocked;
	}
};


class Buff {
public:
	BuffType type;
	int amount;
	int duration;
	bool isMultiplier;
	bool isBuffing = false;

	Buff() {};
	Buff(BuffType type, float amount, int duration, int isMultiplier = false) : type(type), amount(amount), duration(duration), isMultiplier(isMultiplier) {};
	void tick() { duration--; };

	virtual void save(std::ostream& os) {
		os << static_cast<int>(type) << " " << amount << " " << duration << " " << isMultiplier << "\n";
	}

	virtual void load(std::istringstream& in) {
		int intType;
		in >> intType >> amount >> duration >> isMultiplier;
		type = static_cast<BuffType>(intType);
	}
};


#endif // !EFFECTIVE
