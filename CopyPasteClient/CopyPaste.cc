#include "CopyPaste.h"
#include "stdafx.h"

bool SetClipboardText(const std::wstring & data)
{
	std::cout << "=== Set to clipboard ===" << std::endl;
	if (OpenClipboard(0))
	{
		HGLOBAL hgClipBuffer = nullptr;
		std::size_t sizeInWords = data.size() + 1;
		std::size_t sizeInBytes = sizeInWords * sizeof(wchar_t);
		hgClipBuffer = GlobalAlloc(GHND | GMEM_SHARE, sizeInBytes);
		if (!hgClipBuffer)
		{
			CloseClipboard();
			return false;
		}
		wchar_t *wgClipBoardBuffer = static_cast<wchar_t*>(GlobalLock(hgClipBuffer));
		wcscpy_s(wgClipBoardBuffer, sizeInWords, data.c_str());
		GlobalUnlock(hgClipBuffer);
		EmptyClipboard();
		SetClipboardData(CF_UNICODETEXT, hgClipBuffer);
		CloseClipboard();
		std::wcout << data << std::endl;
		std::cout << "=== ok ===" << std::endl;
	}
	else {
		std::cout << ">> fail <<" << std::endl;
		return false;
	}
	return true;
}

wchar_t *GetClipboardText()
{
	wchar_t      *buffer;
	HANDLE       hData;

	// On ouvre le presse-papier
	if (!OpenClipboard(NULL)) {
		std::cerr << "OpenClipboard fails" << std::endl;
		return (nullptr);
	}

	// On recupère le "handle" du presse-papier en mode texte
	if (!(hData = GetClipboardData(CF_UNICODETEXT))) {
		std::cerr << "GetClipboardData returned null" << std::endl;
		return (nullptr);
	}

	// On récupère via le "handle" le presse-papier
	if ((buffer = (wchar_t *)GlobalLock(hData)) == NULL) {
		std::cerr << "Returned buffer is null" << std::endl;
		return (nullptr);
	}

	// On déverouille le presse-papier
	GlobalUnlock(hData);

	// On ferme le presse-papier
	CloseClipboard();
	std::wcout << buffer << std::endl;
	return buffer;
}