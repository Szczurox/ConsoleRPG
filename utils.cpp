#include<filesystem>
#include<iostream>
#include<sstream>
#include<string>
#include<cwctype>
#include<regex>
#include<atomic>
#include<math.h>
#include<mutex>
#include<xaudio2.h>
#if defined(_WIN32) || defined(_WIN64)
#include<windows.h>
#include<conio.h>

#else
#include<sys/ioctl.h>
#include<dirent.h>
#include<unistd.h>
#include<termios.h>
#include<fcntl.h>
#endif

#include"utils.hpp"
#include"const.hpp"

void writeColor(const wchar_t* text, unsigned char color, unsigned char bgColor) {
	std::wprintf(L"\033[%d;%dm%ls\033[m\033[?25l", bgColor + 10, color, text);
}

std::wstringstream colorTextStream(const wchar_t* text, unsigned char color, unsigned char bgColor) {
	std::wstringstream ss;
	ss << L"\033[" << bgColor + 10 << ";" << (int)color << L"m" << text << L"\033[m\033[?25l";
	return ss;
}

std::wstringstream mergeTextStream(const wchar_t* text, const wchar_t* text2) {
	std::wstringstream ss;
	ss << text << text2;
	return ss;
}

std::wstring color(std::wstring text, unsigned char color, unsigned char bgColor) {
	return colorTextStream(text.c_str(), color, bgColor).str();
}

std::wstring color(const wchar_t* text, unsigned char color, unsigned char bgColor) {
	return colorTextStream(text, color, bgColor).str();
}

void write(const wchar_t* format) {
	std::wcout << format;
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
#if defined(_WIN32) || defined(_WIN64)
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
#else
	struct winsize w;
	w.ws_row = height;
	w.ws_col = width;

	if (ioctl(STDOUT_FILENO, TIOCSWINSZ, &w) == -1) {
		std::cerr << "Error setting terminal size!" << std::endl;
	}

	// ANSI escape sequence to resize terminal (may require terminal support)
	std::cout << "\033[8;" << height << ";" << width << "t";
#endif
}

std::vector<std::wstring> getFilesWithPrefix(const std::wstring& dirPath, const std::wstring& prefix) {
	std::vector<std::wstring> files;
#if defined(_WIN32) || defined(_WIN64)
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = FindFirstFileW((dirPath + L"\\*").c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		std::wcerr << L"Invalid directory path!" << std::endl;
		return files;
	}
	do {
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		std::wstring fileName(findFileData.cFileName);

		if (fileName.find(prefix) == 0)
			files.push_back(fileName);

	} while (FindNextFileW(hFind, &findFileData) != 0);

	FindClose(hFind);
#else
	try {
		for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
			if (entry.is_regular_file()) {
				std::wstring fileName = entry.path().filename().wstring();
				if (fileName.find(prefix) == 0) {
					files.push_back(fileName);
				}
			}
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::wcerr << L"Invalid directory path: " << e.what() << std::endl;
	}
#endif
	return files;
}

bool directoryExists(const std::wstring& dirPath) {
#ifdef _WIN32
	DWORD dwAttrib = GetFileAttributesW(dirPath.c_str());

	// Check if path exists and is a directory
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
	struct stat info;

	// Check if path exists and is a directory
	if (stat(std::string(dirPath.begin(), dirPath.end()).c_str(), &info) != 0)
		return false;  // Could not access the directory
	return (info.st_mode & S_IFDIR);  // Check if it's a directory
#endif
}

// Function to create a directory with a specific name (cross-platform)
bool createDirectory(const std::wstring& path) {
	std::wstring dirPath = path;  // Combine base path and directory name

#if defined(_WIN32) || defined(_WIN64)
	return CreateDirectoryW(dirPath.c_str(), NULL) != 0;  // Windows
#else
	return mkdir(dirPath.c_str(), 0777) == 0;  // Linux/macOS
#endif
}

std::vector<std::wstring> getDirectories(const std::wstring& dirPath) {
	std::vector<std::wstring> directories;

#if defined(_WIN32) || defined(_WIN64)
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = FindFirstFileW((dirPath + L"\\*").c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) 
		return directories;  // Invalid directory path

	do {
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			std::wstring dirName = findFileData.cFileName;
			if (dirName != L"." && dirName != L"..") {
				directories.push_back(dirName);
			}
		}
	} while (FindNextFileW(hFind, &findFileData) != 0);

	FindClose(hFind);
#else
	DIR* dir = opendir(dirPath.c_str());
	if (dir == nullptr)
		return directories;  // Invalid directory path

	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr) {
		if (entry->d_type == DT_DIR) {
			std::wstring dirName = entry->d_name;
			if (dirName != L"." && dirName != L"..")
				directories.push_back(dirName);
		}
	}
	closedir(dir);
#endif

	return directories;
}

std::vector<std::wstring> listDirectories(const std::wstring& path) {
	std::vector<std::wstring> directories;

#ifdef _WIN32
	WIN32_FIND_DATAW findData;
	HANDLE handle = FindFirstFileW((path + L"\\*").c_str(), &findData);

	if (handle == INVALID_HANDLE_VALUE) return directories;

	do {
		// Skip "." and ".." entries
		if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			wcscmp(findData.cFileName, L".") != 0 &&
			wcscmp(findData.cFileName, L"..") != 0) {
			directories.emplace_back(findData.cFileName);
		}
	} while (FindNextFileW(handle, &findData) != 0);

	FindClose(handle);
#else
	DIR* dir = opendir(std::string(path.begin(), path.end()).c_str());
	if (dir == nullptr) return directories;

	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr) {
		if (entry->d_type == DT_DIR) {
			std::string name(entry->d_name);
			if (name != "." && name != "..") 
				directories.emplace_back(std::wstring(name.begin(), name.end()));
		}
	}
	closedir(dir);
#endif

	return directories;
}


// Function to remove all files and subdirectories recursively in the specified directory (cross-platform)
bool removeDirectoryRecursive(const std::wstring& dirPath) {
#ifdef _WIN32
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = FindFirstFileW((dirPath + L"\\*").c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		return false; // Directory not found or inaccessible

	do {
		const std::wstring fileOrDirName = findFileData.cFileName;

		if (fileOrDirName != L"." && fileOrDirName != L"..") {
			std::wstring fullPath = dirPath + L"\\" + fileOrDirName;

			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// Recursively remove subdirectory
				removeDirectoryRecursive(fullPath);
				RemoveDirectoryW(fullPath.c_str());
			}
			else 
				DeleteFileW(fullPath.c_str()); // Remove file
		}
	} while (FindNextFileW(hFind, &findFileData) != 0);

	FindClose(hFind);
	return true;
#else
	DIR* dir = opendir(std::string(dirPath.begin(), dirPath.end()).c_str());
	if (dir == nullptr) {
		return false; // Directory not found or inaccessible
	}

	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr) {
		const std::string fileOrDirName = entry->d_name;

		if (fileOrDirName != "." && fileOrDirName != "..") {
			std::string fullPath = std::string(dirPath.begin(), dirPath.end()) + "/" + fileOrDirName;

			if (entry->d_type == DT_DIR) {
				// Recursively remove subdirectory
				removeDirectoryRecursive(std::wstring(fullPath.begin(), fullPath.end()));
				rmdir(fullPath.c_str());
			}
			else
				remove(fullPath.c_str()); // Remove file
		}
	}

	closedir(dir);
	return true;
#endif
}

// Main function to remove a directory and its contents
bool removeDirectory(const std::wstring& dirPath) {
	// Remove all contents inside the directory first and then directory itself
	if (removeDirectoryRecursive(dirPath)) {
#ifdef _WIN32
		return RemoveDirectoryW(dirPath.c_str()) != 0; 
#else
		return rmdir(std::string(dirPath.begin(), dirPath.end()).c_str()) == 0;
#endif
	}
	return false; // Removal of contents failed
}


#if defined(_WIN32) || defined(_WIN64) 

int getch_cross() {
	return _getch();
}

int kbhit_cross() {
	return _kbhit();
}

#else

int getch_cross() {
	struct termios oldt, newt;
	int ch;

	tcgetattr(STDIN_FILENO, &oldt);  // Save current terminal settings
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	ch = getchar();  // Read character

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Restore terminal settings
	return ch;
}

int khbit_cross() {
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if (ch != EOF) {
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
}

#endif