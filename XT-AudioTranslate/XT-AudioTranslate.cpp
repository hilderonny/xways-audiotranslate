#include <stdio.h>
#include <cstdio>
#include <nlohmann/json.hpp>
#include "X-Tension.h"
#include "TaskBridge.h"
#include "ApiUrlInput.h"

#define MAX_MSG_LEN 4096

wchar_t buf[MAX_MSG_LEN];
static const wchar_t* XT_NAME = L"[XT_AudioTranslate]"; // Prefix for messages

CHAR API_URL[256];

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
	ShowApiUrlDialog();
	swprintf(buf, MAX_MSG_LEN, L"%ls XT_About", XT_NAME);
	XWF_OutputMessage(buf, 0);
	return 0;
}

std::wstring GetExeFileName()
{
	TCHAR buffer[MAX_PATH] = { 0 };;
	GetModuleFileName(NULL, buffer, MAX_PATH);
	return std::wstring(buffer);
}

LONG __stdcall XT_Prepare(HANDLE hVolume, HANDLE hEvidence, DWORD nOpType, void* lpReserved)
{
	LoadApiUrl(API_URL, 256);

	swprintf(buf, MAX_MSG_LEN, L"%ls XT_Prepare : API-URL: %hs", XT_NAME, API_URL);
	XWF_OutputMessage(buf, 0);

	std::string jsonResponse = httpGET(std::string(API_URL) + "tasks/list/");

	swprintf(buf, MAX_MSG_LEN, L"%ls XT_Prepare : WORKERS=%hs", XT_NAME, jsonResponse.c_str());
	XWF_OutputMessage(buf, 0);

	nlohmann::json parsed = nlohmann::json::parse(jsonResponse);

	swprintf(buf, MAX_MSG_LEN, L"%ls XT_Prepare : WORKERS=%hs", XT_NAME, parsed.dump(4).c_str());
	XWF_OutputMessage(buf, 0);

	std::string translated = translate();

	swprintf(buf, MAX_MSG_LEN, L"%ls XT_Prepare : TRANSLATED=%hs", XT_NAME, translated.c_str());
	XWF_OutputMessage(buf, 0);

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

