#ifndef EFFECTIVE
#define EFFECTIVE

#include<functional>
#include<string>
#include<vector>
#include<memory>

#include"const.hpp"

class Player;
class Item;

enum class BuffType {
    DMG = 0,  // Damage / Weakness
    SPD = 1,  // Speed / Slowness
    PROT = 2,  // Porotection / Vulnerability
    REG = 3,  // Regeneration / Poison
};

class Buff {
public:
    BuffType type = BuffType::DMG;
    int amount = 1;
    int duration = 0;
    bool isBuffing = false;
    bool isNegative = false;

    Buff() {}
    Buff(BuffType type, float amount, int duration, bool isNegative = false) : type(type), amount(amount), duration(duration), isNegative(isNegative) {}

    // Tick down buff duration by 1 turn
    void tick(Player* player);

    void save(std::ostream& os);

    void load(std::istringstream& in);

    std::pair<std::wstring, unsigned char> getType();
};

#endif // EFFECTIVE
