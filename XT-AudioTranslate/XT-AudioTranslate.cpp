#include <stdio.h>
#include "X-Tension.h"
#include <cstdio>

#define MAX_MSG_LEN 128

wchar_t buf[MAX_MSG_LEN];
static const wchar_t* XT_NAME = L"[XT_AudioTranslate]"; // Prefix for messages

char API_URL[256];

// http://www.x-ways.net/forensics/x-tensions/XWF_functions.html#E

// https://chatgpt.com/share/67123ee9-22c0-8005-9104-50ceaff5d606

LONG __stdcall XT_Init(DWORD nVersion, DWORD nFlags, HANDLE hMainWnd, void* lpReserved)
{
	XT_RetrieveFunctionPointers();
	swprintf(buf, MAX_MSG_LEN, L"%ls XT_Init", XT_NAME);
	XWF_OutputMessage(buf, 0);
	return 1;
}

LONG __stdcall XT_Done(void* lpReserved)
{
	swprintf(buf, MAX_MSG_LEN, L"%ls XT_Done", XT_NAME);
	XWF_OutputMessage(buf, 0);
	return 0;
}

LONG __stdcall XT_About(HANDLE hParentWnd, void* lpReserved)
{
	swprintf(buf, MAX_MSG_LEN, L"%ls XT_About", XT_NAME);
	XWF_OutputMessage(buf, 0);
	return 0;
}

LONG __stdcall XT_Prepare(HANDLE hVolume, HANDLE hEvidence, DWORD nOpType, void* lpReserved)
{
	FILE* file = nullptr; // Dateizeiger initialisieren
	errno_t err = fopen_s(&file, "XT-AudioTranslate.cfg", "r"); // Datei im Lese-Modus ˆffnen

	if (err != 0 || file == nullptr) { // ‹berpr¸fen, ob die Datei geˆffnet werden konnte
		swprintf(buf, MAX_MSG_LEN, L"%ls XT_Prepare : Fehler beim Lesen von XT-AudioTranslate.cfg", XT_NAME);
		XWF_OutputMessage(buf, 0);
		return 1;
	}

	if (fgets(API_URL, sizeof(API_URL), file) != nullptr) { // Eine Zeile lesen
		size_t len = strlen(API_URL);
		if (len > 0 && API_URL[len - 1] == '\n') {
			API_URL[len - 1] = '\0'; // Ersetzen durch Nullterminator
		}
		swprintf(buf, MAX_MSG_LEN, L"%ls XT_Prepare : API-URL: %hs", XT_NAME, API_URL);
		XWF_OutputMessage(buf, 0);
	}
	fclose(file); // Datei schlieﬂen
	return 0;
}

LONG __stdcall XT_Finalize(HANDLE hVolume, HANDLE hEvidence, DWORD nOpType, void* lpReserved)
{
	swprintf(buf, MAX_MSG_LEN, L"%ls XT_Finalize", XT_NAME);
	XWF_OutputMessage(buf, 0);
	return 0;
}

LONG __stdcall XT_ProcessItemEx(LONG nItemID, HANDLE hItem, void* lpReserved)
{
	const wchar_t* name = XWF_GetItemName(nItemID);
	swprintf(buf, MAX_MSG_LEN, L"%ls XT_ProcessItemEx - %ls", XT_NAME, name);
	XWF_OutputMessage(buf, 0);

	swprintf(buf, MAX_MSG_LEN, L"[XTAT] audio:tranlate:original %ls", name);
	XWF_AddExtractedMetadata(nItemID, buf, 0x02);
	return 0;
}

LONG __stdcall XT_ProcessSearchHit(struct SearchHitInfo* info)
{
	swprintf(buf, MAX_MSG_LEN, L"%ls XT_ProcessSearchHit", XT_NAME);
	XWF_OutputMessage(buf, 0);
	return 0;
}

