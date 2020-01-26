#ifndef RAWLAB_CAMLISTDLG_H
#define RAWLAB_CAMLISTDLG_H

#include "ui_camlistdlg.h"

class CamListDialog : public QDialog
{
    Q_OBJECT

public:
    CamListDialog(const char** camlist, QWidget* parent = nullptr);
    
private:
    Ui::CamListDialog ui;
};

#endif