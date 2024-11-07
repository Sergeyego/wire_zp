#ifndef FORMJOB_H
#define FORMJOB_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "truncatepremdialog.h"
#include "rels.h"

namespace Ui {
class FormJob;
}

class ModelJob : public DbTableModel
{
    Q_OBJECT

public:
    explicit ModelJob(QWidget *parent = nullptr);
    void refresh(QDate beg, QDate end, int id_rab=-1, int id_zon=-1);

};

class FormJob : public QWidget
{
    Q_OBJECT

public:
    explicit FormJob(QWidget *parent = nullptr);
    ~FormJob();

private:
    Ui::FormJob *ui;
    ModelJob *modelJob;
    bool updTempTables();

private slots:
    void upd();
    void truncatePrem();   
};

#endif // FORMJOB_H
