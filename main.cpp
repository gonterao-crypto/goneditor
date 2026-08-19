#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <string>
#include <vector>
#include <filesystem>
#include <format>
#include <ios>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <regex>

#include "utils.hpp"
#include "config.hpp"
#include "keymap.hpp"
#include "process.hpp"
#include "isctrl.hpp"

#define CR "\n"
const int fileviewwidth = 11;

enum class tabmenu {
	File,
	Edit,
	Run,
	Fileview,
	Code
};

enum class openmenu {
	File,
	Edit,
	Notopen
};

struct fileobj {
	std::string name = "";
	std::string dir = "";
	std::string ext = "";
};

struct state {
	openmenu    open = openmenu::Notopen;
	tabmenu     tab = tabmenu::Fileview;
	int         sc = 1;
	int         scmax = 1;
	int         fileviewscroll = 0;
	int         openfilescroll = 0;
	int         openfilelines = 0;
	int         openfilex = 0;
	int         openfiley = 1;
	bool        is224 = false;
	std::string dir = R"(./testdir)";
	std::string openfile = R"()";
	std::string openfilelinecontents = "";

	std::vector<fileobj> files;
	std::vector<std::string> openfilecontent;
};

state now;

void issc(int sc) {
	if (now.sc == sc) {
		std::cout << "\033[1m";
	} else {
		std::cout << "\033[0m";
	}
}

void notissc(int sc) {
	if (now.sc != sc) {
		std::cout << "\033[1m";
	} else {
		std::cout << "\033[0m";
	}
}

void moveto(int y, int x) {
	std::string a = std::format("\033[{};{}H", y, x);
	std::cout << a;
}

bool ismenu(tabmenu m) {
	if (now.tab == m) {
		return true;
	} else {
		return false;
	}
}

void menusel(tabmenu m) {
	if (ismenu(m)) {
		std::cout << "\033[1m";
	} else {
		std::cout << "\033[0m";
	}
}
void notmenusel(tabmenu m) {
	if (!ismenu(m)) {
		std::cout << "\033[1m";
	} else {
		std::cout << "\033[0m";
	}
}












int filesave(std::string name) {
	std::ofstream out_file{name};
	if (!out_file) {
		return 1;
	}
	for (int i = 0; i < now.openfilecontent.size(); i++) {
		out_file << now.openfilecontent[i] << std::endl;
	}
	out_file.close();
	return 0;
}

int filesave() {
	std::string name = now.dir + "\\" + now.openfile;
	std::ofstream out_file{name};
	if (!out_file) {
		return 1;
	}
	for (int i = 0; i < now.openfilecontent.size(); i++) {
		out_file << now.openfilecontent[i] << std::endl;
	}
	out_file.close();
	return 0;
}

int fileread(std::string filename) {
	std::ifstream file(filename);
	if (!file) return -1;
	now.openfilecontent.clear();
	std::string line;
	int totalline = 0;
	while (getline(file, line)) {
		now.openfilecontent.push_back(line);
		totalline++;
	}
	return totalline;
}

int fileread(std::string filename, int startline, int endline) {
	std::ifstream file(filename);
	if (!file) return -1;
	for (int i = 0; i > startline; i++) {
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	now.openfilecontent.clear();
	for (int i = startline; i > endline; i++) {
		std::string line;
		if (getline(file, line)) {
			now.openfilecontent.push_back(line);
		} else {
			break;
		}
	}
	return 0;
}

void getfiles() {
	for (const std::filesystem::directory_entry &i : std::filesystem::directory_iterator(now.dir)) {
		fileobj tmpf;
		std::string ext = i.path().extension().string();
		
	  	std::transform(
			ext.begin(), 
			ext.end(), 
			ext.begin(),
			[](char c) { return std::tolower(c); }
		);
		tmpf.name = i.path().filename().string();
		tmpf.dir = i.path().string();
		tmpf.ext = tolower(i.path().extension().string());
		now.files.push_back(tmpf);
	}
}














void linecontentsupdate() {
	now.openfilelinecontents = now.openfilecontent[now.openfiley - 1];
}

void enre() {
	std::string adds = "";
	if (now.openfilelinecontents.length() != now.openfilex) {
		adds = now.openfilelinecontents.substr(now.openfilex);
		now.openfilecontent[now.openfiley - 1] = now.openfilelinecontents.substr(0, now.openfilex);
	}
	now.openfilex = 0;
	now.openfiley += 1;
	now.openfilelines += 1;
	now.openfilecontent.insert(now.openfilecontent.begin() + now.openfiley - 1, adds);
	linecontentsupdate();
}

void backspace() {
	if (now.tab == tabmenu::Code && now.openfilecontent.size() > 0) {
		if (now.openfilex > 0) {
			now.openfilecontent[now.openfiley - 1] = now.openfilecontent[now.openfiley - 1].erase(now.openfilex - 1, 1);
			now.openfilex -= 1;
			linecontentsupdate();
		} else if (now.openfiley > 1) {
			std::string buf = now.openfilecontent[now.openfiley - 1];
			now.openfilecontent.erase(now.openfilecontent.begin() + now.openfiley - 1);
			now.openfilelines -= 1;
			now.openfiley -= 1;
			now.openfilex = now.openfilecontent[now.openfiley - 1].length();
			now.openfilecontent[now.openfiley - 1] = now.openfilecontent[now.openfiley - 1] + buf;
			
			linecontentsupdate();
		}
	}
}
void del() {
	if (now.tab == tabmenu::Code && now.openfilecontent.size() > 0) {
		if (now.openfilex < now.openfilelinecontents.length()) {
			now.openfilecontent[now.openfiley - 1] = now.openfilecontent[now.openfiley - 1].erase(now.openfilex, 1);
			linecontentsupdate();
		} else if (now.openfiley < now.openfilecontent.size()) {
			std::string buf = now.openfilecontent[now.openfiley];
			now.openfilecontent.erase(now.openfilecontent.begin() + now.openfiley);
			now.openfilelines -= 1;
			now.openfilecontent[now.openfiley - 1] = now.openfilecontent[now.openfiley - 1] + buf;
			linecontentsupdate();
			
		}
	}
}

void presskey(char key) {
	if (now.tab == tabmenu::Code && now.openfilecontent.size() > 0) {
		now.openfilecontent[now.openfiley - 1].insert(now.openfilex, 1, key);
		now.openfilex += 1;
		linecontentsupdate();
	}
}

void linedown() {
	now.openfiley += 1;
	int tabmaeline = str_chnum(now.openfilelinecontents.substr(0, now.openfilex), '\t');
	linecontentsupdate();
	{
		int tabimaline = str_chnum(now.openfilelinecontents.substr(0, now.openfilex), '\t');
		int sa = tabmaeline - tabimaline;
		now.openfilex += sa * 3;
		std::cout << "\033[17;1H"<< std::dec  << sa;
	}
	if (now.openfilelinecontents.length() < now.openfilex) {
		now.openfilex = now.openfilelinecontents.length();
	}
}
void lineup() {
	now.openfiley -= 1;
	int tabmaeline = str_chnum(now.openfilelinecontents.substr(0, now.openfilex), '\t');
	linecontentsupdate();
	{
		int tabimaline = str_chnum(now.openfilelinecontents.substr(0, now.openfilex), '\t');
		int sa = tabmaeline - tabimaline;
		now.openfilex += sa * 3;
	}
	if (now.openfilelinecontents.length() < now.openfilex) {
		now.openfilex = now.openfilelinecontents.length();
	}
}

void nexttab() {
	switch (now.tab) {
		case tabmenu::File: { now.tab = tabmenu::Edit; break;}
		case tabmenu::Edit: { now.tab = tabmenu::Run; break;}
		case tabmenu::Run: { now.tab = tabmenu::Fileview; now.scmax = now.files.size(); break;}
		case tabmenu::Fileview: { now.tab = tabmenu::Code; break;}
		case tabmenu::Code: { now.tab = tabmenu::File; break;}
		default: {break;}
	}
}

void nowscupdown(bool isup) {
	if (now.tab == tabmenu::Code) {
		if (now.openfiley > 1 && isup) {
			lineup();
		}
		if (now.openfiley < now.openfilelines && !isup) {
			linedown();
		}
	} else if (now.tab == tabmenu::Run) {
	} else if ((now.open == openmenu::Notopen && now.tab == tabmenu::Fileview) || (now.open != openmenu::Notopen && now.tab != tabmenu::Fileview)) {
		if (isup) {
			if (now.sc > 1) now.sc -= 1;
		} else {
			if (now.sc < now.scmax) now.sc += 1;
		}
	}
}

void left() {
	if (now.tab == tabmenu::Code) {
		if (now.openfilex > 0) {
			now.openfilex -= 1;
		}
	}
}
void right() {
	if (now.tab == tabmenu::Code) {
		if (now.openfilex < now.openfilelinecontents.length()) {
			now.openfilex += 1;
		}
	}
}

void home(bool isctrl) {
	if (now.tab == tabmenu::Code) {
		if (isctrl) {
			now.openfilescroll = 0;
			now.openfilex = 0;
			now.openfiley = 1;
		} else {
			now.openfilex = 0;
		}
	} else if (now.tab == tabmenu::Fileview) {
		if (isctrl) {
			now.fileviewscroll = 0;
		}
	}
}






void openclosemenu(tabmenu n) {
	switch (n) {
		case tabmenu::File: {now.open = (now.open == openmenu::Notopen) ? openmenu::File : openmenu::Notopen; now.scmax = 3; break;}
		case tabmenu::Edit: {now.open = (now.open == openmenu::Notopen) ? openmenu::Edit : openmenu::Notopen; now.scmax = 4; break;}
		default: {break;}
	}
}


void drawTopbar() {
	moveto(1, 1);
	menusel(tabmenu::File);
	settextcolor(outcolor::white);
	setbgcolor(outcolor::blue);
	std::cout << "File ";
	menusel(tabmenu::Edit);
	settextcolor(outcolor::blue);
	setbgcolor(outcolor::blue);
	std::cout << "Edit ";
	menusel(tabmenu::Run);
	setbgcolor(outcolor::green);
	std::cout << "Run";
	setbgcolor(outcolor::blue);
	std::cout << "                                                 ";
	std::cout << CR;
}
void drawNidanme() {
	moveto(2, 1);
	notmenusel(tabmenu::Fileview);
	settextcolor(outcolor::black);
	setbgcolor(outcolor::white);
	std::cout << "   File View  |";
	notmenusel(tabmenu::Code);
	settextcolor(outcolor::black);
	setbgcolor(outcolor::white);
	std::cout << std::left << std::setw(80) << now.openfile << std::endl;
}

void drawopenmenuFile() {
	moveto(2, 1);
	issc(1);
	settextcolor(outcolor::white);
	setbgcolor(outcolor::white);
	std::cout << "Open File  ";
	moveto(3, 1);
	issc(2);
	settextcolor(outcolor::white);
	setbgcolor(outcolor::white);
	std::cout << "Open Folder";
	
	moveto(4, 1);
	settextcolor(outcolor::white);
	setbgcolor(outcolor::syan);
	std::cout << "           ";
	
	moveto(5, 1);
	issc(3);
	settextcolor(outcolor::white);
	setbgcolor(outcolor::syan);
	std::cout << "Save File  ";
}
void drawopenmenuEdit() {
	moveto(2, 5);
	issc(1);
	settextcolor(outcolor::white);
	setbgcolor(outcolor::white);
	std::cout << "Undo ";
	moveto(3, 5);
	issc(2);
	settextcolor(outcolor::white);
	setbgcolor(outcolor::white);
	std::cout << "Redo ";
	
	moveto(4, 5);
	issc(3);
	settextcolor(outcolor::white);
	setbgcolor(outcolor::white);
	std::cout << "Copy ";
	
	moveto(5, 5);
	issc(4);
	settextcolor(outcolor::white);
	setbgcolor(outcolor::white);
	std::cout << "Paste";
}
void draws(int n) {
	moveto(n, 1);
	int index = n - 3;
	int fileviewindex = index + now.fileviewscroll;
	colorinit();
	if (fileviewindex < now.files.size()) {
		fileobj ff = now.files[fileviewindex];
		if (now.open == openmenu::Notopen) {
			issc(fileviewindex + 1);
		}
		settextcolor(outcolor::white);
		setbgcolor(getlaungecolor(ff.ext));
		std::cout << "[" << getlaungekasiramoji(ff.ext) << "]";
		
		
		
		std::cout << "\033[1m";
		if (now.open == openmenu::Notopen) {
			notissc(fileviewindex + 1);
		}
		settextcolor(outcolor::black);
		setbgcolor(outcolor::white);
		if (ff.name.length() > fileviewwidth) {
			std::string ft = ff.name.substr(0, fileviewwidth - 3) + "...";
			std::cout << ft;
		} else {
			std::cout << std::left << std::setw(fileviewwidth) << ff.name;
		}

	} else {
		settextcolor(outcolor::black);
		setbgcolor(outcolor::white);
		std::cout << "   ";
		std::cout << std::left << std::setw(fileviewwidth) << "";
	}
	
	colorinit();
	settextcolor(outcolor::black);
	setbgcolor(outcolor::white);
	
	std::cout << "|";
	colorinit();
	std::cout << "\033[0K";
	
	if ((now.openfilescroll + index) < now.openfilecontent.size()) {
		std::string linecontent = now.openfilecontent[now.openfilescroll + index];
		settextcolor(outcolor::white);
		setbgcolor(outcolor::black);
		if (linecontent.find(BLUECODE) != std::string::npos) {
			setbgcolor(outcolor::blue);
		}
		if (linecontent.find(REDCODE) != std::string::npos) {
			setbgcolor(outcolor::red);
		}
		if (linecontent.find(GREENCODE) != std::string::npos) {
			setbgcolor(outcolor::green);
		}
		if (linecontent.find(YELLOWCODE) != std::string::npos) {
			setbgcolor(outcolor::yellow);
		}
		if (linecontent.find(SYANCODE) != std::string::npos) {
			setbgcolor(outcolor::syan);
		}
		
		
		std::cout << std::dec << std::right << std::setw(linenumketasu) << (now.openfilescroll + index + 1);
		
		
		settextcolor(outcolor::white);
		setbgcolor(outcolor::black);
		std::cout << "|";
		
		settextcolor(outcolor::white);
		setbgcolor(outcolor::black);
		
		std::string lineout = linecontent;
		int tabukazu = str_chnum(now.openfilelinecontents.substr(0, now.openfilex), '\t');
		int nowcursordrawx = tabukazu * 4 + (now.openfilex - tabukazu);
		lineout = std::regex_replace(lineout, std::regex("\t"), "    ");
		
		if (lineout.length() >= nowcursordrawx && 0 <= nowcursordrawx) {
			if (now.openfilescroll + index + 1 == now.openfiley) {
				lineout.insert(nowcursordrawx, CURSORCOLORCODE + "|" + CURSORNOMALCODE);
			} else {
				lineout.insert(nowcursordrawx, " ");
			}
			std::cout<< std::left << std::setw(80) << lineout;
		} else {
			std::cout<< std::left << std::setw(80) << lineout;
		}
	} else {
		settextcolor(outcolor::syan);
		setbgcolor(outcolor::black);
		std::cout << std::dec << std::right << std::setw(linenumketasu) << (now.openfilescroll + index + 1);
		settextcolor(outcolor::white);
		setbgcolor(outcolor::black);
		std::cout << "|";
	}
}

void overdraw() {
	if (now.open == openmenu::File) drawopenmenuFile();
	if (now.open == openmenu::Edit) drawopenmenuEdit();
}

void draw() {
	drawTopbar();
	drawNidanme();
	std::cout << std::flush;
}

void draw(int n) {
	switch (n) {
		case 1: {drawTopbar(); break;}
		case 2: {drawNidanme(); break;}
		default: {draws(n); break;}
	}
}

void Tentermenu() {
	if (now.tab == tabmenu::File) {
		if (now.open != openmenu::Notopen) {
			if (now.sc == 3) {
				int sd = filesave(now.dir + "\\" + now.openfile);
				std::cout << "\033[17;1H"<< std::dec  << sd;
			}
		} else {
			openclosemenu(now.tab);
		}
	} else if (now.tab == tabmenu::Edit) {
		openclosemenu(now.tab);
	} else if (now.tab == tabmenu::Fileview) {
		now.openfile = now.files[now.sc - 1].name;
		int k = fileread(now.files[now.sc - 1].dir);
		if (k != -1) {
			now.openfilelines = k;
			linecontentsupdate();
			if (now.openfilelinecontents.length() < now.openfilex) {
				now.openfilex = now.openfilelinecontents.length();
			}
		}
	} else if (now.tab == tabmenu::Run) {
		compile(now.dir);
	} else if (now.tab == tabmenu::Code) {
		enre();
	}
}

int main(void) {
	/*
	setbgcolor(outcolor::blue);
	settextcolor(outcolor::red);
	std::cout << "awawa" << std::endl;
	setbgcolor(outcolor::def);
	settextcolor(outcolor::syan);
	std::cout << "piepie" << std::endl;
	*/
	getfiles();
	if (now.tab == tabmenu::Fileview) {
		now.scmax = now.files.size();
	}
	clear();
	colorinit();
	draw();
	std::cout << std::endl;
	int input = 0;
	while (true) {
		draw(3);
		draw(4);
		draw(5);
		draw(6);
		draw(7);
		draw(8);
		draw(9);
		draw(10);
		draw(11);
		draw(12);
		draw(13);
		draw(14);
		draw(15);
		draw(16);
		draw(17);
		draw(18);
		draw(19);
		draw(20);
		draw(21);
		overdraw();
		input = _getch();
		std::cout << "\033[22;1H" << (now.is224 ? "224-" : "0-") << std::hex << input;
		if (input == 0x00) {now.is224 = false;}
		else if (input == 224)  {now.is224 = true;}
		else if (input == 0x1b) {now.is224 = false; break;}
		else if (input == 0x08 && !now.is224) { now.is224 = false;
			backspace();	
		} else if (input == 0x53 && now.is224) { now.is224 = false;
			del();
		} else if (input == 0x09 && !now.is224) { now.is224 = false;
			if (ispressshift()) {
				now.open = openmenu::Notopen;
				nexttab();
				draw();
				now.sc = 1;
			} else {
				if (now.tab == tabmenu::Code) presskey((char)'\t');
			}
		} else if (input == 0x0d && !now.is224) { now.is224 = false;
			Tentermenu();
			draw();
			overdraw();
		} else if (input == 0x48 && now.is224) { now.is224 = false;
			nowscupdown(true);
			draw();
		} else if (input == 0x50 && now.is224) { now.is224 = false;
			nowscupdown(false);
			draw();
		} else if (input == 0x4b && now.is224) { now.is224 = false;
			left();
		} else if (input == 0x4d && now.is224) { now.is224 = false;
			right();
		} else if (input == 0x51 && now.is224) { now.is224 = false;
			if (now.tab == tabmenu::Fileview) now.fileviewscroll += 1;
			else now.openfilescroll += 1;
		} else if (input == 0x49 && now.is224) { now.is224 = false;
			if (now.tab == tabmenu::Fileview) now.fileviewscroll -= 1;
			else now.openfilescroll -= 1;
		} else if (input == 0x13 && !now.is224) { now.is224 = false;
			filesave();
		} else if (input == 0x77 && now.is224) { now.is224 = false;
			home(true);
		} else if (input == 0x47 && now.is224) { now.is224 = false;
			home(false);
		} else {
			char c = map(input, now.is224);
			std::cout << "\033[16;1H" << c;
			now.is224 = false;
			if (c != 0x00) {
				presskey(c);
			}
		}

	}
	colorinit();
	
 
    return 0;
}