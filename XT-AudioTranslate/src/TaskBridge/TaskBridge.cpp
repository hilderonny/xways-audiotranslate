#include <vector>
#include <string>
#include "Helper/Helper.h"
#include "XWF/XWF.h"
#include "../json.hpp"
#include <format>
#include <thread>
#include <chrono>

namespace XTAudioTranslate {

	namespace TaskBridge {

		std::string currentApiUrl;

		void SetApiUrl(std::string apiUrl) {
			currentApiUrl = apiUrl;
		}
		
		// Transcribe
		nlohmann::json Transcribe(std::string fileContent) {

			// Add task
			std::string addResult = Helper::httpPostFile(currentApiUrl + "tasks/add/", "{\"type\":\"transcribe\"}", fileContent);
			nlohmann::json addJson = nlohmann::json::parse(addResult);
			std::string taskId = addJson["id"];
			XWF::OutputMessage(std::format(L"ID: {}", Helper::stringToWstring(taskId)));

			// Wait for completion
			std::string statusResult, status = "";
			do {
				statusResult = Helper::httpGet(currentApiUrl + "tasks/status/" + taskId);
				nlohmann::json statusJson = nlohmann::json::parse(statusResult);
				status = statusJson["status"];
				if (status != "completed") {
					std::this_thread::sleep_for(std::chrono::seconds(3));
				}
			} while (status != "completed");

			// Read results
			std::string resultStatus = Helper::httpGet(currentApiUrl + "tasks/result/" + taskId);
			nlohmann::json resultJson = nlohmann::json::parse(resultStatus);

			// Delete task
			Helper::httpDelete(currentApiUrl + "tasks/remove/" + taskId);

			return resultJson["result"];
		}

		nlohmann::json Translate(std::string sourceLanguage, nlohmann::json originalTexts) {

			// Prepare JSON
			nlohmann::json jsonRequest;
			jsonRequest["type"] = "translate";
			jsonRequest["data"] = {};
			jsonRequest["data"]["sourcelanguage"] = sourceLanguage;
			jsonRequest["data"]["targetlanguage"] = "de";
			jsonRequest["data"]["texts"] = originalTexts;

			// Add task
			std::string addResult = Helper::httpPost(currentApiUrl + "tasks/add/", jsonRequest.dump());
			nlohmann::json addJson = nlohmann::json::parse(addResult);
			std::string taskId = addJson["id"];
			XWF::OutputMessage(std::format(L"ID: {}", Helper::stringToWstring(taskId)));

			// Wait for completion
			std::string statusResult, status = "";
			do {
				statusResult = Helper::httpGet(currentApiUrl + "tasks/status/" + taskId);
				nlohmann::json statusJson = nlohmann::json::parse(statusResult);
				status = statusJson["status"];
				if (status != "completed") {
					std::this_thread::sleep_for(std::chrono::seconds(3));
				}
			} while (status != "completed");

			// Read results
			std::string resultStatus = Helper::httpGet(currentApiUrl + "tasks/result/" + taskId);
			nlohmann::json resultJson = nlohmann::json::parse(resultStatus);

			// Delete task
			Helper::httpDelete(currentApiUrl + "tasks/remove/" + taskId);

			return resultJson["result"];

		}

	}

}