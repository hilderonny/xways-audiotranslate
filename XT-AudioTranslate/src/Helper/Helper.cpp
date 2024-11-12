//using namespace std;

#include <winsock2.h>
#include <ws2tcpip.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <windows.h>
#include "Helper/Helper.h"
#include "XWF/XWF.h"
#include <regex>
#include <unordered_map>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

namespace XTAudioTranslate {

	namespace Helper {

		void parseURL(const std::string& url, std::string& host, std::string& path, std::string& port) {
			// Standardwerte setzen
			port = "80"; // Standard HTTP-Port
			path = "/";  // Standardpfad, falls keiner angegeben ist

			// Regulärer Ausdruck für URL-Zerlegung
			std::regex urlRegex(R"(^(\w+):\/\/([^\/:]+)(?::(\d+))?(\/.*)?$)");
			std::smatch urlMatch;

			if (std::regex_match(url, urlMatch, urlRegex)) {
				host = urlMatch[2].str(); // Host aus dem URL extrahieren

				// Optionaler Port, wenn vorhanden
				if (urlMatch[3].matched) {
					port = urlMatch[3].str();
				}

				// Optionaler Pfad, wenn vorhanden
				if (urlMatch[4].matched) {
					path = urlMatch[4].str();
				}
			}
		}

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

		// Implementierungshilfe von ChatGPT: 
		// https://chatgpt.com/share/6732fef9-0f54-8005-a515-c0109b2496ae
		// https://chatgpt.com/c/671a0d4e-0540-8005-80a0-2844059ee707
		// https://chatgpt.com/share/67330fd5-f738-8005-8894-5ca3cf400bb1

		// string zu wstring
		std::wstring stringToWstring(const std::string& str) {
			return std::wstring(str.begin(), str.end());
		}


		static std::string sendRequest(const std::string& host, const std::string& port, const std::string& request) {
			WSADATA wsaData;
			SOCKET sock = INVALID_SOCKET;
			struct addrinfo* result = NULL, * ptr = NULL, hints;
			std::string response;

			// Winsock initialisieren
			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
				XWF::OutputMessage(L"ERROR in Helper::sendRequest: WSAStartup failed.");
				return "";
			}

			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

			// Adresse auflösen
			if (getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0) {
				XWF::OutputMessage(L"ERROR in Helper::sendRequest: getaddrinfo failed.");
				WSACleanup();
				return "";
			}

			// Socket erstellen
			sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			if (sock == INVALID_SOCKET) {
				XWF::OutputMessage(L"ERROR in Helper::sendRequest: Socket creation failed.");
				freeaddrinfo(result);
				WSACleanup();
				return "";
			}

			int timeout = 15000; // Timeout in Millisekunden
			setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

			// Mit dem Server verbinden
			if (connect(sock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
				XWF::OutputMessage(L"ERROR in Helper::sendRequest: Connection failed.");
				closesocket(sock);
				freeaddrinfo(result);
				WSACleanup();
				return "";
			}

			// Anfrage senden
			send(sock, request.c_str(), (int)request.length(), 0);

			// Antwort empfangen
			char buffer[4096];
			int bytesReceived;
			bool headerParsed = false;

			while ((bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
				buffer[bytesReceived] = '\0'; // Null-Terminator für den String
				response += buffer;

				// Header überspringen
				if (!headerParsed) {
					size_t headerEnd = response.find("\r\n\r\n");
					if (headerEnd != std::string::npos) {
						response = response.substr(headerEnd + 4); // Entferne den Header
						headerParsed = true;
					}
				}
			}

			if (bytesReceived == SOCKET_ERROR) {
				XWF::OutputMessage(L"ERROR in Helper::sendRequest: recv failed.");
				std::cerr << "recv failed." << std::endl;
			}

			// Aufräumen
			closesocket(sock);
			freeaddrinfo(result);
			WSACleanup();

			return response;
		}


		// HTTP GET
		std::string httpGet(const std::string& url) {

			// URL parsen, um Host, Pfad und Port zu extrahieren
			std::string host, path, port;
			parseURL(url, host, path, port);

			// HTTP GET-Anfrage erstellen
			std::string request = "GET " + path + " HTTP/1.1\r\n";
			request += "Host: " + host + "\r\n";
			request += "Connection: close\r\n\r\n"; // Verbindung schließen nach der Antwort

			std::string response = sendRequest(host, port, request);
			return response;

		}

		// HTTP POST
		std::string httpPost(const std::string& url, const std::string& json) {

			// URL parsen, um Host, Pfad und Port zu extrahieren
			std::string host, path, port;
			parseURL(url, host, path, port);

			// Request-Inhalt zusammenstellen
			const std::string boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
			std::string postBody = "--" + boundary + "\r\n";
			postBody += "Content-Disposition: form-data; name=\"json\"\r\n\r\n";
			postBody += json + "\r\n";
			postBody += "--" + boundary + "--\r\n";

			int contentLength = (int)postBody.size();
			// Anfrage erstellen und senden
			std::string request = "POST " + path + " HTTP/1.1\r\n";
			request += "Host: " + host + "\r\n";
			request += "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n";
			request += "Content-Length: " + std::to_string(contentLength) + "\r\n";
			request += "\r\n";
			request += postBody;

			std::string response = sendRequest(host, port, request);
			return response;

		}

		// HTTP POST FILE
		std::string httpPostFile(const std::string& url, const std::string& json, const std::string& fileContent) {

			// URL parsen, um Host, Pfad und Port zu extrahieren
			std::string host, path, port;
			parseURL(url, host, path, port);

			// Request-Inhalt zusammenstellen
			const std::string boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
			std::string postBody = "--" + boundary + "\r\n";
			postBody += "Content-Disposition: form-data; name=\"file\"; filename=\"filename\"\r\n";
			postBody += "Content-Type: application/octet-stream\r\n\r\n";
			postBody += fileContent;
			postBody += "\r\n--" + boundary + "\r\n";
			postBody += "Content-Disposition: form-data; name=\"json\"\r\n\r\n";
			postBody += json + "\r\n";
			postBody += "--" + boundary + "--\r\n";

			int contentLength = (int)postBody.size();
			// Anfrage erstellen und senden
			std::string request = "POST " + path + " HTTP/1.1\r\n";
			request += "Host: " + host + "\r\n";
			request += "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n";
			request += "Content-Length: " + std::to_string(contentLength) + "\r\n";
			request += "\r\n";
			request += postBody;

			std::string response = sendRequest(host, port, request);
			return response;

		}

		// HTTP DELETE
		std::string httpDelete(const std::string& url) {

			// URL parsen, um Host, Pfad und Port zu extrahieren
			std::string host, path, port;
			parseURL(url, host, path, port);

			// Anfrage erstellen und senden
			std::string request = "DELETE " + path + " HTTP/1.1\r\n";
			request += "Host: " + host + "\r\n";
			request += "Connection: close\r\n\r\n";

			std::string response = sendRequest(host, port, request);
			return response;

		}

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