#include "modeljob.h"

ModelJob::ModelJob(QDate beg, QDate end, QObject *parent) : DbTableModel("wire_rab_job",parent), date_beg(beg), date_end(end)
{
    addColumn("id","id",true,TYPE_INT);
    addColumn("dat",tr("Дата"),false,TYPE_DATE);
    addColumn("id_sm",tr("Смена"),false,TYPE_STRING,NULL,Models::instance()->relSm);
    addColumn("id_rb",tr("Работник"),false,TYPE_STRING,NULL, Models::instance()->relRab);
    addColumn("id_line",tr("Оборуд."),false,TYPE_STRING,NULL,Models::instance()->relLine);
    addColumn("lid",tr("Вид работы"),false,TYPE_STRING,NULL, Models::instance()->relTypeJob);
    addColumn("kvo",tr("Объем"),false,TYPE_DOUBLE, new QDoubleValidator(0,999999999,3,this));
    addColumn("chas_sm",tr("Час/см"),false,TYPE_INT,new QIntValidator(0,24,this));
    addColumn("koef_prem_kvo",tr("К.прем"),false,TYPE_DOUBLE,new QDoubleValidator(0,50,2,this));
    addColumn("extr_time",tr("Св.ур"),false,TYPE_INTBOOL);
    addColumn("chas_sn",tr("Для с/н, ч"),false,TYPE_INT,new QIntValidator(0,24,this));
    addColumn("prim",tr("Примечание"),false,TYPE_STRING);
    setSort("wire_rab_job.datf, wire_rab_job.id");
    setSuffix("inner join wire_rab_nams on wire_rab_job.lid=wire_rab_nams.lid "
              "inner join wire_rab_liter on wire_rab_nams.id=wire_rab_liter.id ");
    setDefaultValue(7,11);
    setDefaultValue(8,1);
    fzon=false;
    frb=false;
    pos_rb=0;
    pos_zon=0;
    refreshState();
}

QVariant ModelJob::data(const QModelIndex &index, int role) const
{
    if (role==Qt::BackgroundColorRole){
        int area = colorState.value(DbTableModel::data(this->index(index.row(),5),Qt::EditRole).toString());
                if(area == 0) return QVariant(QColor(255,170,170)); else
                    if(area == 1) return QVariant(QColor(Qt::yellow)); else
                        if(area == 2) return QVariant(QColor(255,200,100)); else
                            return QVariant(QColor(Qt::white));

    } else return DbTableModel::data(index,role);
}

void ModelJob::setDateBeg(QDate val)
{
    date_beg=val;
    refreshState();
    select();
}

void ModelJob::setDateEnd(QDate val)
{
    date_end=val;
    setDefaultValue(1,val);
    select();
}

void ModelJob::setPosZon(int p)
{
    pos_zon=p;
    select();
}

void ModelJob::setPosRb(int p)
{
    pos_rb=p;
    select();
}

void ModelJob::setFZon(bool val)
{
    fzon=val;
    select();
}

void ModelJob::setFRb(bool val)
{
    frb=val;
    select();
}

bool ModelJob::select()
{
    QString flt="";
    if (fzon){
        int id_zon=Models::instance()->relZon->model()->data(Models::instance()->relZon->model()->index(pos_zon,0),Qt::EditRole).toInt();
        flt+=" and  wire_rab_liter.id_zon= "+QString::number(id_zon);
    }
    if (frb){
        int id_rab=Models::instance()->relRab->proxyModel()->data(Models::instance()->relRab->proxyModel()->index(pos_rb,0),Qt::EditRole).toInt();
        flt+=" and  wire_rab_job.id_rb= "+QString::number(id_rab);
    }
    setFilter("wire_rab_job.dat between '"+date_beg.toString("yyyy-MM-dd")+"' and '"
                             +date_end.toString("yyyy-MM-dd")+"'"+flt);
    return DbTableModel::select();
}

void ModelJob::refreshState()
{
    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare("select r.lid, t.tarif, n.norm, l.is_nrm from wire_rab_nams as r "
                  "inner join wire_rab_liter as l on r.id=l.id "
                  "left outer join wire_tarifs(:d1) as t on r.lid=t.lid "
                  "left outer join wire_norms(:d2) as n on r.lid=n.lid");
    query.bindValue(":d1",date_beg);
    query.bindValue(":d2",date_beg);
    if (query.exec()){
        colorState.clear();
        beginResetModel();
        while (query.next()){
            bool is_nrm=query.value(3).toBool();
            bool trf=query.value(1).toDouble()!=0.0;
            bool nrm=query.value(2).toDouble()!=0.0;
            colorState[query.value(0).toString()]=(trf ? 1 : 0)+((nrm && is_nrm)||(!is_nrm) ? 2 : 0);
        }
        endResetModel();
    } else {
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Ok);
    }
}
