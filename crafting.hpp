#ifndef CRAFT
#define CRAFT

#include<memory>

#include"menu.hpp"
#include"effective.hpp"

// Bone Armor recipe
class BARecipe : public Recipe {
public:
	virtual std::shared_ptr<Item> getItem();

	BARecipe(bool unl = false);
};

void pushRecipies(Player& p);

#endif // !CRAFT