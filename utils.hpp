#ifndef UTILS
#define UTILS

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

void writeColor(const char* text, unsigned char color = WHITE) {
	printf("\033[97;%dm%s\033[m\033[?25l", color, text);
}

std::stringstream colorTextStream(const char* text, int color = GREEN) {
	std::stringstream ss;
	ss << "\033[97;" << (int)color << "m" << text << "\033[m\033[?25l";
	return ss;
}

std::stringstream mergeTextStream(const char* text, const char* text2) {
	std::stringstream ss;
	ss << text << text2;
	return ss;
}

std::string color(const char* text, unsigned char color = RED) {
	return colorTextStream(text, color).str();
}

void write(const char* format) {
	std::cout << format;
}

template<typename T, typename ... Args>
void write(const char* format, T value, Args ... args) {
	for (; *format != '\0'; format++) {
		if (*format == '%') {
			std::cout << value;
			write(format + 1, args ...);
			return;
		}
		std::cout << *format;
	}
}

void moveCursor(size_t row, size_t col) {
	printf("\033[%d;%dH\033[?25l", (int)row + 1, (int)col + 1);
}

#endif // !UTILS