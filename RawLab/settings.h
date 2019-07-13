/*
�������� ����:
��������� �������� � xml-����� settings.xml
����� ����� ���� ��������� �� ���������, ������� �������� � ������� (std::map) m_settingsmap
����� �������������� ���������� ������ ������ ��������� �� ���������, � ����� ������� �������, 
� ������� ����� �������� setting.xml
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
	void setPath(const std::string& path);
	bool setValue(const std::wstring& name, const std::wstring& value);
	std::wstring getValue(const std::wstring& name);
};

#endif