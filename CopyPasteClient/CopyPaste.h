#ifndef COPY_PASTE_H_
#define COPY_PASTE_H_

#pragma region Includes
#include <string>
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#pragma endregion
using namespace std;
bool		SetClipboardText(const std::wstring & data);
wchar_t		*GetClipboardText();
#endif  // COPY_PASTE_H_