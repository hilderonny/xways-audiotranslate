#include <stdio.h>
#include "X-Tension.h"

#define MAX_MSG_LEN 128

wchar_t buf[MAX_MSG_LEN];
static const wchar_t* XT_NAME = L"[XT_AudioTranslate]"; // Prefix for messages

// http://www.x-ways.net/forensics/x-tensions/XWF_functions.html#E

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
	swprintf(buf, MAX_MSG_LEN, L"%ls XT_Prepare", XT_NAME);
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

