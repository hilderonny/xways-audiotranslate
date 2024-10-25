#include <format>
#include <string>
#include <wtypes.h>
#include "XWF/XWF.h"

// https://www.x-ways.net/forensics/x-tensions/XT_functions.html
// http://www.x-ways.net/forensics/x-tensions/XWF_functions.html

namespace XTAudioTranslate {

	namespace XWF {

		fptr_XWF_Close XWF_Close;
		fptr_XWF_GetProp XWF_GetProp;
		fptr_XWF_GetSize XWF_GetSize;
		fptr_XWF_OpenItem XWF_OpenItem;
		fptr_XWF_OutputMessage XWF_OutputMessage;
		fptr_XWF_Read XWF_Read;

		void* currentVolumeHandle;

		void __stdcall RetrieveFunctionPointers() {
			HMODULE Hdl = GetModuleHandle(NULL);
			XWF_Close = (fptr_XWF_Close)GetProcAddress(Hdl, "XWF_Close");
			XWF_GetProp = (fptr_XWF_GetProp)GetProcAddress(Hdl, "XWF_GetProp");
			XWF_GetSize = (fptr_XWF_GetSize)GetProcAddress(Hdl, "XWF_GetSize");
			XWF_OpenItem = (fptr_XWF_OpenItem)GetProcAddress(Hdl, "XWF_OpenItem");
			XWF_OutputMessage = (fptr_XWF_OutputMessage)GetProcAddress(Hdl, "XWF_OutputMessage");
			XWF_Read = (fptr_XWF_Read)GetProcAddress(Hdl, "XWF_Read");
		}

		void OutputMessage(std::wstring message) {
			XWF_OutputMessage(std::format(L"[XTAudioTranslate] {}", message).c_str(), 0);
		}

		std::string ReadItemContent(long nItemId) {
			HANDLE hItem = XWF_OpenItem(currentVolumeHandle, nItemId, 1);
			OutputMessage(std::format(L"Item handle {}", hItem));
			INT64 size = XWF_GetProp(hItem, 1, NULL);
			OutputMessage(std::format(L"Size {}", size));
			std::string buffer(size, '\0');
			DWORD bytesRead = XWF_Read(hItem, 0, reinterpret_cast<BYTE*>(&buffer[0]), (DWORD)size);
			OutputMessage(std::format(L"Bytes read {}", bytesRead));
			XWF_Close(hItem);
			return buffer;
		}

		void SetCurrentVolume(void* volumeHandle) {
			currentVolumeHandle = volumeHandle;
			OutputMessage(std::format(L"Using volume {}", volumeHandle));
		}

	}

}