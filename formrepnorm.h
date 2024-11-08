#ifndef FORMREPNORM_H
#define FORMREPNORM_H

#include <QWidget>
#include <QtGui>
#include <QtSql>
#include <QMessageBox>
#include <QFileDialog>
#include "xlsx/xlsxdocument.h"
#include "modelzon.h"
#include "rels.h"

using namespace QXlsx;

namespace Ui {
class FormRepNorm;
}

class ModelRepJob : public ModelRo
{
    Q_OBJECT

 public:
    explicit ModelRepJob(QWidget *parent = nullptr);
    void refresh(int typ, QString zonSuf, int idSm, QDate dbeg, QDate dend);
};

class FormRepNorm : public QWidget
{
    Q_OBJECT
    
public:
    explicit FormRepNorm(QWidget *parent = nullptr);
    ~FormRepNorm();
    
private:
    Ui::FormRepNorm *ui;
    ModelZon *modelZon;
    ModelRepJob *modelJob;
    QString signRep;
    QString getProf(int id_rb, QDate date);
    bool getTotalVip(double &vip, int &d, double &kvo, const int id_rb, const int id_sm=-1);

private slots:
    void chSm();
    void upd();
    void saveXls();
    void saveXlsPer();
    void updTotal(QModelIndex index);
    void goRep();
    void updSign();
};

#endif // FORMREPNORM_H
