#include <ostream>
#include <sstream>

#include"effective.hpp"
#include"menu.hpp"
#include"items.hpp"
#include"player.hpp"
#include"const.hpp"

void Buff::tick(Player* player) {
    if (type == BuffType::REG)
        player->health += isNegative ? -amount : amount;
    if (player->health > player->maxHealth)
        player->health = player->maxHealth;
    duration--;
}

void Buff::save(std::ostream& os) {
    os << static_cast<int>(type) << " " << amount << " " << duration << "\n";
}

void Buff::load(std::istringstream& in) {
    int intType;
    in >> intType >> amount >> duration;
    type = static_cast<BuffType>(intType);
}

std::pair<std::wstring, unsigned char> Buff::getType() {
    switch (type) {
    case BuffType::DMG:
        return isNegative ? std::make_pair<std::wstring, unsigned char>(L"Weakness", GREY) : std::make_pair<std::wstring, unsigned char>(L"Strength", RED);
    case BuffType::PROT:
        return isNegative ? std::make_pair<std::wstring, unsigned char>(L"Vulnerability", GREY) : std::make_pair<std::wstring, unsigned char>(L"Protection", BLUE);
    case BuffType::REG:
        return isNegative ? std::make_pair<std::wstring, unsigned char>(L"Poison", GREEN) : std::make_pair<std::wstring, unsigned char>(L"Regeneration", BRIGHT_PURPLE);
    case BuffType::SPD:
        return isNegative ? std::make_pair<std::wstring, unsigned char>(L"Slowness", GREY) : std::make_pair<std::wstring, unsigned char>(L"Speed", YELLOW);
    default:
        return {L"", 0};
    }
}
