#ifndef RAWLAB_SETTINGSDLG_H
#define RAWLAB_SETTINGSDLG_H

#include "ui_settingsdlg.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(CSettings* pSettings, QWidget* parent = nullptr);

    QString getSaveLastDir();
    bool getAutoGreen2();
    bool getTiff();
    bool getBps();
private:
    Ui::SettingsDialog ui;
    CSettings* m_settings;

    QString m_lastDir;
    bool m_saveLastDir = true;
    bool m_isAutoGreen2 = true;
    bool m_isTiff = true;
    bool m_is16bps = true;

public slots:
    void onLastDirChanged(const QString& text);
    void onSelectLastDir();
    void onSaveLastDirClicked(bool checked);
    void onAutoGreen2Clicked(bool checked);
    void onTiffClicked(bool checked);
    void onBpsClicked(bool checked);
};

#endif