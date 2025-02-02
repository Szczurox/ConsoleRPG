#include <ostream>
#include <sstream>

#include"effective.hpp"
#include"menu.hpp"
#include"items.hpp"

std::shared_ptr<Item> Recipe::getItem() {
    return std::shared_ptr<Item>(new Item);
}

std::shared_ptr<Item> Recipe::craft(std::map<std::wstring, std::shared_ptr<Item>>& inv, int& curID, int& invTaken) {
    // Crafting logic
    return std::shared_ptr<Item>(new Item());
}

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

void Buff::tick() {
    duration--;
}

void Buff::save(std::ostream& os) {
    os << static_cast<int>(type) << " " << amount << " " << duration << " " << isMultiplier << "\n";
}

void Buff::load(std::istringstream& in) {
    int intType;
    in >> intType >> amount >> duration >> isMultiplier;
    type = static_cast<BuffType>(intType);
}
