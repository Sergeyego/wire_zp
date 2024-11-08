#ifndef FORMJOB_H
#define FORMJOB_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "truncatepremdialog.h"
#include "rels.h"
#include "db/executor.h"

namespace Ui {
class FormJob;
}

class ModelJob : public DbTableModel
{
    Q_OBJECT

public:
    explicit ModelJob(QWidget *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const;
    void refresh(QDate beg, QDate end, int id_rab=-1, int id_zon=-1);
    void setIdBrig(int id);
    int getIdBrig();

public slots:
    void refreshState();

private slots:
    void stFinished();

private:
    int id_brig;
    QMap <QString,int> colorState;
    QDate dBeg;
    Executor *stateExecutor;
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
    void chkPrim();
    void chkRab(int row);
};

#endif // FORMJOB_H
