#include "models.h"

Models* Models::models_instance=0;

Models::Models(QObject *parent) :
    QObject(parent)
{
    relRab = new DbRelation("select e.id, e.first_name||' '||e.last_name||' '||e.middle_name, oj.nam, CASE WHEN (COALESCE(oj.nam,'--')<>'--') THEN 1 ELSE 0 END "
                            "from wire_empl as e "
                            "left outer join "
                            "(select md.id_empl, p.nam from "
                            "(select id_empl, max(dat) as mdat from (select * from wire_empl_pos_h where dat<='"+QDate::currentDate().toString("yyyy-MM-dd")+"') as th group by id_empl) as md "
                            "inner join wire_empl_pos_h h on (h.id_empl=md.id_empl and h.dat=md.mdat and h.id_op<>3) "
                            "inner join wire_empl_pos as p on p.id=h.id_pos) as oj on e.id=oj.id_empl "
                            "order by e.first_name, e.last_name, e.middle_name",0,1,this);
    relTypeJob = new DbRelation(QString("select n.lid, n.fnam from wire_rab_nams as n "
                                        "inner join wire_tarifs('"+QDate::currentDate().toString("yyyy-MM-dd")+"') as t on t.lid=n.lid order by n.fnam"),0,1,this);
    relSm = new DbRelation(QString("select id, name from wire_smena order by name"),0,1,this);
    relLine = new DbRelation(QString("select id, snam from wire_line order by snam"),0,1,this);
    relPremList = new DbRelation(QString("select id, nam from wire_rab_prem_n_list order by nam"),0,1,this);
    relZon = new DbRelation(QString("select id, nam from wire_rab_zon order by nam"),0,1,this);
    relConn = new DbRelation(QString("select id, nam from wire_rab_conn order by nam"),0,1,this);
    relProf = new DbRelation(QString("select id, nam from wire_empl_pos order by nam"),0,1,this);
    relOp = new DbRelation(QString("select id, nam from wire_empl_op order by nam"),0,1,this);
    relEd = new DbRelation(QString("select id, nam from wire_rab_ed order by nam"),0,1,this);

    modelJob = new DbRelationalModel(QString("select id, naim from wire_rab_liter order by naim"),this);
    modelMark = new DbRelationalModel(QString("select id, nam from provol where is_w=0 order by nam"),this);
    modelProvol = new DbRelationalModel(QString("select id, nam from provol where is_w=1 order by nam"),this);
    modelDiam = new DbRelationalModel(QString("select id, sdim from diam order by sdim"),this);
    modelPack = new DbRelationalModel(QString("select id, nam, short from wire_pack_kind order by nam"),this);
    modelOpNrm  = new DbRelationalModel(QString("select id, naim from wire_rab_liter where is_nrm=true order by naim"),this);
    modelOpNrm->setHeaderData(1,Qt::Horizontal,tr("Вид работы"));
    refreshOrg();
    setRabFilter(true);

}


void Models::refresh()
{
    relRab->model()->refresh();
    relTypeJob->model()->refresh();
    relSm->model()->refresh();
    relLine->model()->refresh();
    relPremList->model()->refresh();
    relZon->model()->refresh();
    relConn->model()->refresh();
    relProf->model()->refresh();
    relOp->model()->refresh();
    relEd->model()->refresh();
    modelJob->refresh();
    modelMark->refresh();
    modelProvol->refresh();
    modelDiam->refresh();
    modelPack->refresh();
    modelOpNrm->refresh();
    refreshOrg();
    //setRabFilter(true);
}

void Models::refreshOrg()
{
    QSqlQuery query;
    query.prepare("select fnam, sign_norm , sign_rep from hoz where id=11");
    if (query.exec()){
        while (query.next()){
            orgNam=query.value(0).toString();
            signNrm=query.value(1).toString();
            signReport=query.value(2).toString();
        }
    } else {
        QMessageBox::critical(NULL,"Error",query.lastError().text(),QMessageBox::Cancel);
    }
}

void Models::setRabFilter(bool b)
{
    relRab->proxyModel()->setFilterKeyColumn(3);
    relRab->proxyModel()->setFilterFixedString(b ? "1" : "");
}


Models *Models::instance()
{
    if (models_instance == 0)
        models_instance = new Models;
    return models_instance;
}

QString Models::sign()
{
    return signNrm;
}

QString Models::signRep()
{
    return signReport;
}

QString Models::org()
{
    return orgNam;
}
