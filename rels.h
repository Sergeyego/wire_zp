#ifndef RELS_H
#define RELS_H

#include <QObject>
#include "db/dbtablemodel.h"

class Rels : public QObject
{
    Q_OBJECT
public:
    static Rels *instance();
    DbSqlRelation *relLine;
    DbSqlRelation *relZon;
    DbSqlRelation *relJobNam;
    DbSqlRelation *relRab;
    DbSqlRelation *relEd;
    DbSqlRelation *relConn;
    DbSqlRelation *relKaminEmp;
    DbSqlRelation *relList;
    DbSqlRelation *relMark;
    DbSqlRelation *relDiam;
    DbSqlRelation *relSpool;
    DbSqlRelation *relLiter;

protected:
    explicit Rels(QObject *parent = nullptr);

private:
    static Rels *rels_instance;

};

#endif // RELS_H
