#pragma once
#include <string>

namespace XTAudioTranslate {

	namespace XWF {

		typedef void(__stdcall* fptr_XWF_Close)(HANDLE hVolumeOrItem);
		extern fptr_XWF_Close XWF_Close;

		typedef INT64(__stdcall* fptr_XWF_GetProp)(HANDLE hVolumeOrItem, DWORD nPropType, void* lpBuffer);
		extern fptr_XWF_GetProp XWF_GetProp;

		typedef INT64(__stdcall* fptr_XWF_GetSize) (HANDLE hVolumeOrItem, LPVOID lpOptional);
		extern fptr_XWF_GetSize XWF_GetSize;

		typedef HANDLE(__stdcall* fptr_XWF_OpenItem) (HANDLE hVolume, LONG nItemID, DWORD nFlags);
		extern fptr_XWF_OpenItem XWF_OpenItem;

		typedef void(__stdcall* fptr_XWF_OutputMessage) (const wchar_t* lpMessage, DWORD nFlags);
		extern fptr_XWF_OutputMessage XWF_OutputMessage;

		typedef DWORD(__stdcall* fptr_XWF_Read) (HANDLE hVolumeOrItem, INT64 nOffset, BYTE* lpBuffer, DWORD nNumberOfBytesToRead);
		extern fptr_XWF_Read XWF_Read;

		void __stdcall RetrieveFunctionPointers();

		// Message output
		void OutputMessage(std::wstring message);

		// Read Item content
		std::string ReadItemContent(long nItemId);

		// Set current volume
		void SetCurrentVolume(void* volumeHandle);

		// Add Metadata
		//void AddMetadata(long nItemID, std::wstring message);

		// Show Progress

		// Show alert

	}

}