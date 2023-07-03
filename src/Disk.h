#pragma once
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <sstream>

#include "Help.cpp"

using json = nlohmann::json;

class Disk
{

	std::string GetPath(const std::string& path);
	void PutOnDisk(const std::string& path, const std::string& file);

public:
	void Send(unsigned name, const std::string& img);
	std::string GetURL(unsigned name);
};

