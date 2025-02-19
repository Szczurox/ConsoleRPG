#include<functional>
#include<filesystem>
#include<algorithm>
#include<iostream>
#include<direct.h>
#include<fcntl.h>
#include<fstream>
#include<cstdarg>
#include<cstdlib>
#include<sstream>
#include<codecvt>
#include<locale>
#include<vector>
#include<memory>
#include<regex>
#include<array>
#include<map>

#include"utils.hpp"
#include"menu.hpp"
#include"effective.hpp"
#include"player.hpp"
#include"items.hpp"
#include"crafting.hpp"
#include"npc.hpp"
#include"enemy.hpp"
#include"tiles.hpp"
#include"board.hpp"

std::shared_ptr<MenuItem> createMenuItem(std::wstring text, unsigned char color) {
	return std::make_shared<MenuItem>(text, color);
}

std::shared_ptr<MenuItem> createMenuItem(int colorNum, std::wstring text) {
	return std::make_shared<MenuItem>(colorNum, text);
}

std::shared_ptr<MenuItem> createMenuItem(std::vector<std::wstring> texts, unsigned char color) {
	return std::make_shared<MenuItem>(texts, color);
}

std::shared_ptr<MenuItem> createMenuItem(int colorNum, std::vector<std::wstring> texts) {
	return std::make_shared<MenuItem>(colorNum, texts);
}

void Menu::init(std::vector<std::shared_ptr<MenuItem>> options) {
	opts = std::vector<std::vector<std::shared_ptr<MenuItem>>>();
	size_t pageSize = 20 - texts.size();
	int optionsSize = options.size();
	int pages;
	pages = optionsSize / pageSize;
	if (optionsSize % pageSize != 0)
		pages++;
	for (size_t i = 0; i < pages; i++) {
		std::vector<std::shared_ptr<MenuItem>> op = {};
		for (size_t j = 0; j < pageSize && i * pageSize + j < options.size(); j++)
			op.push_back(options.at(i * (int)pageSize + j));
		opts.push_back(op);
	}
}

int Menu::open(int opt) {
	option = opt;
	int choice = -1;
	render();
	while (choice == -1) {
		int prev = option;
		int size = (int)opts[curPage].size() - 1;
		int pages = opts.size() - 1;
		if (kbhit_cross()) {
			char ch = getch_cross();

			// Esc
			if (ch == 27) return -1;

			// Go up and down on current page
			if (size >= 0) {
				if (ch == 'w' || ch == 'W' || ch == 72)  // Up
					option--;
				else if (ch == 's' || ch == 'S' || ch == 80)  // Down
					option++;
				else if ((ch == 'a' || ch == 'A' || ch == 75) && opts[curPage][option]->isSelectable)  // Left
					opts[curPage][option]->selected++;
				else if ((ch == 'd' || ch == 'D' || ch == 77) && opts[curPage][option]->isSelectable)  // Right
					opts[curPage][option]->selected--;

				// Menu cap (prevents player from choosing option that doesn't exist)
				if (option < 0) option = size;
				else if (option > size) option = 0;

				if (opts[curPage][option]->isSelectable) {
					int selectSize = opts[curPage][option]->texts.size() - 1;
					if (opts[curPage][option]->selected < 0) opts[curPage][option]->selected = selectSize;
					else if (opts[curPage][option]->selected > selectSize) opts[curPage][option]->selected = 0;
				}

				if (ch == '\r' || ch == ' ')
					choice = option;

				render(prev);
			}

			// Switch to the next page if all elements didn't fit on one page
			if (pages > 0) {
				if (ch == 'a' || ch == 'A' || ch == 75)
					curPage--;
				else if (ch == 'd' || ch == 'D' || ch == 77)
					curPage++;

				if (curPage < 0) curPage = pages;
				else if (curPage > pages) curPage = 0;

				if ((ch == 'a' || ch == 'A' || ch == 75) || (ch == 'd' || ch == 'D' || ch == 77)) {
					option = 0;
					render();
				}
			}
		}
	}
	return choice;
};

void Menu::render() {
	system("cls");
	int textsLeng = texts.size();
	// Render menu text
	for (int i = 0; i < textsLeng; i++) {
		std::shared_ptr<MenuItem> m = texts.at(i);
		int size = m->texts[m->selected].size();
		setCursor((B_WIDTH - 10) / 2 - size / 2 + m->colorsCount * 9, i);
		if (m->color != 0)
			wprintf(L"  \033[97;%dm%ls\033[m\n\033[?25l", m->color, m->texts[m->selected].c_str());
		else
			wprintf(L"  %ls\n\033[?25l", m->texts[m->selected].c_str());
	}
	// Render menu elements
	for (int i = 0; i < opts[curPage].size(); i++) {
		std::shared_ptr<MenuItem> m = opts[curPage].at(i);
		int size = m->texts[m->selected].size();
		setCursor((B_WIDTH - 10) / 2 - size / 2 + m->colorsCount * 9, i + textsLeng + (space ? 1 : 0));
		if (m->color != 0)
			wprintf(L"\033[40;%dm%ls%ls%ls\033[m\n", m->color, i == option ? m->isSelectable ? L"< " : L"> " : L"  ", m->texts[m->selected].c_str(), i == option ? m->isSelectable ? L" >" : L" <" : L"  ");
		else
			wprintf(L"%ls%ls%ls\n", i == option ? m->isSelectable ? L"< " : L"> " : L"  ", m->texts[m->selected].c_str(), i == option ? m->isSelectable ? L" >" : L" <" : L"  ");
	}
	if (opts.size() > 1) {
		setCursor((B_WIDTH - 10) / 2 - 3, textsLeng + opts[curPage].size() + 1);
		wprintf(L"\033[40;%dm(Page %d/%d)\033[m\n\033[?25l", WHITE, curPage + 1, static_cast<int>(opts.size()));
	}
	// If menu is empty write (empty), empty inventory
	if ((int)opts[0].size() <= 0) {
		setCursor((B_WIDTH - 10) / 2 - 3, textsLeng);
		wprintf(L"\033[40;%dm(empty)\033[m\n\033[?25l", WHITE);
	}
}

void Menu::render(int prev) {
	// Render menu elements
	std::shared_ptr<MenuItem> n = opts[curPage].at(prev);
	std::shared_ptr<MenuItem> m = opts[curPage].at(option);
	// Previously selected element
	int size = n->texts[n->selected].size();
	setCursor((B_WIDTH - 10) / 2 - size / 2 + n->colorsCount * 9, texts.size() + prev + (space ? 1 : 0));
	if (n->color != 0)
		wprintf(L"\x1b[2K\033[40;%dm%ls%ls%ls\033[m", n->color, L"  ", n->texts[n->selected].c_str(), L"  ");
	else
		wprintf(L"\x1b[2K%ls%ls%ls", L"  ", n->texts[n->selected].c_str(), L"  ");
	// Newly selected element
	int size2 = m->texts[m->selected].size();
	setCursor((B_WIDTH - 10) / 2 - size2 / 2 + m->colorsCount * 9, texts.size() + option + (space ? 1 : 0));
	if (m->color != 0)
		wprintf(L"\x1b[2K\033[40;%dm%ls%ls%ls\033[m", m->color, m->isSelectable ? L"< " : L"> ", m->texts[m->selected].c_str(), m->isSelectable ? L" >" : L" <");
	else
		wprintf(L"\x1b[2K%ls%ls%ls", m->isSelectable ? L"< " : L"> ", m->texts[m->selected].c_str(), m->isSelectable ? L" >" : L" <");
}