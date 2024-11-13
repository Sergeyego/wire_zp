#ifndef FORMTN_H
#define FORMTN_H

#include <QWidget>
#include "modelzon.h"
#include "rels.h"
#include "tndialog.h"
#include "progressexecutor.h"

namespace Ui {
class FormTN;
}

class FormTN : public QWidget
{
    Q_OBJECT

public:
    explicit FormTN(QWidget *parent = nullptr);
    ~FormTN();

private:
    Ui::FormTN *ui;
    ModelZon *modelMark;
    ModelZon *modelDiam;
    ModelZon *modelSpool;
    ModelRo *modelStat;
    DbTableModel *modelTarifs;
    DbTableModel *modelNorms;
    ProgressExecutor *sqlExecutor;

private slots:
    void upd();
    void updStat();
    void updTN(QModelIndex index);
    void setGrpTarif();
    void setGrpNorm();
    void updFull();
};

#endif // FORMTN_H
