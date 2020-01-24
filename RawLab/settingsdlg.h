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
private:
    Ui::SettingsDialog ui;
    CSettings* m_settings;

    QString m_lastDir;
    bool m_saveLastDir = true;
    bool m_isAutoGreen2 = true;

public slots:
    void onLastDirChanged(const QString& text);
    void onSelectLastDir();
    void onSaveLastDirClicked(bool checked);
    void onAutoGreen2Clicked(bool checked);
};

#endif