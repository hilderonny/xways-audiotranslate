#pragma once
#include <string>

void sendFile(const char* server, const char* filePath, const char* endpoint);

std::string httpGET(const std::string& url);

std::string translate();