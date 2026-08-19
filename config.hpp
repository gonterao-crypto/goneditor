#pragma once
#include <string>
#include "utils.hpp"

int linenumketasu = 5;
std::string BLUECODE = "EBLUECODE";
std::string REDCODE = "EREDCODE";
std::string GREENCODE = "EGREENCODE";
std::string YELLOWCODE = "EYELLOWCODE";
std::string SYANCODE = "ESYANCODE";
std::string CURSORCOLORCODE = "\033[36m";
std::string CURSORNOMALCODE = "\033[37m";

extern outcolor getlaungecolor(std::string ext) {
	     if (ext == ".cpp") return outcolor::blue;
	else if (ext == ".hpp") return outcolor::blue;
	else if (ext == ".h")   return outcolor::black;
	else if (ext == ".c")   return outcolor::black;
	else if (ext == ".ixx") return outcolor::mazenda;
	else if (ext == ".ipp") return outcolor::mazenda;
	else if (ext == ".cppm")return outcolor::mazenda;
	else if (ext == ".txt") return outcolor::black;
	else if (ext == ".md")  return outcolor::green;
	else                    return outcolor::def;
}

extern char getlaungekasiramoji(std::string ext) {
	     if (ext == ".cpp") return 'C';
	else if (ext == ".hpp") return 'H';
	else if (ext == ".h")   return 'H';
	else if (ext == ".c")   return 'C';
	else if (ext == ".ixx") return 'M';
	else if (ext == ".ipp") return 'M';
	else if (ext == ".txt") return 'T';
	else if (ext == ".md")  return 'd';
	else                    return '?';
}