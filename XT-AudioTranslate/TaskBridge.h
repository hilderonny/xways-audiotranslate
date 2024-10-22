#pragma once
#include <string>

void sendFile(const char* server, const char* filePath, const char* endpoint);

void getrequest();

std::string httpGET(const std::string& url);

std::string httpGET(const std::string& server, const std::string& path);

std::string translate();