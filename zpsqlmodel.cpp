#include "zpsqlmodel.h"
#include <QtGui>

zpSqlModel::zpSqlModel(QObject *parent) :
    QSqlQueryModel(parent)
{
}

void zpSqlModel::refresh(int id_rb)
{
    setQuery("select datf, nrab, kvo, tarif, zp, dopl, extrtime, premk, premn, total, sn from wire_calc_zp_rab("+ QString::number(id_rb)+",'"+dBeg.toString("yyyy.MM.dd")+
             "','"+dEnd.toString("yyyy.MM.dd")+"')");
    if (lastError().isValid())
    {
        QMessageBox::critical(NULL,"Error",lastError().text(),QMessageBox::Cancel);
    } else {
        setHeaderData(0, Qt::Horizontal,tr("Дата"));
        setHeaderData(1, Qt::Horizontal,tr("Наименование работ"));
        setHeaderData(2, Qt::Horizontal,tr("К-во"));
        setHeaderData(3, Qt::Horizontal,tr("Тариф"));
        setHeaderData(4, Qt::Horizontal,tr("З/П"));
        setHeaderData(5, Qt::Horizontal,tr("В т/ч допл."));
        setHeaderData(6, Qt::Horizontal,tr("Св.ур."));
        setHeaderData(7, Qt::Horizontal,tr("Пр.кач"));
        setHeaderData(8, Qt::Horizontal,tr("Пр.нор"));
        setHeaderData(9, Qt::Horizontal,tr("К выд."));
        setHeaderData(10, Qt::Horizontal,tr("с/н"));
    }
}

void zpSqlModel::setdate(QDate begDate, QDate endDate)
{
    dBeg=begDate;
    dEnd=endDate;
}

QVariant zpSqlModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlQueryModel::data(index, role);
    switch (role) {
    case Qt::DisplayRole: // Данные для отображения
        if (index.column()==0) return QLocale().toString(value.toDate(),"dd.MM.yy");
        if (index.column()==2) return QLocale().toString(value.toDouble(),'f',3); else
            if (index.column() >2 && index.column()<10) return QLocale().toString(value.toDouble(),'f',2);
            else return value;

     case Qt::EditRole:    // Данные для редактирования
         return value;

    case Qt::CheckStateRole:  // Галочка
        if (index.column() == 10)
            return (QSqlQueryModel::data(index).toBool()) ?
                Qt::Checked : Qt::Unchecked;
        else return value;

     case Qt::TextAlignmentRole: // Выравнивание
         if (index.column() >1)
             return int(Qt::AlignRight | Qt::AlignVCenter);
         else return int(Qt::AlignLeft | Qt::AlignVCenter);

    }
    return value;
}



ModelRabDate::ModelRabDate(QObject *parent) :QSqlQueryModel(parent)
{
}

void ModelRabDate::refresh(QDate begDate, QDate endDate)
{
    QString query ("select e.id, e.first_name, e.last_name, e.middle_name, e.tabel, e.first_name||' '||e.last_name||' '||e.middle_name, oj.nam "
             "from wire_empl as e "
             "left outer join "
             "(select md.id_empl, p.nam from "
             "(select id_empl, max(dat) as mdat from (select * from wire_empl_pos_h where dat<=':d2') as th group by id_empl) as md "
             "inner join wire_empl_pos_h h on (h.id_empl=md.id_empl and h.dat=md.mdat and h.id_op<>3) "
             "inner join wire_empl_pos as p on p.id=h.id_pos) as oj on e.id=oj.id_empl "
             "inner join "
             "(select distinct j.id_rb from wire_rab_job as j where j.dat between ':d1' and ':d2' ) as r "
             "on r.id_rb=e.id "
             "order by e.first_name, e.last_name, e.middle_name");
    query.replace(":d1",begDate.toString("yyyy-MM-dd"));
    query.replace(":d2",endDate.toString("yyyy-MM-dd"));
    setQuery(query);
    if (lastError().isValid())
    {
        QMessageBox::critical(NULL,"Error",lastError().text(),QMessageBox::Cancel);
    } else {
        setHeaderData(0, Qt::Horizontal,tr("Id"));
        setHeaderData(1, Qt::Horizontal,tr("ФИО"));
    }
}
