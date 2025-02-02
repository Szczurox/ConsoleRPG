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

std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>> Enemy::attacked(Player* p, bool first) {
    // Player dmg, number of player attacks, enemy dmg, number of enemy attacks
	std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>> attacks({ 0, 1, 0, 1, xp }, {});
	int pSpeed;
	if (p->weapon == nullptr) pSpeed = p->baseSpeed;
	else pSpeed = p->weapon->speed + p->baseSpeed;
	// Player and enemy have same speed
	if (pSpeed == speed) {
		// Both attack each other once
		int playerDmg = p->attack();
		int enemyDmg = attack();
		hit(playerDmg);
		attacks.first[0] = playerDmg;
		if (health > 0 || first) {
			p->hit(attack());
			attacks.first[2] = enemyDmg;
		}
	}
	// Player is faster
	else if (pSpeed > speed) {
		attacks.first[1] = 0;
		// Player attacks playerSpeed - enemySpeed amount of times
		for (int i = speed; i <= pSpeed; i++) {
			int playerDmg = p->attack();
			hit(playerDmg);
			attacks.first[0] += playerDmg;
			attacks.first[1]++;
		}
		// Enemy attacks once if alive
		int enemyDmg = attack();
		if (health > 0 || first) {
			p->hit(attack());
			attacks.first[2] += enemyDmg;
		}
	}
	// Enemy is faster
	else {
		attacks.first[3] = 0;
		// Enemy attacks enemySpeed - playerSpeed amount of times
		for (int i = pSpeed; i <= speed; i++) {
			int enemyDmg = attack();
			p->hit(attack());
			attacks.first[2] += enemyDmg;
			attacks.first[3]++;
		}
		// Player attacks once if alive
		int playerDmg = p->attack();
		hit(playerDmg);
		attacks.first[0] += playerDmg;
	}

	if (health <= 0) {
		p->xp += xp;
		p->checkLevelUp();
		attacks.second = getLoot();
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

std::vector<std::shared_ptr<Item>> Assassin::getLoot() {
    std::vector<std::shared_ptr<Item>> items;
    randLoot<GoldPile>(items, 4, 1, 2, minGold, maxGold);
    randLoot<IronShortsword>(items, 1, 1, 75, randMinMax(20, 200));
    return items;
}
