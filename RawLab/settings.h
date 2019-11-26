/*
основна€ иде€:
настройки хран€тс€ в xml-файле settings.xml
кроме этого есть настройки по умолчанию, которые хран€тс€ в словаре (std::map) m_settingsmap
перед использованием необходимо задать сперва настройки по умолчанию, а затем указать каталог, 
в котором будет сохранен setting.xml
*/
#ifndef CSETTINGS_H
#define CSETTINGS_H

class CSettings
{
	std::string m_path;
	std::map<std::string, std::string> m_settingsmap;

	void setValue(const xmlDocPtr doc, const xmlChar* nodeName, const xmlChar* nodeContent);

public:
	CSettings() {}
	~CSettings() {}

	void setDefaultValue(const std::string& name, const std::string& value);
	void setPath(const std::string& path);
	bool setValue(const std::string& name, const std::string& value);
	std::string getValue(const std::string& name);
};

#endif