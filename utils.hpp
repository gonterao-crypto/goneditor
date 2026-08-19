#pragma once
#include <string>
#include <cctype>
#include <iostream>

int str_chnum(std::string str, char c) {
    int count = 0;
    
	for (int i = 0; i < str.length(); i++) {
        if (str[i] == c) count++;
	}
    return (count);
}


enum class outcolor {
	black,
	red,
	green,
	yellow,
	blue,
	mazenda,
	syan,
	white,
	def
};



void settextcolor(outcolor color) {
		switch(color) {
		case outcolor::black: {
			std::cout << "\033[30m";
			break;
		}
		case outcolor::red: {
			std::cout << "\033[31m";
			break;
		}
		case outcolor::green: {
			std::cout << "\033[32m";
			break;
		}
		case outcolor::yellow: {
			std::cout << "\033[33m";
			break;
		}
		case outcolor::blue: {
			std::cout << "\033[34m";
			break;
		}
		case outcolor::mazenda: {
			std::cout << "\033[35m";
			break;
		}
		case outcolor::syan: {
			std::cout << "\033[36m";
			break;
		}
		case outcolor::white: {
			std::cout << "\033[37m";
			break;
		}
		case outcolor::def: {
			std::cout << "\033[39m";
			break;
		}
	}
}
void setbgcolor(outcolor color) {
		switch(color) {
		case outcolor::black: {
			std::cout << "\033[40m";
			break;
		}
		case outcolor::red: {
			std::cout << "\033[41m";
			break;
		}
		case outcolor::green: {
			std::cout << "\033[42m";
			break;
		}
		case outcolor::yellow: {
			std::cout << "\033[43m";
			break;
		}
		case outcolor::blue: {
			std::cout << "\033[44m";
			break;
		}
		case outcolor::mazenda: {
			std::cout << "\033[45m";
			break;
		}
		case outcolor::syan: {
			std::cout << "\033[46m";
			break;
		}
		case outcolor::white: {
			std::cout << "\033[47m";
			break;
		}
		case outcolor::def: {
			std::cout << "\033[49m";
			break;
		}
	}
}


extern std::string tolower(const std::string& s) {
    std::string result = s;
    for (char& c : result) {
        if (std::isupper(c)) c = std::tolower(c);
    }
    return result;
}

extern void colorinit() {
	std::cout << "\033[0m\033[39m\033[49m";
}

extern void clear() {
	std::cout << "\033[2J";
}

