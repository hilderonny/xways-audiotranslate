#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <windows.h>
#include "Helper/Helper.h"
#include "XWF/XWF.h"

namespace XTAudioTranslate {

	namespace Helper {

		std::string base64Encode(const BYTE* byteData, size_t length) {
			// Base64-Zeichen
			static const char base64_chars[] =
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
				"0123456789+/";

			std::string encodedString;
			size_t i = 0;
			BYTE array3[3];
			BYTE array4[4];

			while (length--) {
				array3[i++] = *(byteData++);
				if (i == 3) {
					array4[0] = (array3[0] & 0xfc) >> 2;
					array4[1] = ((array3[0] & 0x03) << 4) + ((array3[1] & 0xf0) >> 4);
					array4[2] = ((array3[1] & 0x0f) << 2) + ((array3[2] & 0xc0) >> 6);
					array4[3] = array3[2] & 0x3f;

					for (i = 0; i < 4; i++)
						encodedString += base64_chars[array4[i]];
					i = 0;
				}
			}

			if (i) {
				for (size_t j = i; j < 3; j++)
					array3[j] = '\0';

				array4[0] = (array3[0] & 0xfc) >> 2;
				array4[1] = ((array3[0] & 0x03) << 4) + ((array3[1] & 0xf0) >> 4);
				array4[2] = ((array3[1] & 0x0f) << 2) + ((array3[2] & 0xc0) >> 6);
				array4[3] = array3[2] & 0x3f;

				for (size_t j = 0; j < i + 1; j++)
					encodedString += base64_chars[array4[j]];

				while ((i++ < 3))
					encodedString += '=';
			}

			return encodedString;
		}


		// string zu wstring
		std::wstring stringToWstring(std::string str) {
			std::wstringstream cls;
			cls << str.c_str();
			return cls.str();
		}

		// wstring zu string

		// HTTP GET

		// HTTP POST

		// HTTP POST FILE

		// HTTP DELETE

		// Load API URL
		std::string LoadApiUrl() {
			std::string apiUrl;
			std::string settingsFileName = "XTAudioTranslate.cfg";

			char exePath[MAX_PATH];
			GetModuleFileNameA(NULL, exePath, MAX_PATH);
			std::string exeDirectory(exePath);
			exeDirectory = exeDirectory.substr(0, exeDirectory.find_last_of("\\/"));
			std::string absolutePath = exeDirectory + "\\" + settingsFileName;
			std::ifstream settingsFile(settingsFileName);
			if (settingsFile.is_open()) {
				std::getline(settingsFile, apiUrl);
				XWF::OutputMessage(std::format(L"Using API URL \"{}\"", stringToWstring(apiUrl)));
			}
			else {
				apiUrl = "https://taskbridge.glitch.io/api/";
				XWF::OutputMessage(std::format(L"Could not read settings file \"{}\". Using default API URL \"{}\"", stringToWstring(absolutePath), stringToWstring(apiUrl)));
			}
			return apiUrl;
		}

	}

}