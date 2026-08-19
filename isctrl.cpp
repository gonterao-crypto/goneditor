#include "isctrl.hpp"
#include <windows.h>
bool ispressctrl() {
	return (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
}
bool ispressshift() {
	return (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
}