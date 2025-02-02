#ifndef MENU
#define MENU

#include<string>

#include"const.hpp"

class MenuItem {
public:
	std::wstring text;
	unsigned char color = WHITE;
	int colorsCount = 0;
	MenuItem(std::wstring text, unsigned char color) : text(text), color(color) {};
	MenuItem(int howManyColors, std::wstring text) : text(text), color(0), colorsCount(howManyColors) {};
	MenuItem(): text(L"") {};
};

std::shared_ptr<MenuItem> createMenuItem(std::wstring text, unsigned char color);

std::shared_ptr<MenuItem> createMenuItem(int colorNum, std::wstring text);

class Menu {
public:
	std::vector<std::shared_ptr<MenuItem>> texts = {};
	std::vector<std::vector<std::shared_ptr<MenuItem>>> opts;
	// Menu with title
	Menu(std::vector<std::shared_ptr<MenuItem>> options, std::shared_ptr<MenuItem> title = std::shared_ptr<MenuItem>(new MenuItem()), bool space = false) : texts(std::vector<std::shared_ptr<MenuItem>>()), space(space) {
		texts.push_back(title);
		init(options);
	};
	// Menu with Texts
	Menu(std::vector<std::shared_ptr<MenuItem>> options, std::vector<std::shared_ptr<MenuItem>> texts, bool space = false) : texts(texts), space(space) { init(options);  };

	void init(std::vector<std::shared_ptr<MenuItem>> options);

	int open(int opt = 0);

	void refresh() { render(); }

private:
	int option = 0;
	int curPage = 0;
	bool space = false;

	void render();

	void render(int prev);
};

#endif // !MENU
