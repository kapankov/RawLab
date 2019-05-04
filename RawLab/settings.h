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
	std::map<std::wstring, std::wstring> m_settingsmap;

	void setValue(const xmlDocPtr doc, const xmlChar* nodeName, const xmlChar* nodeContent);

public:
	CSettings() {}
	~CSettings() {}

	void setDefaultValue(const std::wstring& name, const std::wstring& value);
	void setPath(std::string path);
	bool setValue(const std::wstring& name, const std::wstring& value);
	std::wstring getValue(const std::wstring& name);
};

#endif