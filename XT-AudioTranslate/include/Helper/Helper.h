#pragma once
#include <string>

namespace XTAudioTranslate {

	namespace Helper {

		std::string base64Encode(const BYTE* byteData, size_t length);

		// string zu wstring
		std::wstring stringToWstring(std::string str);

		// wstring zu string

		// HTTP GET

		// HTTP POST

		// HTTP POST FILE

		// HTTP DELETE

		// Load API URL
		std::string LoadApiUrl();

	}

}