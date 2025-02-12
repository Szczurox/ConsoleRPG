#include<sstream>
#include<vector>
#include<memory>

#include"crafting.hpp"
#include"player.hpp"
#include"items.hpp"
#include"utils.hpp"

std::shared_ptr<Item> Recipe::getItem() {
    return std::shared_ptr<Item>(new Item);
}

std::shared_ptr<Item> Recipe::craft(std::map<std::wstring, std::shared_ptr<Item>>& inv, int& curID, int& invTaken) {
	std::vector<std::shared_ptr<MenuItem>> texts = {};
	std::vector<std::shared_ptr<MenuItem>> options = {};
	texts.push_back(createMenuItem(item->name.c_str(), item->colord));
	texts.push_back(createMenuItem(item->lore.c_str(), WHITE));
	int check = 0;
	int count = 0;
	int cur = 2;
	for (std::shared_ptr<Item> req : items) {
		std::wstringstream s;
		s << L" (0/" << req->count << L")";
		std::wstring d = color(req->name, req->colord) + color(s.str(), RED);
		texts.push_back(createMenuItem(2, d));
		for (std::pair<std::wstring, std::shared_ptr<Item>> item : inv) {
			std::shared_ptr<Item> it = item.second;
			std::wstring nameReq = it->ID == 0 ? req->name : req->name + std::to_wstring(it->ID);
			std::wstring nameIt = it->ID == 0 ? it->name : it->name + std::to_wstring(it->ID);
			if (nameReq == nameIt) {
				bool enough = it->count >= req->count;
				std::wstringstream s;
				s << L" (" << it->count << L"/" << req->count << L")";
				std::wstring d = color(req->name, req->colord) + color(s.str(), enough ? GREEN : RED);
				texts[cur]->texts[0] = d;
				count++;
				if (enough)
					check++;
				break;
			}
		}
		cur++;
	}
	if (check >= items.size())
		options.push_back(createMenuItem(L"Craft", GREEN));
	else
		texts.push_back(createMenuItem(L"Insufficient resources", RED));
	options.push_back(createMenuItem(L"Back", WHITE));
	Menu recipesMenu(options, texts);
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


std::string Recipe::getType() const {
    std::string typeName = typeid(*this).name();
    if (typeName.rfind("class ", 0) == 0)
        typeName = typeName.substr(6);
    return typeName;
}

void Recipe::save(std::ostream& os) {
    os << getType() << " " << unlocked << "\n";
}

void Recipe::load(std::istringstream& in) {
    in >> unlocked;
}

BARecipe::BARecipe(bool unl) {
    unlocked = unl;
    item = std::shared_ptr<Item>(new BoneArmor());
    items = std::vector<std::shared_ptr<Item>>();
    items.push_back(std::shared_ptr<Item>(new Bone(30)));
}

std::shared_ptr<Item> BARecipe::getItem() {
    return std::shared_ptr<Item>(new BoneArmor());
}

// Add all of the recipes to the Player's recipes
void pushRecipies(Player& p) {
    p.addRecipe<BARecipe>(true);
}
