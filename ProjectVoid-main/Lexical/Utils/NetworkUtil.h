#pragma once
#include "../Libs/xorstr.hpp"
#include <string>

#define CURL_STATICLIB
#define WEBHOOK_ADDRESS xorstr_("your webhook here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")

namespace NetworkUtil {
	bool canAccessInternet();
	bool downloadFile(std::string name, std::string path, std::string url);
	bool sendWebhook(const char* botName, const char* content, const char* webhookAddr);
	std::string getStrings(const std::string& url);
}
