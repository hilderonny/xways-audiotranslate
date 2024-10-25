#include <vector>
#include <string>

namespace XTAudioTranslate {

	namespace TaskBridge {

		std::string currentApiUrl;

		void SetApiUrl(std::string apiUrl) {
			currentApiUrl = apiUrl;
		}

	}

}