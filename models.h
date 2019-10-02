#ifndef MODELS_H
#define MODELS_H

#include <QObject>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QtSql>
#include "db/dbtablemodel.h"
#include <QSortFilterProxyModel>
#include <QMap>

class Models : public QObject
{
    Q_OBJECT
public:
    static Models *instance();

    DbRelation *relRab;
    DbRelation *relTypeJob;
    DbRelation *relSm;
    DbRelation *relLine;
    DbRelation *relPremList;
    DbRelation *relZon;
    DbRelation *relConn;
    DbRelation *relProf;
    DbRelation *relOp;
    DbRelation *relEd;

    DbRelationalModel *modelMark;
    DbRelationalModel *modelProvol;
    DbRelationalModel *modelDiam;
    DbRelationalModel *modelPack;
    DbRelationalModel *modelOpNrm;
    DbRelationalModel *modelJob;

    QString sign();
    QString signRep();
    QString org();

protected:
    Models(QObject *parent = 0);

private:
    static Models* models_instance;
    QString signNrm;
    QString orgNam;
    QString signReport;
    
signals:
    
public slots:
    void refresh();
    void refreshOrg();
    void setRabFilter(bool b);
};

#endif // MODELS_H
