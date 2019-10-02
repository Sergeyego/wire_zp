#include "jobsqlmodel.h"
#include <QtGui>


JobSqlModel::JobSqlModel(QObject *parent) :
    QSqlQueryModel(parent)
{
}

void JobSqlModel::refresh(bool emp, QString zonSuf, bool fsm, int idSm, QDate dbeg, QDate dend)
{
    begdate=dbeg;
    enddate=dend;
    smId=idSm;
    fSm=fsm;
    QString sufSm;
    if (fsm) {
        sufSm=" and j.id_sm= "+QString::number(idSm);
    }else {
        sufSm="";
    }
    QString order= emp? " order by j.id_sm, l.snam, fio, j.datf" : " order by fio, j.datf, j.id_sm, l.id";
    setQuery("select j.id, j.datf, s.fname, l.snam, e.first_name||' '||substr(e.last_name,1,1)||'. '||substr(e.middle_name,1,1)||'.' as fio, j.chas_sm, nr.fnam, "
             "(select n.norms from wire_rab_norms as n where n.dat=(select max(dat) from wire_rab_norms where dat<=j.dat and lid=j.lid) and n.lid=j.lid) as norms, "
             "(select n.norms from wire_rab_norms as n where n.dat=(select max(dat) from wire_rab_norms where dat<=j.dat and lid=j.lid) and n.lid=j.lid)*j.chas_sm/11 as nrm, j.kvo, "
             "j.kvo*100/((select n.norms from wire_rab_norms as n where n.dat=(select max(dat) from wire_rab_norms where dat<=j.dat and lid=j.lid) and n.lid=j.lid)*j.chas_sm/11) as vip, "
             "j.prim, j.id_rb, lt.id_ed "
             "from wire_rab_job j "
             "inner join wire_smena s on s.id=j.id_sm "
             "inner join wire_line l on l.id=j.id_line "
             "inner join wire_empl e on e.id=j.id_rb "
             "inner join wire_rab_nams nr on nr.lid=j.lid "
             "inner join wire_rab_liter lt on nr.id=lt.id "
             "where j.dat between '"+begdate.toString("yyyy.MM.dd")+"' and '"
             +enddate.toString("yyyy.MM.dd")+"'"+sufSm+zonSuf+order);
    if (lastError().isValid())
    {
        QMessageBox* errmes= new QMessageBox("Error", lastError().text(), QMessageBox::Critical,NULL,QMessageBox::Cancel,NULL);
        errmes->exec();
        delete errmes;
    } else {
        setHeaderData(1, Qt::Horizontal,tr("Дата"));
        setHeaderData(2, Qt::Horizontal,tr("Смена"));
        setHeaderData(3, Qt::Horizontal,tr("Оборуд."));
        setHeaderData(4, Qt::Horizontal,tr("Ф.И.О. работника"));
        setHeaderData(5, Qt::Horizontal,tr("Отр,ч."));
        setHeaderData(6, Qt::Horizontal,tr("Задание"));
        setHeaderData(7, Qt::Horizontal,tr("Нор.11"));
        setHeaderData(8, Qt::Horizontal,tr("Норма"));
        setHeaderData(9, Qt::Horizontal,tr("Вып."));
        setHeaderData(10, Qt::Horizontal,tr("% вып."));
        setHeaderData(11, Qt::Horizontal,tr("Примечание"));
        emit sigUpd();
    }
}

QVariant JobSqlModel::data(const QModelIndex &index,int role) const
{
    QVariant value = QSqlQueryModel::data(index, role);
    switch (role) {
    case Qt::DisplayRole: // Данные для отображения
    {
        if (index.column()==1) {
            return QSqlQueryModel::data(index,Qt::EditRole).toDate().toString("dd.MM.yy");
        }
        if (index.column()>=7 && index.column()<=9) {
            return QString("%1").arg(QSqlQueryModel::data(index,Qt::EditRole).toDouble(),0,'f',3);
        }
        if (index.column()==10) {
            return value.isNull() ? QString("-") : QString("%1").arg(value.toDouble(),0,'f',2);
        } else return value;
    }
    case Qt::TextAlignmentRole: // Выравнивание
        if(index.column() == 5 || (index.column()>=7 && index.column()<=10) )
            return int(Qt::AlignRight | Qt::AlignVCenter);
        else return int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    return value;
}

