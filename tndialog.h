#ifndef TNDIALOG_H
#define TNDIALOG_H

#include <QDialog>
#include <QDate>
#include "models.h"

namespace Ui {
class TnDialog;
}

class TnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TnDialog(bool nrm, QWidget *parent = 0);
    ~TnDialog();
    QDate getDate();
    double getVal();
    int getIdList();

private:
    Ui::TnDialog *ui;
};

#endif // TNDIALOG_H
