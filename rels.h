#ifndef RELS_H
#define RELS_H

#include <QObject>
#include "db/dbtablemodel.h"

class Rels : public QObject
{
    Q_OBJECT
public:
    static Rels *instance();
    DbSqlRelation *relSm;
    DbSqlRelation *relLine;
    DbSqlRelation *relZon;
    DbSqlRelation *relJobNam;
    DbSqlRelation *relRab;
    DbSqlRelation *relEd;
    DbSqlRelation *relConn;
    DbSqlRelation *relKaminEmp;
    DbSqlRelation *relList;

    /*DbSqlRelation *relLiter;
    DbSqlRelation *relMark;
    DbSqlRelation *relDiam;
    DbSqlRelation *relComp;
    DbSqlRelation *relPress;
    DbSqlRelation *relPressFlt;
    DbSqlRelation *relMarkType;
    DbSqlRelation *relElPart;*/

protected:
    explicit Rels(QObject *parent = nullptr);

private:
    static Rels *rels_instance;

};

#endif // RELS_H
