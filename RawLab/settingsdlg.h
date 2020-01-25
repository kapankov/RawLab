#ifndef RAWLAB_SETTINGSDLG_H
#define RAWLAB_SETTINGSDLG_H

#include "ui_settingsdlg.h"

#define JPEGQUALDEF 90
#define JPEGQUALMIN 50
#define JPEGQUALMAX 100

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(CSettings* pSettings, QWidget* parent = nullptr);

    QString getSaveLastDir();
    bool getAutoGreen2();
    int getJpegQuality();
private:
    Ui::SettingsDialog ui;
    CSettings* m_settings;

    QString m_lastDir;
    bool m_saveLastDir = true;
    bool m_isAutoGreen2 = true;
    int m_JpegQual = JPEGQUALDEF;

public slots:
    void onLastDirChanged(const QString& text);
    void onSelectLastDir();
    void onSaveLastDirClicked(bool checked);
    void onAutoGreen2Clicked(bool checked);
    void onJpegQualChanged(int i);
};

#endif