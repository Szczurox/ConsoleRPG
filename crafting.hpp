#ifndef CRAFT
#define CRAFT

// Bone Armor recipe
class BARecipe : public Recipe {
public:
	virtual std::shared_ptr<Item> getItem() {
		return std::shared_ptr<Item>(new BoneArmor());
	}

	BARecipe(bool unl = false) {
		unlocked = unl;
		item = std::shared_ptr<Item>(new BoneArmor());
		items = std::vector<std::shared_ptr<Item>>();
		items.push_back(std::shared_ptr<Item>(new Bone(30)));
	};
};

void pushRecipies(Player& p) {
	p.addRecipe<BARecipe>(true);
}

#endif // !CRAFT