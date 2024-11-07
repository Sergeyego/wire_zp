#include "rels.h"

Rels* Rels::rels_instance=nullptr;

Rels *Rels::instance()
{
    if (rels_instance==nullptr)
        rels_instance = new Rels();
    return rels_instance;
}


Rels::Rels(QObject *parent) : QObject(parent)
{  
    relSm = new DbSqlRelation("wire_smena","id","name",this);
    relSm->setSort("wire_smena.name");

    relLine = new DbSqlRelation("wire_line","id","snam",this);
    relLine->setSort("wire_line.snam");

    relZon = new DbSqlRelation("wire_rab_zon","id","nam",this);
    relZon->setSort("wire_rab_zon.nam");
    relZon->setEditable(true);
    relZon->model()->setAsync(false);

    /*relLiter = new DbSqlRelation("rab_liter","id","naim",this);
    relLiter->setSort("rab_liter.naim");

    relList = new DbSqlRelation("rab_prem_list","id","nam",this);
    relList->setSort("rab_prem_list.nam");

    relEd = new DbSqlRelation("wire_rab_ed","id","nam",this);
    relEd->setSort("wire_rab_ed.nam");
    relEd->setEditable(true);

    relConn = new DbSqlRelation("rab_conn","id","tip_conn",this);
    relConn->setSort("rab_conn.tip_conn");*/

    relKaminEmp = new DbSqlRelation("kamin_empl","id","str",this);
    relKaminEmp->setSort("kamin_empl.str");
    relKaminEmp->model()->setAsync(false);

    /*relMark = new DbSqlRelation("elrtr","id","marka",this);
    relMark->setFilter("elrtr.id>0");
    relMark->setSort("elrtr.marka");

    relDiam = new DbSqlRelation("diam","id","sdim",this);
    relDiam->setFilter("diam.is_el=1");
    relDiam->setSort("diam.diam");

    relComp = new DbSqlRelation("matr","id","nam",this);
    relComp->setSort("matr.nam");*/

    relJobNam = new DbSqlRelation("wire_rbt_nam","lid","fnam",this);
    relJobNam->setSort("wire_rbt_nam.fnam");
    relJobNam->model()->setAsync(false);
    relJobNam->setAsyncSearch(false);

    relRab = new DbSqlRelation("wire_rbt_lst","id","rnam",this);
    relRab->setSort("wire_rbt_lst.rnam");
    relRab->model()->setAsync(false);
    relRab->setAsyncSearch(false);

    /*relPress = new DbSqlRelation("pres","id","nam",this);
    relPress->setSort("pres.nam");

    relPressFlt = new DbSqlRelation("pres","id","nam",this);
    relPressFlt->model()->setAsync(false);
    relPressFlt->setFilter("pres.id<>0");
    relPressFlt->setSort("pres.nam");
    relPressFlt->refreshModel();

    relMarkType = new DbSqlRelation("el_norm_types","id","nam",this);
    relMarkType->model()->setAsync(false);
    relMarkType->setEditable(true);
    relMarkType->setSort("el_norm_types.nam");

    relElPart = new DbSqlRelation("parti","id","str",this);
    relElPart->setAlias("eparti");
    relElPart->setSort("eparti.yea desc, eparti.n_s");
    relElPart->setFilter("eparti.id<>0");
    relElPart->model()->setLimit(4000);*/
}

