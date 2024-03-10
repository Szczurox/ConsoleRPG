#ifndef ENEMY
#define ENEMY

class Tile;

class Enemy {
public:
	const char* name = "Enemy";
	const char* symbol = "\2";
	unsigned char nameColor = BLUE;
	unsigned char color = RED;
	int health = 10;
	int minDamage = 5;
	int maxDamage = 10;
	int defence = 0;
	int minGold = 10;
	int maxGold = 50;
	int level = 0;
	int speed = 1;
	int exp = 10;
	int sight = 8;
	int x = 1;
	int y = 1;

	Enemy() {}
	Enemy(int x, int y) : x(x), y(y) {}

	int hit(int dmg) {
		health -= std::max<int>(dmg - defence, dmg / 4);
		return health;
	}

	int attack() {
		return rand() % (maxDamage - minDamage+1) + minDamage;
	}

	std::vector<std::shared_ptr<Item>> getLoot() {
		std::vector<std::shared_ptr<Item>> items;
		items.push_back(std::shared_ptr<Item>(new GoldPile(minGold, maxGold)));
		return items;
	}

	std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>>attacked(Player* p, bool first = false) {
		// Player dmg, number of player attacks, enemy dmg, number of enemy attacks
		std::pair<std::array<int, 5>, std::vector<std::shared_ptr<Item>>> attacks({ 0, 1, 0, 1, exp }, {});
		int pSpeed;
		if (p->weapon == nullptr) pSpeed = 1;
		else pSpeed = p->weapon->speed;
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
			for (int i = speed; i < pSpeed; i++) {
				int playerDmg = p->attack();
				hit(playerDmg);
				attacks.first[0] += playerDmg;
				attacks.first[1]++;
			}
			// Enemy attacks once if alive
			int enemyDmg = attack();
			if (health > 0 || first) {
				p->hit(attack());
				attacks.first[1] += enemyDmg;
			}
		}
		// Enemy is faster
		else {
			attacks.first[3] = 0;
			// Enemy attacks enemySpeed - playerSpeed amount of times
			for (int i = pSpeed; i < speed; i++) {
				int enemyDmg = attack();
				p->hit(attack());
				attacks.first[1] += enemyDmg;
				attacks.first[3]++;
			}
			// Player attacks once if alive
			int playerDmg = p->attack();
			hit(playerDmg);
			attacks.first[0] += playerDmg;
		}

		if (health <= 0) {
			p->exp += exp;
			attacks.second = getLoot();
		}

		return attacks;
	}
};





#endif // !ENEMY
