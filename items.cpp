#include"items.hpp"
#include"player.hpp"
#include"menu.hpp"
#include"utils.hpp"
#include"effective.hpp"

#include<cwchar>
#include<functional>
#include<windows.h>
#include<sstream>

std::shared_ptr<Item> ItemFactory::createItem(const std::string& type) {
    return itemMap[type]();
}

std::pair<int, std::function<void()>> Item::menuHandle(Player* p, std::vector<std::shared_ptr<MenuItem>>& options, std::vector<std::shared_ptr<MenuItem>>& texts) {
    Menu menu(options, texts, true);
    int choice = -2;
    while (choice == -2) {
        choice = menu.open();
        if (choice == 0 && options.size() == 3)
            return { used(p), [this]() { this->writeMessage(); } };
        if ((choice == 1 && options.size() == 3) || (choice == 0 && options.size() == 2)) {
            std::shared_ptr<MenuItem> option = createMenuItem(L"Are you sure you want to destroy this item?", RED);
            std::shared_ptr<MenuItem> no = createMenuItem(L"No", WHITE);
            std::shared_ptr<MenuItem> yes = createMenuItem(L"Yes", WHITE);
            std::vector<std::shared_ptr<MenuItem>> options({ no, yes });
            Menu deleteMenu(options, option);
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

std::string Item::getType() const {
    std::string typeName = typeid(*this).name();
    if (typeName.rfind("class ", 0) == 0)
        typeName = typeName.substr(6);
    return typeName;
}

void Item::save(std::ostream& os) {
    os << getType() << " " << count << " " << durability << "\n";
}

void Item::load(std::istringstream& in) {
    in >> count >> durability;
}

int Weapon::used(Player* player) {
    if (player->weapon.get() != this) {
        player->minDamage = player->baseDamage + minDmg;
        player->maxDamage = player->baseDamage + maxDmg;
        return 1;
    }
    else {
        player->minDamage = player->baseDamage;
        player->maxDamage = player->baseDamage;
        return 3;
    }
}

void Weapon::onRemove(Player* p) {
    if (p->weapon.get() == this) {
        p->weapon = nullptr;    
        p->minDamage = p->baseDamage;
        p->maxDamage = p->baseDamage;
    }
    p->removeItem(name, 1, ID);
}

std::wstring getDurString(int durability, int maxDurability) {
    char durColor = RED;
    if (durability * 100 / maxDurability > 30)
        durColor = YELLOW;
    if (durability * 100 / maxDurability > 60)
        durColor = GREEN;
    return color(L"Durability: ", BRIGHT_BLUE).c_str() + color(std::to_wstring(durability) + L"/" + std::to_wstring(maxDurability), durColor) + L" ";
}

std::pair<int, std::function<void()>> Weapon::itemMenu(Player* p) {
    std::vector<std::shared_ptr<MenuItem>> options;

    std::shared_ptr<MenuItem> nameI = createMenuItem(name, colord);
    std::shared_ptr<MenuItem> loreI = createMenuItem(lore, WHITE);
    std::wstringstream s;
    s << L"Deals " << minDmg << L"-" << maxDmg << L" damage and has " << speed << L" speed";
    std::shared_ptr<MenuItem> damage = createMenuItem(s.str(), YELLOW);
    std::shared_ptr<MenuItem> dur = createMenuItem(2, getDurString(durability, maxDurability));
    std::vector<std::shared_ptr<MenuItem>> texts({ nameI, loreI, damage, dur });

    if (p->level < reqLevel) {
        std::wstringstream e;
        e << L"Required Level: " << reqLevel;
        texts.push_back(createMenuItem(e.str(), RED));
    }
    if (p->level >= reqLevel) {
        std::shared_ptr<MenuItem> equip;
        if (p->weapon.get() != this)
            equip = createMenuItem(L"Equip", BRIGHT_GREEN);
        else
            equip = createMenuItem(L"Unequip", BRIGHT_GREEN);
        options.push_back(equip);
    }
    options.push_back(createMenuItem(L"Destroy", RED));
    options.push_back(createMenuItem(L"Back", WHITE));

    return menuHandle(p, options, texts);
}

int Armor::used(Player* player) {
    if (player->armor.get() != this) {
        player->defence = prot;
        return 2;
    }
    else {
        player->defence = 0;
        return 4;
    }
}

void Armor::onRemove(Player* p) {
    if (p->armor.get() == this) {
        p->armor = nullptr;
        p->defence = 0;
    }
    p->removeItem(name, 1, ID);
}

std::pair<int, std::function<void()>> Armor::itemMenu(Player* p) {
    std::vector<std::shared_ptr<MenuItem>> options;

    std::shared_ptr<MenuItem> nameI = createMenuItem(name, colord);
    std::shared_ptr<MenuItem> loreI = createMenuItem(lore, WHITE);
    std::wstringstream s;
    s << L"Gives you " << prot << " defence";
    std::shared_ptr<MenuItem> prot = createMenuItem(s.str(), YELLOW);
    std::shared_ptr<MenuItem> dur = createMenuItem(2, getDurString(durability, maxDurability));
    std::vector<std::shared_ptr<MenuItem>> texts({ nameI, loreI, prot, dur });
    if (p->level < reqLevel) {
        std::wstringstream e;
        e << L"Required Level: " << reqLevel;
        texts.push_back(createMenuItem(e.str(), RED));
    }

    if (p->level >= reqLevel) {
        std::shared_ptr<MenuItem> equip;
        if (p->armor.get() != this)
            equip = createMenuItem(L"Equip", BRIGHT_GREEN);
        else
            equip = createMenuItem(L"Unequip", BRIGHT_GREEN);
        options.push_back(equip);
    }
    options.push_back(createMenuItem(L"Destroy", RED));
    options.push_back(createMenuItem(L"Back", WHITE));

    return menuHandle(p, options, texts);
}

int Usable::used(Player* player) {
    return 5;
}

void Usable::onRemove(Player* p) {
    p->removeItem(name, count);
}

void Usable::writeMessage() {
    write(L"used!");
}

std::pair<int, std::function<void()>> Usable::itemMenu(Player* p) {
    std::shared_ptr<MenuItem> nameI = createMenuItem(name.c_str(), colord);
    std::shared_ptr<MenuItem> loreI = createMenuItem(lore.c_str(), WHITE);
    std::shared_ptr<MenuItem> coun = createMenuItem(L"Amount: " + std::to_wstring(count), BRIGHT_BLUE);
    std::vector<std::shared_ptr<MenuItem>> texts({ nameI, loreI, coun });

    std::shared_ptr<MenuItem> use = createMenuItem(L"Use", BRIGHT_GREEN);
    std::shared_ptr<MenuItem> remove = createMenuItem(L"Destroy", RED);
    std::shared_ptr<MenuItem> back = createMenuItem(L"Back", WHITE);
    std::vector<std::shared_ptr<MenuItem>> options({ use, remove, back });

    return menuHandle(p, options, texts);
}

int Ranged::used(Player* player) {
    return 6;
}

void Ranged::onRemove(Player* p) {
    p->removeItem(name, count);
}

std::pair<int, std::function<void()>> Ranged::itemMenu(Player* p) {
    std::vector<std::shared_ptr<MenuItem>> options;

    std::shared_ptr<MenuItem> nameI = createMenuItem(name, colord);
    std::shared_ptr<MenuItem> loreI = createMenuItem(lore, WHITE);
    std::wstringstream s;
    s << L"Deals " << minDmg << L"-" << maxDmg << L" damage";
    std::shared_ptr<MenuItem> damage = createMenuItem(s.str(), YELLOW);  
    std::shared_ptr<MenuItem> durCoun;
    if(!stackable)
        durCoun = createMenuItem(2, getDurString(durability, maxDurability));
    else 
        durCoun = createMenuItem(L"Amount: " + std::to_wstring(count), BRIGHT_BLUE);
    std::vector<std::shared_ptr<MenuItem>> texts({ nameI, loreI, damage, durCoun });

    if (p->level < reqLevel) {
        std::wstringstream e;
        e << L"Required Level: " << reqLevel;
        texts.push_back(createMenuItem(e.str(), RED));
    }

    options.push_back(createMenuItem(L"Use", BRIGHT_GREEN));
    options.push_back(createMenuItem(L"Destroy", RED));
    options.push_back(createMenuItem(L"Back", WHITE));

    return menuHandle(p, options, texts);
}

int Resource::used(Player* player) {
    return 1;
}

void Resource::onRemove(Player* p) {
    p->removeItem(name, count);
}

std::pair<int, std::function<void()>> Resource::itemMenu(Player* p) {
    std::shared_ptr<MenuItem> nameI = createMenuItem(name, colord);
    std::shared_ptr<MenuItem> loreI = createMenuItem(lore, WHITE);
    std::shared_ptr<MenuItem> coun = createMenuItem(L"Amount: " + std::to_wstring(count), BRIGHT_BLUE);
    std::vector<std::shared_ptr<MenuItem>> texts({ nameI, loreI, coun });

    std::shared_ptr<MenuItem> remove = createMenuItem(L"Destroy", RED);
    std::shared_ptr<MenuItem> back = createMenuItem(L"Back", WHITE);
    std::vector<std::shared_ptr<MenuItem>> options({ remove, back });

    return menuHandle(p, options, texts);
}

// Tile items
GoldPile::GoldPile(int minGold, int maxGold) {
    type = ItemType::RESOURCE;
    name = L"gold";
    count = randMinMax(minGold, maxGold);
    stackable = false;
}

int GoldPile::picked(Player* player) {
    player->gold += count;
    return 0;
}

// Weapons

// Armor
void CeremonialRobes::special(Player* p, int dmg) {
    if (dmg >= p->health && chance(1, 6) && p->faith > 0)
        p->health = 6 + dmg;
    if (dmg > p->maxHealth / 6 && (chance(1, 6) || (p->faith < -2 && chance(1, 2))))
        p->giveBuff(BuffType::DMG, 6, 6);
    else if (p->health < p->maxHealth / 6)
        p->giveBuff(BuffType::DMG, 4, 1);
    else if (p->health < p->maxHealth / 2)
        p->giveBuff(BuffType::DMG, 2, 1);
}

void Chasuble::special(Player* p, int dmg) {
    if (dmg >= p->health && chance(1, 3) && p->faith >= 0)
        p->health = 33 + dmg;
    if (chance(1, 3))
        p->giveBuff(BuffType::PROT, 1, 3);
    if (p->health < p->maxHealth / 3 && (chance(1, 7) || (p->faith > 2 && chance(1, 3))))
        p->giveBuff(BuffType::REG, 7, 3);
    else if(chance(1, 7) || (p->faith > 2 && chance(1, 3)))
        p->giveBuff(BuffType::REG, 3, 3);
}

int Chasuble::used(Player* player) {
    if (player->armor.get() != this) {
        player->defence = prot;
        player->faith++;
        return 2;
    }
    else {
        player->defence = 0;
        player->faith--;
        return 4;
    }
}

void Chasuble::onRemove(Player* p) {
    if (p->armor.get() == this) {
        p->armor = nullptr;
        p->faith--;
        p->defence = 0;
    }
    p->removeItem(name, 1, ID);
}


// Consumables
// Health Potion
int HealthPotion::used(Player* p) {
    p->health += 100;
    if (p->health > p->maxHealth) p->health = p->maxHealth;
    p->removeItem(name, 1);
    return 5;
}

void HealthPotion::writeMessage() {
    write(L"You drank %.\nHealed for 100HP!", color(name.c_str(), colord).c_str());
}

// Zombie Meat
int ZombieMeat::used(Player* p) {
    int random = randMinMax(0, 50);
    int buff = randMinMax(1, 3);
    if (random >= 80) {
        p->health += random;
        if (p->health > p->maxHealth) p->health = p->maxHealth;
    }
    else if (random < 60)
        p->health -= random;
    p->giveBuff(BuffType::DMG, buff, 50, false);
    p->removeItem(name, 1);
    return 5;
}

void ZombieMeat::writeMessage() {
    write(L"You ate %.\nYou feel strange.", color(name.c_str(), colord).c_str());
}

// Blood Oath
int BloodOath::used(Player* p) {
    int healthChange = randMinMax(10, 30);
    p->health -= healthChange;
    p->maxHealth -= healthChange;
    p->baseDamage += randMinMax(1, 3);
    if (p->faith < -1) {
        p->health -= p->health / 2;
        p->giveBuff(BuffType::DMG, 6, 67);
    }
    p->faith -= 1;
    p->removeItem(name, 1);
    return 5;
}

void BloodOath::writeMessage() {
    write(color(L"There is no going back...", colord).c_str());
}

// Sacramental Bread
int SacramentalBread::used(Player* p) {
    if (p->faith < -5) {
        p->health -= p->health / 2;
        p->faith++;
    }
    else {
        p->maxHealth += 10 + p->faith * 10;
        p->health = p->maxHealth;
    }
    if (p->faith > -1)
        p->giveBuff(BuffType::PROT, 3, 334);
    else
        messageType = 1;
    p->removeItem(name, 1);
    return 5;
}

void SacramentalBread::writeMessage() {
    if (messageType == 0)
        write(color(L"You feel refreshed and protected.", colord).c_str());
    else if (messageType == 2)
        write(L"You ate %.\n%", color(name.c_str(), colord).c_str(), color(L"It hurts...", RED).c_str());
    else
        write(L"%%", color(L"You feel refreshed... ", colord).c_str(), color(L"but not protected.", RED).c_str());
}

void WandOfLightning::writeMessage() {
    write(color(L"Select an enemy to zap", colord).c_str());
}

void Shuriken::writeMessage() {
    write(color(L"Select an enemy to throw Shuriken at", colord).c_str());
}