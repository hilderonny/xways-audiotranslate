#include <wtypes.h>
#include <format>
#include "Helper/Helper.h"
#include "TaskBridge/TaskBridge.h"
#include "XWF/XWF.h"

namespace XTAudioTranslate {

	LONG __stdcall XT_Init(DWORD nVersion, DWORD nFlags, HANDLE hMainWnd, void* lpReserved)
	{
		// Fetch function pointers
		XWF::RetrieveFunctionPointers();

		// Load API URL
		std::string apiUrl = Helper::LoadApiUrl();
		TaskBridge::SetApiUrl(apiUrl);

		// TODO: In settings there should be taskBridgeApiUrl and targetLanguage

		return 2; // Mark this extension as thread safe - https://www.x-ways.net/forensics/x-tensions/XT_functions.html#:~:text=not%20thread%2Dsafe.-,0x02,-%3A%20Your%20X%2DTension
	}

	LONG __stdcall XT_About(HANDLE hParentWnd, void* lpReserved)
	{
		// TODO: Show info window with hint to github repository
		XWF::OutputMessage(L"XT_About");

		return 0;
	}

	LONG __stdcall XT_Prepare(HANDLE hVolume, HANDLE hEvidence, DWORD nOpType, void* lpReserved)
	{
		// Remember current volume
		XWF::SetCurrentVolume(hVolume);

		return 0;
	}

	LONG __stdcall XT_ProcessItemEx(LONG nItemID, HANDLE hItem, void* lpReserved)
	{

		XWF::OutputMessage(std::format(L"Processing item {}", nItemID));

		// Read item content
		std::string itemContent = XWF::ReadItemContent(nItemID);
		std::string base64Content = Helper::base64Encode(reinterpret_cast<const BYTE*>(itemContent.data()), itemContent.size());
		XWF::OutputMessage(std::format(L"Item content: {}...", Helper::stringToWstring(base64Content.substr(0, 80))));
		
		// TODO: Transcribe item

		// TODO: Add language, transcription and possible error to metadata
		
		// TODO: Translate text

		// TODO: Add translation and possible error to metadata


		//const wchar_t* name = XWF_GetItemName(nItemID);
		//swprintf(buf, MAX_MSG_LEN, L"%ls XT_ProcessItemEx - %ls", XT_NAME, name);
		//XWF_OutputMessage(buf, 0);

		//swprintf(buf, MAX_MSG_LEN, L"[XTAT] audio:tranlate:original %ls", name);
		//XWF_AddExtractedMetadata(nItemID, buf, 0x02);



		//LoadApiUrl(API_URL, 256);

		//swprintf(buf, MAX_MSG_LEN, L"%ls XT_Prepare : API-URL: %hs", XT_NAME, API_URL);
		//XWF_OutputMessage(buf, 0);

		//std::string jsonResponse = httpGET(std::string(API_URL) + "tasks/list/");
		//nlohmann::json parsed = nlohmann::json::parse(jsonResponse);

		//swprintf(buf, MAX_MSG_LEN, L"%ls XT_Prepare : WORKERS=%hs", XT_NAME, parsed.dump(4).c_str());
		//XWF_OutputMessage(buf, 0);

		//std::string translated = translate();

		//swprintf(buf, MAX_MSG_LEN, L"%ls XT_Prepare : TRANSLATED=%hs", XT_NAME, translated.c_str());
		//XWF_OutputMessage(buf, 0);

		return 0;
	}

}