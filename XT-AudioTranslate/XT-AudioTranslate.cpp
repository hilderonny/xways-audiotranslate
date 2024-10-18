///////////////////////////////////////////////////////////////////////////////
// X-Tension API - template for new X-Tensions
// Copyright X-Ways Software Technology AG
///////////////////////////////////////////////////////////////////////////////

#include "X-Tension.h"

// Please consult
// http://x-ways.com/forensics/x-tensions/api.html
// for current documentation

///////////////////////////////////////////////////////////////////////////////
// XT_Init

LONG __stdcall XT_Init(DWORD nVersion, DWORD nFlags, HANDLE hMainWnd,
	void* lpReserved)
{
	XT_RetrieveFunctionPointers();
	XWF_OutputMessage(L"[XT_AudioTranslate] XT_Init", 0);
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// XT_Done

LONG __stdcall XT_Done(void* lpReserved)
{
	XWF_OutputMessage(L"[XT_AudioTranslate] XT_Done", 0);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// XT_About

LONG __stdcall XT_About(HANDLE hParentWnd, void* lpReserved)
{
	XWF_OutputMessage(L"[XT_AudioTranslate] XT_About", 0);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// XT_Prepare

LONG __stdcall XT_Prepare(HANDLE hVolume, HANDLE hEvidence, DWORD nOpType, void* lpReserved)
{
	XWF_OutputMessage (L"[XT_AudioTranslate] XT_Prepare", 0);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// XT_Finalize

LONG __stdcall XT_Finalize(HANDLE hVolume, HANDLE hEvidence, DWORD nOpType, void* lpReserved)
{
	XWF_OutputMessage (L"[XT_AudioTranslate] XT_Finalize", 0);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// XT_ProcessItem

LONG __stdcall XT_ProcessItem(LONG nItemID, void* lpReserved)
{
	XWF_OutputMessage(L"[XT_AudioTranslate] XT_ProcessItem", 0);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// XT_ProcessItemEx

LONG __stdcall XT_ProcessItemEx(LONG nItemID, HANDLE hItem, void* lpReserved)
{
	XWF_OutputMessage(L"[XT_AudioTranslate] XT_ProcessItemEx", 0);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// XT_ProcessSearchHit

LONG __stdcall XT_ProcessSearchHit(struct SearchHitInfo* info)
{
	XWF_OutputMessage(L"[XT_AudioTranslate] XT_ProcessSearchHit", 0);
	return 0;
}

