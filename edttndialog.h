#ifndef EDTTNDIALOG_H
#define EDTTNDIALOG_H

#include <QWidget>
#include "models.h"
#include "chtypjobsqlmodel.h"
#include "db/dbtablemodel.h"
#include "modelchk.h"
#include "tndialog.h"
#include "models.h"

namespace Ui {
class EdtTNDialog;
}

class EdtTNDialog : public QWidget
{
    Q_OBJECT
    
public:
    explicit EdtTNDialog(QWidget *parent = 0);
    ~EdtTNDialog();
    
private:
    Ui::EdtTNDialog *ui;
    ChTypJobSqlModel *jobModel;
    DbTableModel *modelTarifs;
    DbTableModel *modelNorms;
    ModelChk *modelMark;
    ModelChk *modelDiam;
    ModelChk *modelPack;

private slots:
    void refresh();
    void updTN(QModelIndex index);
    void updTblJob();
    void updJob();
    void setGrpTarif();
    void setGrpNorm();
};

#endif // EDTTNDIALOG_H
