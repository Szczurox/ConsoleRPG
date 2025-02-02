#ifndef UTILS
#define UTILS

#include<iostream>

#include"const.hpp"

enum Direction {
	UP = 0,
	DOWN = 1,
	LEFT = 2,
	RIGHT = 3
};

void writeColor(const wchar_t* text, unsigned char color = WHITE);

std::wstringstream colorTextStream(const wchar_t* text, int color = GREEN);

std::wstringstream mergeTextStream(const wchar_t* text, const wchar_t* text2);

std::wstring color(std::wstring text, unsigned char color = WHITE);
std::wstring color(const wchar_t* text, unsigned char color = WHITE);

void write(const wchar_t* format);

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

void setCursor(int col, int row);

void clearLine(int row);

int randMinMax(int min, int max);

bool chance(int prob, int omega);

int distance(int x, int y, int x2, int y2);

// Set window's size
void setWindow(int width, int height);

// Get all files with a certain prefix
std::vector<std::wstring> getFilesWithPrefix(const std::wstring& dirPath, const std::wstring& prefix);

// Check if directory exists (cross-platform)
bool directoryExists(const std::wstring& dirPath);

// Create a directory (cross-platform)
bool createDirectory(const std::wstring& path);

// Get all directories in a directory
std::vector<std::wstring> getDirectories(const std::wstring& dirPath);

// Function to remove all files and subdirectories recursively in the specified directory (cross-platform)
bool removeDirectory(const std::wstring& dirPath);

// Main function to remove a specified directory (cross-platform)
bool removeDirectoryRecursive(const std::wstring& dirPath);

std::vector<std::wstring> listDirectories(const std::wstring& path);

int getch_cross();

int kbhit_cross();

#endif // !UTILS