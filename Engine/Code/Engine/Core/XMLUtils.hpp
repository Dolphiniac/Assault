#pragma once

#include "ThirdParty/XML/xml.hpp"

#include <string>
#ifndef __USING_UWP

//-----------------------------------------------------------------------------------------------
class XMLUtils
{
public:
	static struct XMLNode GetRootNode(const std::string& filepath);
	static std::string GetAttribute(const struct XMLNode& node, const std::string& attributeName);
	static std::string GetName(const struct XMLNode& node);
};
#endif