#ifndef MENU
#define MENU

struct MenuItem {
	const char* text;
	unsigned char color;
	MenuItem(const char* text = "", unsigned char color = WHITE) : text(text), color(color) {};
};

class Menu {
public:
	// Menu
	Menu(std::vector<MenuItem*>* options, MenuItem* title = new MenuItem()) : texts(new std::vector<MenuItem*>({ title })), opts(options) {};
	// Menu with Texts
	Menu(std::vector<MenuItem*>* options, std::vector<MenuItem*>* texts) : opts(options), texts(texts) {};

	int open() {
		option = 0;
		int choice = -1;
		int size = (int)opts->size() - 1;
		render();
		while (choice == -1) {
			if (_kbhit()) {
				char ch = _getch();

				// Esc
				if (ch == 27) return -1;

				if (size >= 0) {
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
		}
		return choice;
	};

private:
	std::vector<MenuItem*>* texts;
	std::vector<MenuItem*>* opts;
	int option = 0;

	void render() {
		system("cls");
		// Render menu text
		for (int i = 0; i < texts->size(); i++) {
			MenuItem m = *texts->at(i);
			printf("           \033[97;%dm%s\033[m\n\033[?25l", m.color, m.text);
		}
		// Render menu elements
		for (int i = 0; i < opts->size(); i++) {
			MenuItem m = *opts->at(i); 
			printf("         \033[%d;%dm%s%s%s\033[m\n", BG, m.color, i == option ? "> " : "  ", m.text, i == option ? " <" : "");
		}
		// If menu is empty write (empty), empty inventory
		if ((int)opts->size() <= 0) {
			printf("           \033[97;%dm(empty)\033[m\n\033[?25l", WHITE);
		}
	}
};

#endif // !MENU
