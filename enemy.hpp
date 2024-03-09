#ifndef ENEMY
#define ENEMY

class Enemy {
public:
	const char* symbol = "\2";
	unsigned char color = RED;
	int health = 10;
	int minDamage = 5;
	int maxDamage = 15;
	int defence = 0;
	int gold = 0;
	int level = 0;
	int speed = 1;
	size_t x = 1;
	size_t y = 1;

	Enemy() {}

	int hit(int dmg) {
		health -= std::max<int>(dmg - defence, dmg / 4);
		return health;
	}

	int attack() {
		return rand() % (maxDamage - minDamage) + minDamage;
	}

	std::array<int, 4> attacked(Player* p) {
		// Player dmg, number of player attacks, enemy dmg, number of enemy attacks
		std::array<int, 4> attacks({ 0, 1, 0, 1 });
		int pSpeed;
		if (p->weapon == nullptr) pSpeed = 1;
		else pSpeed = p->weapon->speed;
		// Player and enemy have same speed
		if (pSpeed == speed) {
			// Both attack each other once
			int playerDmg = p->attack();
			int enemyDmg = attack();
			hit(playerDmg);
			attacks[0] = playerDmg;
			if (health > 0) {
				p->hit(attack());
				attacks[2] = enemyDmg;
			}
		}
		// Player is faster
		else if (pSpeed > speed) {
			attacks[1] = 0;
			// Player attacks playerSpeed - enemySpeed amount of times
			for (int i = speed; i < pSpeed; i++) {
				int playerDmg = p->attack();
				hit(playerDmg);
				attacks[0] += playerDmg;
				attacks[1]++;
			}
			// Enemy attacks once if alive
			int enemyDmg = attack();
			if (health > 0) {
				p->hit(attack());
				attacks[1] += enemyDmg;
			}
		}
		// Enemy is faster
		else {
			attacks[3] = 0;
			// Enemy attacks enemySpeed - playerSpeed amount of times
			for (int i = pSpeed; i < speed; i++) {
				int enemyDmg = attack();
				p->hit(attack());
				attacks[1] += enemyDmg;
				attacks[3]++;
			}
			// Player attacks once if alive
			int playerDmg = p->attack();
			if (p->health > 0) {
				hit(playerDmg);
				attacks[0] += playerDmg;
			}
		}

		return attacks;
	}
};



#endif // !ENEMY
