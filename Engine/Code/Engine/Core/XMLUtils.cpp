#include "Engine/Core/XMLUtils.hpp"
#include "ThirdParty/XML/xml.hpp"

#ifndef __USING_UWP
//-----------------------------------------------------------------------------------------------
XMLNode XMLUtils::GetRootNode(const std::string& filepath)
{
	return XMLNode::parseFile(filepath.c_str()).getChildNode();
}


//-----------------------------------------------------------------------------------------------
std::string XMLUtils::GetAttribute(const struct XMLNode& node, const std::string& attributeName)
{
	const char* cResult = node.getAttribute(attributeName.c_str());
	if (!cResult)
	{
		return "";
	}
	else
	{
		return std::string(cResult);
	}
}


//-----------------------------------------------------------------------------------------------
std::string XMLUtils::GetName(const struct XMLNode& node)
{
	return std::string(node.getName());
}

#endif