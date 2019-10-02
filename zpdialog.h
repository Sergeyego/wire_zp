#ifndef ZPDIALOG_H
#define ZPDIALOG_H

#include <QWidget>
#include "zpsqlmodel.h"
#include <QtGui>
#include "models.h"
#include <QProgressDialog>

namespace Ui {
class ZpDialog;
}

class ZpDialog : public QWidget
{
    Q_OBJECT
    
public:
    explicit ZpDialog(QWidget *parent = 0);
    ~ZpDialog();
    
private:
    Ui::ZpDialog *ui;
    zpSqlModel *zpmodel;
    ModelRabDate *modelRabDate;

private slots:
    void updTable();
    void setModelDate();
    void genTabel();
    void genTabelShort();
    void loadSettings();
    void saveSettings();
};

#endif // ZPDIALOG_H
