#pragma once


#include <string>
#include <map>


class IMaterialMetadata
{
public:
	virtual std::string GetMaterialSection() = 0;
	virtual std::map<std::string, std::string> GetMaterialKeyValues() = 0;
};