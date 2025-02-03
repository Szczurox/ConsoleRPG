#ifndef CRAFT
#define CRAFT

#include<memory>
#include<vector>
#include<fstream>
#include<sstream>
#include<map>

#include"menu.hpp"

class Player;
class Item;

class Recipe {
public:
    std::shared_ptr<Item> item = nullptr;
    std::vector<std::shared_ptr<Item>> items = std::vector<std::shared_ptr<Item>>();
    bool unlocked = false;
    Recipe(std::vector<std::shared_ptr<Item>> items) : items(items) {};
    Recipe() {};
    virtual std::shared_ptr<Item> getItem();
    virtual std::shared_ptr<Item> craft(std::map<std::wstring, std::shared_ptr<Item>>& inv, int& curID, int& invTaken);
    virtual std::string getType() const;
    virtual void save(std::ostream& os);
    virtual void load(std::istringstream& in);
};

// Bone Armor recipe
class BARecipe : public Recipe {
public:
	virtual std::shared_ptr<Item> getItem();

	BARecipe(bool unl = false);
};

void pushRecipies(Player& p);

#endif // !CRAFT