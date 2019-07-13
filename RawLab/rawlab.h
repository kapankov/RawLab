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
	// делитель для автоматического расчета второго зеленого канала Green2 баланса белого
	float m_G2div;
	// флаг, что ББ уже был обновлен/установлен в UI в ходе конвертации
	// чтобы не обновлять ББ несколько раз
	bool m_isWBset;
	// сохраняется имя пресета ББ перед запуском потока, чтобы после конвертации восстановить
	QString m_lastWBPreset;

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

	QString m_filename; // открытый файл
	std::thread m_threadProcess;
	std::atomic_flag m_cancelProcess ATOMIC_FLAG_INIT;
	RgbBuffPtr	m_pRawBuff;

	bool setImageJpegFile(const QString& filename);
	bool setImageRawFile(const QString&  filename);
	void setProgress(const QString& text);
	bool isCancel(); // проверить, нужно ли остановить поток обработки RAW
	void addPropertiesSection(const QString& name);
	void addPropertiesItem(const QString& name, const QString& value);
	void fillProperties(const LibRawEx &lr);
	void RawProcess();

	void setWBSliders(const float(&mul)[4], bool setDefault = true);
	void setWBControls(const float(&mul)[4], RAWLAB::WBSTATE wb);
	void updateAutoWB(const float(&mul)[4], RAWLAB::WBSTATE wb);
	void setAutoGreen2(bool checked);
	void updateParamControls();
	// по текущим значениям слайдеров ББ определить пресет
	int getWbPreset(const QString& lastPreset = "") const;
signals:
	void rawProcessed(QString message);
	void setRun(bool default);
public slots:
	void onOpen();
	void onExit();
	void onRun();
	void onAbout();
	void onZoomChanged(int prc);
	void onProcessed(QString message);
	void onSetRun(bool default);
	void onWBRedValueChanged(double value);
	void onWBGreenValueChanged(double value);
	void onWBBlueValueChanged(double value);
	void onWBGreen2ValueChanged(double value);
	void onAutoGreen2Clicked(bool checked);

};
