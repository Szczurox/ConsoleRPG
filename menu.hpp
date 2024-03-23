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
	Menu(std::vector<MenuItem>* options, MenuItem title = MenuItem(), bool space = false) : texts(new std::vector<MenuItem>({ title })), opts(options), space(space) {};
	// Menu with Texts
	Menu(std::vector<MenuItem>* options, std::vector<MenuItem>* texts, bool space = false) : opts(options), texts(texts), space(space) {};

	int open(int opt = 0) {
		option = opt;
		int choice = -1;
		int size = (int)opts->size() - 1;
		render();
		while (choice == -1) {
			int prev = option;
			if (_kbhit()) {
				char ch = _getch();

				// Esc
				if (ch == 27) return -1;

				if (size >= 0) {
					if (ch == 'w' || ch == 'W' || ch == 72)  // Up
						option--;
					else if (ch == 's' || ch == 'S' || ch == 80)  // Down
						option++;

					// Menu cap (prevents player from choosing mode that doesn't exist)
					if (option < 0) option = size;
					if (option > size) option = 0;

					if (ch == '\r' || ch == ' ')
						choice = option;

					render(prev);
				}
			}
		}
		return choice;
	};

private:
	std::vector<MenuItem>* texts;
	std::vector<MenuItem>* opts;
	int option = 0;
	bool space = false;

	void render() {
		system("cls");
		int textsLeng = texts->size();
		// Render menu text
		for (int i = 0; i < textsLeng; i++) {
			MenuItem m = texts->at(i);
			int size = 0;
			while (m.text[size] != '\0') size++;
			setCursor((B_WIDTH - 10) / 2 - size / 2, i);
			printf("  \033[97;%dm%s\033[m\n\033[?25l", m.color, m.text);
		}
		// Render menu elements
		for (int i = 0; i < opts->size(); i++) {
			MenuItem m = opts->at(i); 
			int size = 0;
			while (m.text[size] != '\0') size++;
			setCursor((B_WIDTH - 10) / 2 - size / 2, i + textsLeng + (space ? 1 : 0));
			printf("\033[97;%dm%s%s%s\033[m\n", m.color, i == option ? "> " : "  ", m.text, i == option ? " <" : "");
		}
		// If menu is empty write (empty), empty inventory
		if ((int)opts->size() <= 0) {
			setCursor((B_WIDTH - 10) / 2 - 3, textsLeng);
			printf("\033[97;%dm(empty)\033[m\n\033[?25l", WHITE);
		}
	}

	void render(int prev) {
		// Render menu elements
		MenuItem n = opts->at(prev);
		MenuItem m = opts->at(option);
		int size = 0;
		while (n.text[size] != '\0') size++; 
		int size2 = 0;
		while (m.text[size2] != '\0') size2++;
		setCursor((B_WIDTH - 10) / 2 - size / 2, texts->size() + prev + (space ? 1 : 0));
		printf("\x1b[2K\033[97;%dm%s%s%s\033[m", n.color, "  ", n.text, " ");
		setCursor((B_WIDTH - 10) / 2 - size2 / 2, texts->size() + option + (space ? 1 : 0));
		printf("\x1b[2K\033[97;%dm%s%s%s\033[m", m.color, "> ", m.text, " <");
	}
};

#endif // !MENU
