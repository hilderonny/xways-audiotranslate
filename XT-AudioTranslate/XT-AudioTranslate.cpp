#include <wtypes.h>
#include <format>
#include "Helper/Helper.h"
#include "TaskBridge/TaskBridge.h"
#include "XWF/XWF.h"
#include "../src/json.hpp"

namespace XTAudioTranslate {

	const std::wstring VERSION = L"1.0.0";

	LONG __stdcall XT_Init(DWORD nVersion, DWORD nFlags, HANDLE hMainWnd, void* lpReserved)
	{
		// Fetch function pointers
		XWF::RetrieveFunctionPointers();

		// Load API URL
		std::string apiUrl = Helper::LoadApiUrl();
		TaskBridge::SetApiUrl(apiUrl);

		// TODO: In settings there should be taskBridgeApiUrl and targetLanguage

		XWF::OutputMessage(std::format(L"Version {}", VERSION));

		return 2; // Mark this extension as thread safe - https://www.x-ways.net/forensics/x-tensions/XT_functions.html#:~:text=not%20thread%2Dsafe.-,0x02,-%3A%20Your%20X%2DTension
	}

	LONG __stdcall XT_About(HANDLE hParentWnd, void* lpReserved)
	{
		// TODO: Show info window with hint to github repository
		XWF::OutputMessage(L"Transcribe and translate media files using TaskBridge (https://github.com/hilderonny/taskbridge)");
		XWF::OutputMessage(L"Detailed information about this extension you can find at https://github.com/hilderonny/xways-audiotranslate");

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
		//XWF::OutputMessage(std::format(L"Item content: {}...", Helper::stringToWstring(base64Content.substr(0, 80))));
		
		// Transcribe item
		XWF::OutputMessage(L"Transcribing ...");
		nlohmann::json transcribeJson = TaskBridge::Transcribe(itemContent);
		nlohmann::json textsToTranslate = {};
		std::string sourceLanguage = transcribeJson["language"];
		std::string originalText = "";
		for (auto& paragraph : transcribeJson["texts"]) {
			std::string text = paragraph["text"];
			if (originalText.size() > 0) {
				originalText += " ";
			}
			originalText += text;
			textsToTranslate.push_back(paragraph["text"]);
		}
		//XWF::OutputMessage(Helper::stringToWstring(transcribeJson.dump()));
		//XWF::OutputMessage(Helper::stringToWstring(sourceLanguage));
		//XWF::OutputMessage(Helper::stringToWstring(originalText));

		// Add language and transcription to metadata
		XWF::AddMetadata(nItemID, std::format(L"[XTAT] audio:translate:language {}", Helper::stringToWstring(sourceLanguage)));
		XWF::AddMetadata(nItemID, std::format(L"[XTAT] audio:translate:original {}", Helper::stringToWstring(originalText)));

		// Translate text
		XWF::OutputMessage(L"Translating ...");
		nlohmann::json translateJson = TaskBridge::Translate(sourceLanguage, textsToTranslate);
		//XWF::OutputMessage(Helper::stringToWstring(translateJson.dump()));

		// Add translation to metadata
		std::string translatedText = "";
		for (auto& paragraph : translateJson["texts"]) {
			std::string text = paragraph["text"];
			if (translatedText.size() > 0) {
				translatedText += " ";
			}
			translatedText += text;
		}
		XWF::AddMetadata(nItemID, std::format(L"[XTAT] audio:translate:de {}", Helper::stringToWstring(translatedText)));
		XWF::OutputMessage(std::format(L"Finished item {}", nItemID));

		return 0;
	}

}