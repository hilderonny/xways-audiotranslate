#pragma once
#include <vector>
#include <string>
#include "../../src/json.hpp"

namespace XTAudioTranslate {

	namespace TaskBridge {

		void SetApiUrl(std::string apiUrl);

		nlohmann::json Transcribe(std::string fileContent);

		nlohmann::json Translate(std::string sourceLanguage, nlohmann::json originalTexts);

	}

}