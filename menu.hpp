#ifndef MENU
#define MENU

#include<string>
#include<memory>

#include"const.hpp"

class MenuItem {
public:
	std::vector<std::wstring> texts = {};
	unsigned char color = WHITE;
	int colorsCount = 0;
	int selected = 0;
	bool isSelectable = false;
	MenuItem(std::wstring text, unsigned char color) : texts({ text }), color(color) {};
	MenuItem(std::vector<std::wstring> texts, unsigned char color) : texts(texts), color(color), isSelectable(true) {};
	MenuItem(int howManyColors, std::wstring text) : texts({ text }), colorsCount(howManyColors), color(0) {};
	MenuItem(int howManyColors, std::vector<std::wstring> texts) : texts(texts), colorsCount(howManyColors), isSelectable(true), color(0) {};
	MenuItem(): texts({ L"" }) {};
};

std::shared_ptr<MenuItem> createMenuItem(std::wstring text, unsigned char color);

std::shared_ptr<MenuItem> createMenuItem(int colorNum, std::wstring text);

std::shared_ptr<MenuItem> createMenuItem(std::vector<std::wstring> texts, unsigned char color);

std::shared_ptr<MenuItem> createMenuItem(int colorNum, std::vector<std::wstring> texts);

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
