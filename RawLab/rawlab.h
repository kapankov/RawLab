#pragma once

#include <QObject>
#include <QtWidgets/QMainWindow>
#include <QShortcut>
#include "ui_rawlab.h"

struct wbpreset
{
	QString name;
	float wb[4];
	wbpreset(const QString& presetName, const float (&presetWb)[4]) : name(presetName)
	{
		wb[0] = presetWb[0];
		wb[1] = presetWb[1];
		wb[2] = presetWb[2];
		wb[3] = presetWb[3];
	}
	wbpreset(const QString& presetName, const int(&presetWb)[4]) : name(presetName)
	{
		wb[0] = presetWb[1] ? static_cast<float>(presetWb[0])/presetWb[1] : 1.0f;
		wb[1] = 1.0f;
		wb[2] = presetWb[1] ? static_cast<float>(presetWb[2]) / presetWb[1] : 1.0f;
		wb[3] = presetWb[1] ? static_cast<float>(presetWb[3]) / presetWb[1] : 1.0f;
	}
};

class RawLab : public QMainWindow
{
	Q_OBJECT

	friend int RAW_progress_cb(void *callback_data, enum LibRaw_progress stage, int iteration, int expected);
public:
	RawLab(QWidget *parent = Q_NULLPTR);
	~RawLab();

	void openFile(const QString& filename);
private:
	Ui::RawLabClass ui;

	std::unique_ptr<LibRawEx> m_lr;
	std::vector<wbpreset> m_wbpresets;
	// �������� ��� ��������������� ������� ������� �������� ������ Green2 ������� ������
	float m_green2div;

	CSettings m_settings;

	QString m_lastDir;
	QShortcut* m_MoveLeft;
	QShortcut* m_MoveRight;
	QShortcut* m_MoveUp;
	QShortcut* m_MoveDown;

	QLabel* m_plblState;
	QLabel* m_plblProgress;
	QLabel* m_plblScale;
	QLabel* m_plblInfo;

	QString m_filename; // �������� ����
	std::thread m_threadProcess;
	std::atomic_flag m_cancelProcess ATOMIC_FLAG_INIT;
	RgbBuffPtr	m_pRawBuff;
	// ����������� ��������� � �������� � ��������� �����
	// � ��������� ��������� Input Profiles
	std::unique_ptr<QFileSystemWatcher> m_inputProfilesWatcher;
	// ����������� ��������� � �������� � ��������� ���������
	// � ��������� ��������� Output Profiles
	std::unique_ptr<QFileSystemWatcher> m_outputProfilesWatcher;
	std::string m_inputProfile;
	std::string m_outputProfile;

	QShortcut* m_NextLeftPanelShortcut;

	bool setImageJpegFile(const QString& filename);
	bool setImageRawFile(const QString&  filename);
	void setProgress(const QString& text);
	bool isCancel(); // ���������, ����� �� ���������� ����� ��������� RAW
	void addPropertiesSection(const QString& name);
	void addPropertiesItem(const QString& name, const QString& value);
	void setPropertiesItem(const QString& name, const QString& value);
	void fillProperties(const LibRawEx &lr);
	// ������� �� ��������� RAW-����� ������ � �������� ���
	bool ExtractAndShowPreview(const std::unique_ptr<LibRawEx>& pLr);
	// ��������� ������������� ����������� �� LibRaw � m_pRawBuff
	void ExtractProcessedRaw();
	void RawProcess();
	void UpdateCms(bool enable);

	void setWBSliders(const float(&mul)[4], bool setDefault = true);
	// ��������� WB �������� ������ ����� ���������� RAW
	void setWBControls(const float(&mul)[4], RAWLAB::WBSTATE wb);
	void updateAutoWB(const float(&mul)[4], RAWLAB::WBSTATE wb);
	void setAutoGreen2(bool value);
	// ��� ������ ��������� ��������� �����
	void onGammaSlope(double gamma, double slope);
	void updateParamControls();
	// �� ������� ��������� ��������� �� ���������� ������
	int getWbPreset(const QString& lastPreset = "") const;
	void updateGreen2Div();
	void updateInputProfiles();
	void updateOutputProfiles();
signals:
	void rawProcessed(QString message);
	void setProcess(bool default);
public slots:
	void onOpen();
	void onSave();
	void onSavePreview();
	void onCms();
	void onExit();
	void onProcess();
	void onAbout();
	// ������������ ������� Process-Properties (Ctrl+Tab)
	void onNextLeftPanel();
	// �������� Preview
	void onShowPreview();
	// �������� ��������� RAW-���������
	void onShowProcessedRaw();
	// �������� ������� ��������
	void onMonitorProfileChanged(bool enable);
	void onZoomChanged(int prc);
	void onPointerChanged(int x, int y);
	void onProcessed(QString message);
	void onSetProcess(bool default);
	void onWBRedValueChanged(double value);
	void onWBGreenValueChanged(double value);
	void onWBBlueValueChanged(double value);
	void onWBGreen2ValueChanged(double value);
	void onAutoGreen2Clicked(bool checked);
	void onAutoBrightnessClicked(bool checked);
	void onGammaValueChanged(double value);
	void onSlopeValueChanged(double value);
	void onInputProfilesDirChanged(const QString &path);
	void onOutputProfilesDirChanged(const QString &path);

};
