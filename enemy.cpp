#include<sstream>

#include"enemy.hpp"
#include"player.hpp"
#include"items.hpp"
#include"utils.hpp"

int Enemy::hit(int dmg) {
    health -= std::max<int>(dmg - defence, dmg / 4);
    return health;
}

int Enemy::attack() {
    return rand() % (maxDamage - minDamage + 1) + minDamage;
}

std::vector<std::shared_ptr<Item>> Enemy::getLoot() {
    std::vector<std::shared_ptr<Item>> items;
    randLoot<GoldPile>(items, 3, 1, 2, minGold, maxGold);
    return items;
}

std::tuple<std::array<int, 5>, std::pair<std::wstring, unsigned char>> Enemy::attacked(Player* p, bool first) {
    // Player dmg, number of player attacks, enemy dmg, number of enemy attacks
	std::tuple<std::array<int, 5>, std::pair<std::wstring, unsigned char>> attacks({ 0, 1, 0, 1, xp }, {L"", 0});
	// Player and enemy have same speed
	if (p->speed == speed) {
		// Both attack each other once
		int playerDmg = p->attack();
		int enemyDmg = attack();
		hit(playerDmg);
		std::get<0>(attacks)[0] = playerDmg;
		if (health > 0 || first) {
			std::get<1>(attacks) = special(p);
			p->hit(attack());
			std::get<0>(attacks)[2] = enemyDmg;
		}
	}
	// Player is faster
	else if (p->speed > speed) {
		std::get<0>(attacks)[1] = 0;
		// Player attacks playerSpeed - enemySpeed amount of times
		for (int i = speed; i <= p->speed; i++) {
			int playerDmg = p->attack();
			hit(playerDmg);
			std::get<0>(attacks)[0] += playerDmg;
			std::get<0>(attacks)[1]++;
			if (health <= 0)
				break;
		}
		// Enemy attacks once if alive
		int enemyDmg = attack();
		if (health > 0 || first) {
			std::get<1>(attacks) = special(p);
			p->hit(attack());
			std::get<0>(attacks)[2] += enemyDmg;
		}
	}
	// Enemy is faster
	else {
		std::get<0>(attacks)[3] = 0;
		// Enemy attacks enemySpeed - playerSpeed amount of times
		for (int i = p->speed; i <= speed; i++) {
			int enemyDmg = attack();
			std::get<1>(attacks) = special(p);
			p->hit(attack());
			std::get<0>(attacks)[2] += enemyDmg;
			std::get<0>(attacks)[3]++;
		}
		// Player attacks once if alive
		int playerDmg = p->attack();
		hit(playerDmg);
		std::get<0>(attacks)[0] += playerDmg;
	}

	return attacks;
};

std::string Enemy::getType() const {
    std::string typeName = typeid(*this).name();
    if (typeName.rfind("class ", 0) == 0)
        typeName = typeName.substr(6);
    return typeName;
}

void Enemy::save(std::ostream& os) {
    os << getType() << " " << x << " " << y << " " << roomNum << "\n";
}

void Enemy::load(std::istringstream& in) {
    in >> x >> y >> roomNum;
}

EnemyFactory::EnemyFactory() {}

std::shared_ptr<Enemy> EnemyFactory::createEnemy(const std::string& type) {
    return enemyMap[type]();
}

std::vector<std::shared_ptr<Item>> Skeleton::getLoot() {
    std::vector<std::shared_ptr<Item>> items;
    randLoot<GoldPile>(items, 3, 1, 2, minGold, maxGold);
    randLoot<Bone>(items, 5, 1, 3);
    return items;
}

std::vector<std::shared_ptr<Item>> Zombie::getLoot() {
    std::vector<std::shared_ptr<Item>> items;
    randLoot<GoldPile>(items, 4, 1, 2, minGold, maxGold);
    randLoot<ZombieMeat>(items, 5, 1, 3);
    return items;
}

std::pair<std::wstring, unsigned char> Zombie::special(Player* p) {
	if(chance(1, 4))
		return p->giveBuff(BuffType::DMG, 1, 3, true);
	return {L"", 0};
}

std::vector<std::shared_ptr<Item>> Assassin::getLoot() {
    std::vector<std::shared_ptr<Item>> items;
    randLoot<GoldPile>(items, 4, 1, 2, minGold, maxGold);
    randLoot<IronShortsword>(items, 1, 1, 75, randMinMax(20, 190));
    return items;
}

std::pair<std::wstring, unsigned char> Assassin::special(Player* p) {
	if (chance(1, 3))
		return p->giveBuff(BuffType::PROT, 1, 3, true);
	return { L"", 0 };
}

std::vector<std::shared_ptr<Item>> Snake::getLoot() {
	std::vector<std::shared_ptr<Item>> items;
	randLoot<WoodenSword>(items, 1, 1, 50, randMinMax(1, 100));
	return items;
}

std::pair<std::wstring, unsigned char> Snake::special(Player* p) {
	if (chance(1, 3))
		return p->giveBuff(BuffType::REG, 1, 6, true);
	return { L"", 0 };
}