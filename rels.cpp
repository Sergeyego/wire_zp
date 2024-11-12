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
    relSm->model()->setAsync(false);

    relLine = new DbSqlRelation("wire_line","id","snam",this);
    relLine->setSort("wire_line.snam");

    relZon = new DbSqlRelation("wire_rab_zon","id","nam",this);
    relZon->setSort("wire_rab_zon.nam");
    relZon->setEditable(true);
    relZon->model()->setAsync(false);

    relEd = new DbSqlRelation("wire_rab_ed","id","nam",this);
    relEd->setSort("wire_rab_ed.nam");
    relEd->setEditable(true);

    relConn = new DbSqlRelation("wire_rab_conn","id","nam",this);
    relConn->setSort("wire_rab_conn.nam");

    relList = new DbSqlRelation("wire_rab_prem_n_list","id","nam",this);
    relList->setSort("wire_rab_prem_n_list.nam");

    relKaminEmp = new DbSqlRelation("kamin_empl","id","str",this);
    relKaminEmp->setSort("kamin_empl.str");
    relKaminEmp->model()->setAsync(false);

    relMark = new DbSqlRelation("provol","id","nam",this);
    relMark->setFilter("provol.is_w=1");
    relMark->setSort("provol.nam");
    relMark->model()->setAsync(false);

    relDiam = new DbSqlRelation("diam","id","sdim",this);
    relDiam->setFilter("diam.is_w=1");
    relDiam->setSort("diam.diam");
    relDiam->model()->setAsync(false);

    relSpool = new DbSqlRelation("wire_pack_kind","id","short",this);
    relSpool->setSort("wire_pack_kind.short");
    relSpool->model()->setAsync(false);

    relJobNam = new DbSqlRelation("wire_rbt_nam","lid","fnam",this);
    relJobNam->setSort("wire_rbt_nam.fnam");
    relJobNam->model()->setAsync(false);
    relJobNam->setAsyncSearch(false);

    relRab = new DbSqlRelation("wire_rbt_lst","id","rnam",this);
    relRab->setSort("wire_rbt_lst.rnam");
    relRab->model()->setAsync(false);
    relRab->setAsyncSearch(false);

    relLiter = new DbSqlRelation("wire_rab_liter","id","naim",this);
    relLiter->setSort("wire_rab_liter.naim");
    relLiter->model()->setAsync(false);

}

