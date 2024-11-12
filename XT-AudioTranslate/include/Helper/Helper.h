#pragma once
#include <string>
#include <windows.h>
#include <unordered_map>

namespace XTAudioTranslate {

	namespace Helper {

		std::string base64Encode(const BYTE* byteData, size_t length);

		// string zu wstring
		std::wstring stringToWstring(const std::string& str);

		// HTTP GET
		std::string httpGet(const std::string& url);

		// TODO: HTTP POST
		std::string httpPost(const std::string& url, const std::string& json);

		// HTTP POST FILE
		std::string httpPostFile(const std::string& url, const std::string& json, const std::string& fileContent);

		// HTTP DELETE
		std::string httpDelete(const std::string& url);

		// Load API URL
		std::string LoadApiUrl();

	}

}