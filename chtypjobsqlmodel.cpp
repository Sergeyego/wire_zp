#include "chtypjobsqlmodel.h"
#include <QMessageBox>

ChTypJobSqlModel::ChTypJobSqlModel(QObject *parent) :
    QSqlQueryModel(parent)
{
    notNull=true;
    fvid=false;
    fmarka=false;
    fdiam=false;
    fpack=false;
    vidId=-1;
    refreshCur();
}

void ChTypJobSqlModel::refresh(bool evid, int id_vid, bool emarka, QVector<int> id_provol, bool ediam, QVector<int> id_diam, bool epack, QVector<int> id_pack)
{ 
    fvid=evid;
    vidId=id_vid;
    fmarka=emarka;
    provolId=id_provol;
    fdiam=ediam;
    diamId=id_diam;
    fpack=epack;
    packId=id_pack;
    QString sufVid, sufMarka, sufDiam, sufPack;
    if (fvid) sufVid=" and n.id= "+QString::number(vidId); else sufVid="";
    if (fmarka) {
        if (id_provol.size()){
            sufMarka=" and (n.id_prov= ";
            for (int i=0; i<id_provol.size(); i++){
                sufMarka+=QString::number(id_provol.at(i))+" or n.id_prov = ";
            }
            sufMarka.truncate(sufMarka.size()-16);
            sufMarka+=" )";
        } else {
            sufMarka=" and n.id_prov=-1";
        }
    }
    if (fdiam) {
        if (id_diam.size()){
            sufDiam=" and (n.id_diam= ";
            for (int i=0; i<id_diam.size(); i++){
                sufDiam+=QString::number(id_diam.at(i))+" or n.id_diam = ";
            }
            sufDiam.truncate(sufDiam.size()-16);
            sufDiam+=" )";
        } else {
            sufDiam=" and n.id_diam=-1";
        }
    }
    if (fpack) {
        if (id_pack.size()){
            sufPack=" and (n.id_pack= ";
            for (int i=0; i<id_pack.size(); i++){
                sufPack+=QString::number(id_pack.at(i))+" or n.id_pack = ";
            }
            sufPack.truncate(sufPack.size()-16);
            sufPack+=" )";
        } else {
            sufPack=" and n.id_pack=-1";
        }
    }
    QString sJoin=notNull? "inner" : "left outer";
    setQuery("select n.lid, n.fnam, t.tarif, j.norm from wire_rab_nams as n "
             +sJoin+" join wire_tarifs('"+QDate::currentDate().toString("yyyy-MM-dd")+"') as t on t.lid=n.lid "
             "left outer join wire_norms('"+QDate::currentDate().toString("yyyy-MM-dd")+"') as j on j.lid=n.lid "
             "where n.lid>0"+sufVid+sufMarka+sufDiam+sufPack +" order by n.fnam");
    if (lastError().isValid())
    {
        QMessageBox::critical(NULL,"Error",lastError().text(),QMessageBox::Ok);
    } else {
        setHeaderData(1, Qt::Horizontal,tr("Вид работы"));
        setHeaderData(2, Qt::Horizontal,tr("Тариф"));
        setHeaderData(3, Qt::Horizontal,tr("Норма"));
        emit sUpd();
    }
}

void ChTypJobSqlModel::refreshCur()
{
    refresh(fvid,vidId,fmarka,provolId,fdiam,diamId,fpack,packId);
}

void ChTypJobSqlModel::setNotNull(bool val)
{
    notNull=val;
    refreshCur();
}

QVariant ChTypJobSqlModel::data(const QModelIndex &index,int role) const
{
   QVariant value = QSqlQueryModel::data(index, role);
   switch (role) {
    case Qt::DisplayRole: // Данные для отображения
       if (index.column()==2 and !value.isNull()) return QLocale().toString(value.toDouble(),'f',2); else
       if (index.column()==3 and !value.isNull()) return QLocale().toString(value.toDouble(),'f',3); else
       return value;

    case Qt::EditRole:    // Данные для редактирования
        return value;

    case Qt::TextAlignmentRole: // Выравнивание
        if(index.column() == 2 || index.column() == 3)
            return int(Qt::AlignRight | Qt::AlignVCenter);
        else return int(Qt::AlignLeft | Qt::AlignVCenter);

   }
   return value;
}
