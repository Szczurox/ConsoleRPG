#ifndef MENU
#define MENU

#include<vector>
#include<cstdlib>
#include<iostream>
#include<conio.h>

enum Colors {
	WHITE = 37,
	RED = 31,
	GREEN = 32,
	YELLOW = 33,
	BLUE = 34,
	PURPLE = 35,
	CYAN = 36,
	BRIGHT_CYAN = 96,
	BRIGHT_BG = 103,
	BG = 97,
};

struct MenuItem {
	const char* text;
	unsigned char color;
	MenuItem(const char* text = "", unsigned char color = 0) : text(text), color(color) {};
};

class Menu {
public:
	// Menu
	Menu(std::vector<MenuItem> options, MenuItem title = MenuItem()) : title(title), opts(options) {};
	// Text info
	Menu(MenuItem title) : title(title), opts(std::vector<MenuItem>({ MenuItem("CONTINUE", GREEN) })) {};

	int open() {
		option = 0;
		int choice = -1;
		size_t size = opts.size() - 1;
		render();
		while (choice == -1) {
			if (_kbhit()) {
				char ch = _getch();

				// Esc
				if (ch == 27) return 0;

				system("cls");

				if (ch == 'w' || ch == 'W' || ch == 72)  // Up
					option--;
				else if (ch == 's' || ch == 'S' || ch == 80)  // Down
					option++;

				// Menu cap (prevents player from choosing mode that doesn't exist)
				if (option < 0) option = 0;
				if (option > size) option = (int)size;

				if (ch == '\r' || ch == ' ')
					choice = option;

				render();
			}
		}
		return choice;
	};
	~Menu() {};

private:
	MenuItem title;
	std::vector<MenuItem> opts;
	int option = 0;

	void render() {
		printf("          \033[97;%dm%s\033[m\n\033[?25l", title.color, title.text);
		for (int i = 0; i < opts.size(); i++) {
			MenuItem m = opts[i]; 
			printf("         \033[%d;%dm%s%s%s\033[m\n", BG, m.color, i == option ? "> " : "  ", m.text, i == option ? " <" : "");
		}
	}
};

#endif // !MENU
