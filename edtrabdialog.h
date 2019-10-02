#ifndef EDTRABDIALOG_H
#define EDTRABDIALOG_H

#include <QWidget>
#include "models.h"
#include "db/dbtablemodel.h"

namespace Ui {
class EdtRabDialog;
}

class EdtRabDialog : public QWidget
{
    Q_OBJECT
    
public:
    explicit EdtRabDialog(QWidget *parent = 0);
    ~EdtRabDialog();
    
private:
    Ui::EdtRabDialog *ui;
    DbTableModel *modelHist;
    DbTableModel *modelProf;
    DbTableModel *modelRab;
    int id_rab;

private slots:
    void updHist(QModelIndex index);
    void updStat();
};

#endif // EDTRABDIALOG_H
