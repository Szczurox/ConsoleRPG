#ifndef UTILS
#define UTILS

// Directional arrays
// Used for checking tiles surrounding a main tile in this order:
//   1
// 3   4
//   2 
int dx[4] = { 0, 0, -1, 1 };
int dy[4] = { -1, 1, 0, 0 };

enum Direction {
	UP = 0,
	DOWN = 1,
	LEFT = 2,
	RIGHT = 3
};

enum Colors {
	WHITE = 37,
	RED = 31,
	GREEN = 32,
	YELLOW = 33,
	BLUE = 34,
	PURPLE = 35,
	CYAN = 36,
	BRIGHT_BG = 103,
	YELLOW_BG = 43,
	GREY = 90,
	BRIGHT_BLUE = 94,
	BRIGHT_YELLOW = 93,
	BRIGHT_GREEN = 92,
	BRIGHT_RED = 91,
	BRIGHT_PURPLE = 95,
	BRIGHT_CYAN = 96,
	BRIGHT_WHITE = 97,
};

void writeColor(const wchar_t* text, unsigned char color = WHITE) {
	std::wprintf(L"\033[97;%dm%ls\033[m\033[?25l", color, text);
}

std::wstringstream colorTextStream(const wchar_t* text, int color = GREEN) {
	std::wstringstream ss;
	ss << L"\033[97;" << (int)color << L"m" << text << L"\033[m\033[?25l";
	return ss;
}

std::wstringstream mergeTextStream(const wchar_t* text, const wchar_t* text2) {
	std::wstringstream ss;
	ss << text << text2;
	return ss;
}

std::wstring color(const wchar_t* text, unsigned char color = RED) {
	return colorTextStream(text, color).str();
}

void write(const wchar_t* format) {
	std::wcout << format;
}

template<typename T, typename ... Args>
void write(const wchar_t* format, T value, Args ... args) {
	for (; *format != '\0'; format++) {
		if (*format == '%') {
			std::wcout << value;
			write(format + 1, args ...);
			return;
		}
		std::wcout << *format;
	}
}

void setCursor(int col, int row) {
	std::wprintf(L"\033[%d;%dH\033[?25l", (int)row + 1, (int)col + 1);
}

void clearLine(int row) {
	setCursor(0, row);
	std::wcout << L"\x1b[2K";
}

int randMinMax(int min, int max) {
	return rand() % (max - min + 1) + min;
}

bool chance(int prob, int omega) {
	return prob > (rand() % omega);
}

int distance(int x, int y, int x2, int y2) {
	return (int)sqrtf((float)((y - y2) * (y - y2) + (x - x2) * (x - x2)));
};

void setWindow(int width, int height) {
	_COORD coord;
	coord.X = width;
	coord.Y = height;

	_SMALL_RECT rect;
	rect.Top = 0;
	rect.Left = 0;
	rect.Bottom = height - 1;
	rect.Right = width - 1;

	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);   // Get Handle 
	SetConsoleScreenBufferSize(Handle, coord);         // Set Buffer Size 
	SetConsoleWindowInfo(Handle, TRUE, &rect);         // Set Window Size 
	ShowScrollBar(GetConsoleWindow(), SB_BOTH, 0);     // Hide Scrollbar
}

std::vector<std::wstring> getFilesWithPrefix(const std::wstring& dirPath, const std::wstring& prefix) {
	std::vector<std::wstring> files;

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((dirPath + L"\\*").c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		std::cerr << "Invalid directory path!" << std::endl;
		return files;
	}

	do {
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;  // Skip directories

		std::wstring fileName = findFileData.cFileName;

		if (fileName.find(prefix) == 0)
			files.push_back(fileName);

	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);
	return files;
}

#endif // !UTILS