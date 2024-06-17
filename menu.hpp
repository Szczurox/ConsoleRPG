#ifndef MENU
#define MENU

struct MenuItem {
	const wchar_t* text = L"";
	unsigned char color = WHITE;
	int colorsCount = 0;
	MenuItem(const wchar_t* text, unsigned char color) : text(text), color(color) {}
	MenuItem(int howManyColors, const wchar_t* text) : text(text), color(0), colorsCount(howManyColors) {}
	MenuItem() {}
};

class Menu {
public:
	// Menu
	Menu(std::vector<MenuItem>* options, MenuItem title = MenuItem(), bool space = false) : texts(new std::vector<MenuItem>({ title })), space(space) { init(options); };
	// Menu with Texts
	Menu(std::vector<MenuItem>* options, std::vector<MenuItem>* texts, bool space = false) : texts(texts), space(space) { init(options);  };

	void init(std::vector<MenuItem>* options) {
		opts = std::vector<std::vector<MenuItem>*>();
		int pageSize = 20 - texts->size();
		int optionsSize = options->size();
		int pages;
		pages = optionsSize / pageSize;
		if (optionsSize % pageSize != 0)
			pages++;
		for (int i = 0; i < pages; i++) {
			std::vector<MenuItem>* op = new std::vector<MenuItem>();
			for (int j = 0; j < pageSize && i * pageSize + j < options->size(); j++)
				op->push_back(options->at(i * pageSize + j));
			opts.push_back(op);
		}
	}

	int open(int opt = 0) {
		option = opt;
		int choice = -1;
		render();
		while (choice == -1) {
			int prev = option;
			int size = (int)opts[curPage]->size() - 1;
			int pages = opts.size() - 1;
			if (_kbhit()) {
				char ch = _getch();

				// Esc
				if (ch == 27) return -1;

				// Go up and down on current page
				if (size >= 0) {
					if (ch == 'w' || ch == 'W' || ch == 72)  // Up
						option--;
					else if (ch == 's' || ch == 'S' || ch == 80)  // Down
						option++;

					// Menu cap (prevents player from choosing mode that doesn't exist)
					if (option < 0) option = size;
					else if (option > size) option = 0;

					if (ch == '\r' || ch == ' ')
						choice = option;

					render(prev);
				}

				// Switch to the next page if all elements didn't fit on one page
				if (pages >= 0) {
					if (ch == 'a')
						curPage--;
					else if (ch == 'd')
						curPage++;

					if (curPage < 0) curPage = pages;
					else if (curPage > pages) curPage = 0;

					if (ch == 'a' || ch == 'd') {
						option = 0;
						render();
					}
				}
			}
		}
		return choice;
	};

	void refresh() {
		render();
	}

	~Menu() {
		for (auto o : opts)
			delete o;
	}

private:
	std::vector<MenuItem>* texts;
	std::vector<std::vector<MenuItem>*> opts;
	int option = 0;
	int curPage = 0;
	bool space = false;

	void render() {
		system("cls");
		int textsLeng = texts->size();
		// Render menu text
		for (int i = 0; i < textsLeng; i++) {
			MenuItem m = texts->at(i);
			int size = 0;
			while (m.text[size] != '\0') size++;
			setCursor((B_WIDTH - 10) / 2 - size / 2 + m.colorsCount * 9, i);
			if (m.color != 0)
				wprintf(L"   \033[97;%dm%ls\033[m\n\033[?25l", m.color, m.text);
			else
				wprintf(L"  %ls\n\033[?25l", m.text);
		}
		// Render menu elements
		for (int i = 0; i < opts[curPage]->size(); i++) {
			MenuItem m = opts[curPage]->at(i); 
			int size = 0;
			while (m.text[size] != '\0') size++;
			setCursor((B_WIDTH - 10) / 2 - size / 2 + m.colorsCount * 9, i + textsLeng + (space ? 1 : 0));
			if (m.color != 0)
				wprintf(L"\033[97;%dm%ls%ls%ls\033[m\n", m.color, i == option ? L"> " : L"  ", m.text, i == option ? L" <" : L"");
			else
				wprintf(L"%ls%ls%ls\n", i == option ? L"> " : L"  ", m.text, i == option ? L" <" : L"");
		}
		if (opts.size() > 1) {
			setCursor((B_WIDTH - 10) / 2 - 3, textsLeng + opts[curPage]->size() + 1);
			wprintf(L"\033[97;%dm(Page %d/%d)\033[m\n\033[?25l", WHITE, curPage + 1, opts.size());
		}
		// If menu is empty write (empty), empty inventory
		if ((int)opts[0]->size() <= 0) {
			setCursor((B_WIDTH - 10) / 2 - 3, textsLeng);
			wprintf(L"\033[97;%dm(empty)\033[m\n\033[?25l", WHITE);
		}
	}

	void render(int prev) {
		// Render menu elements
		MenuItem n = opts[curPage]->at(prev);
		MenuItem m = opts[curPage]->at(option);
		// Previously selected element
		int size = 0;
		while (n.text[size] != '\0') size++; 
		setCursor((B_WIDTH - 10) / 2 - size / 2 + n.colorsCount * 9, texts->size() + prev + (space ? 1 : 0));
		if (n.color != 0)
			wprintf(L"\x1b[2K\033[97;%dm%ls%ls%ls\033[m", n.color, L"  ", n.text, L" ");
		else 
			wprintf(L"\x1b[2K%ls%ls%ls", L"  ", n.text, L" ");
		// Newly selected element
		int size2 = 0;
		while (m.text[size2] != '\0') size2++;
		setCursor((B_WIDTH - 10) / 2 - size2 / 2 + m.colorsCount * 9, texts->size() + option + (space ? 1 : 0));
		if (m.color != 0)
			wprintf(L"\x1b[2K\033[97;%dm%ls%ls%ls\033[m", m.color, L"> ", m.text, L" <");
		else
			wprintf(L"\x1b[2K%ls%ls%ls", L"> ", m.text, L" <");
	}
};

#endif // !MENU
