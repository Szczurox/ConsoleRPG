#include<vector>

#include"crafting.hpp"
#include"player.hpp"
#include"items.hpp"

BARecipe::BARecipe(bool unl) {
    unlocked = unl;
    item = std::shared_ptr<Item>(new BoneArmor());
    items = std::vector<std::shared_ptr<Item>>();
    items.push_back(std::shared_ptr<Item>(new Bone(30)));
}

std::shared_ptr<Item> BARecipe::getItem() {
    return std::shared_ptr<Item>(new BoneArmor());
}

// Add all of the recipes to the Player's recipes
void pushRecipies(Player& p) {
    p.addRecipe<BARecipe>(true);
}
