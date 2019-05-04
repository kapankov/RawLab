// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "stdafx.h"

std::unique_ptr<xmlChar[]> wstringToXmlChar(std::wstring w)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> cnv;
	std::string s = cnv.to_bytes(w);
	std::unique_ptr<xmlChar[]> r = std::make_unique<xmlChar[]>(s.length() + 1);
	memcpy(r.get(), s.c_str(), s.length());
	return r;
}

std::wstring xmlCharToWstring(const xmlChar* value)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> cnv;
	return cnv.from_bytes(const_cast<const char*>(reinterpret_cast<char*>(const_cast<xmlChar*>(value))));
}

void CSettings::setDefaultValue(const std::wstring & name, const std::wstring & value)
{
//	m_settingsmap.emplace(std::make_pair(name, value));
	m_settingsmap[name] = value;
}

void CSettings::setPath(std::string path)
{
	m_path = path+"/settings.xml";
	bool isnew = false;
	xmlDocPtr doc = xmlParseFile(m_path.c_str());
	if (doc == nullptr)
	{
		doc = xmlNewDoc(BAD_CAST "1.0");
		isnew = true;
	}
	if (doc)
	{
		// создать файл настроек по умолчанию
		if (isnew)
		{
			std::map<std::wstring, std::wstring>::iterator it;
			for (it = m_settingsmap.begin(); it != m_settingsmap.end(); it++)
			{
				std::unique_ptr<xmlChar[]> n = wstringToXmlChar(it->first);
				std::unique_ptr<xmlChar[]> v = wstringToXmlChar(it->second);
				setValue(doc, n.get(), v.get());
			}
		}
		xmlFreeDoc(doc);
	}
}

void CSettings::setValue(const xmlDocPtr doc, const xmlChar* nodeName, const xmlChar* nodeContent)
{
	xmlNodePtr root_node = xmlDocGetRootElement(doc);
	if (!root_node || (root_node && xmlStrcmp(root_node->name, (const xmlChar *) "settings")))
	{
		root_node = xmlNewNode(nullptr, BAD_CAST "settings");
		xmlDocSetRootElement(doc, root_node);
	}
	if (root_node)
	{
		xmlNodePtr destNode = nullptr;
		xmlNodePtr curNode = root_node->xmlChildrenNode;
		while (curNode != nullptr) {
			if ((!xmlStrcmp(curNode->name, nodeName)))
			{
				destNode = curNode;
				break;
			}
			curNode = curNode->next;
		}
		if (!destNode) destNode = xmlNewChild(root_node, nullptr, nodeName, BAD_CAST nodeContent);
		else xmlNodeSetContent(destNode, BAD_CAST nodeContent);
		xmlSaveFileEnc(m_path.c_str(), doc, "UTF-8");
	}
}

bool CSettings::setValue(const std::wstring& name, const std::wstring& value)
{
	if (name.empty() || value.empty()) return false;

	std::unique_ptr<xmlChar[]> nodeName = wstringToXmlChar(name);
	if (!nodeName) return false;
	std::unique_ptr<xmlChar[]> nodeContent = wstringToXmlChar(value);
	if (!nodeContent) return false;

	xmlDocPtr doc = xmlParseFile(m_path.c_str());
	if (doc == nullptr)
		doc = xmlNewDoc(BAD_CAST "1.0");
	if (doc)
	{
		setValue(doc, nodeName.get(), nodeContent.get());
		xmlFreeDoc(doc);
	}

	return true;
}

std::wstring CSettings::getValue(const std::wstring & name)
{
	if (!name.empty())
	{
		std::unique_ptr<xmlChar[]> nodeName = wstringToXmlChar(name);
		if (nodeName)
		{
			std::wstring r;
			xmlDocPtr doc = xmlParseFile(m_path.c_str());
			if (doc)
			{
				xmlNodePtr root_node = xmlDocGetRootElement(doc);
				if (root_node && xmlStrcmp(root_node->name, (const xmlChar *) "settings") == 0)
				{
					xmlNodePtr curNode = root_node->xmlChildrenNode;
					while (curNode != nullptr) {
						if ((!xmlStrcmp(curNode->name, nodeName.get())))
						{
							if (xmlChar* nodeContent = xmlNodeListGetString(doc, curNode->xmlChildrenNode, 1))
							{
								r = xmlCharToWstring(nodeContent);
								xmlFree(nodeContent);
							}
							break;
						}
						curNode = curNode->next;
					}
				}
				xmlFreeDoc(doc);
			}
			if (r.empty())
			{
				// вернуть дефолтное значение из map
				return m_settingsmap[name];
			}
		}
	}
	return std::wstring();
}
