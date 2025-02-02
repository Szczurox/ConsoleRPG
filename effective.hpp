#ifndef EFFECTIVE
#define EFFECTIVE

#include<functional>
#include<string>
#include<vector>
#include<memory>
#include<map>

#include"const.hpp"

class Player;
class Item;

enum class BuffType {
	DMG = 0, 
	SPD = 1,
	PROT = 2,
};

class Recipe {
public:
    std::shared_ptr<Item> item =  nullptr;
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

class Buff {
public:
    BuffType type = BuffType::DMG;
    int amount = 1;
    int duration = 0;
    bool isMultiplier = false;
    bool isBuffing = false;

    Buff() {}
    Buff(BuffType type, float amount, int duration, int isMultiplier) : type(type), amount(amount), duration(duration), isMultiplier(isMultiplier) {}

    // Tick down buff duration by 1 turn
    void tick();

    virtual void save(std::ostream& os);

    virtual void load(std::istringstream& in);
};

#endif // EFFECTIVE
