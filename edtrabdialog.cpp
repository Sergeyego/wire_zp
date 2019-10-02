#include "edtrabdialog.h"
#include "ui_edtrabdialog.h"

EdtRabDialog::EdtRabDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EdtRabDialog)
{
    ui->setupUi(this);
    id_rab=-1;

    modelRab = new DbTableModel("wire_empl",this);
    modelRab->addColumn("id","id",true,TYPE_INT);
    modelRab->addColumn("first_name",tr("Фамилия"),false,TYPE_STRING);
    modelRab->addColumn("last_name",tr("Имя"),false,TYPE_STRING);
    modelRab->addColumn("middle_name",tr("Отчество"),false,TYPE_STRING);
    modelRab->addColumn("tabel",tr("Табель"),false,TYPE_INT);
    modelRab->setSort("wire_empl.first_name, wire_empl.last_name, wire_empl.middle_name");
    modelRab->select();
    ui->tableViewRab->setModel(modelRab);
    ui->tableViewRab->setColumnHidden(0,true);
    ui->tableViewRab->setColumnWidth(1,200);
    ui->tableViewRab->setColumnWidth(2,200);
    ui->tableViewRab->setColumnWidth(3,200);
    ui->tableViewRab->setColumnWidth(4,80);

    ui->listViewRab->setModel(Models::instance()->relRab->model());
    ui->listViewRab->setModelColumn(1);

    modelHist = new DbTableModel("wire_empl_pos_h",this);
    modelHist->addColumn("id","id",true,TYPE_INT);
    modelHist->addColumn("id_empl","id_empl",false,TYPE_INT);
    modelHist->addColumn("id_pos",tr("Должность"),false,TYPE_STRING,NULL,Models::instance()->relProf);
    modelHist->addColumn("id_op",tr("Операция"),false,TYPE_STRING,NULL,Models::instance()->relOp);
    modelHist->addColumn("dat",tr("Дата"),false,TYPE_DATE);
    modelHist->setSort("wire_empl_pos_h.dat");

    modelProf=new DbTableModel("wire_empl_pos",this);
    modelProf->addColumn("id","id",true,TYPE_INT);
    modelProf->addColumn("nam",tr("Наименование"),false,TYPE_STRING);
    modelProf->select();

    ui->tableViewProf->setModel(modelProf);
    ui->tableViewProf->setColumnHidden(0,true);
    ui->tableViewProf->setColumnWidth(1,300);

    ui->tableViewHist->setModel(modelHist);
    ui->tableViewHist->setColumnHidden(0,true);
    ui->tableViewHist->setColumnHidden(1,true);
    ui->tableViewHist->setColumnWidth(2,260);
    ui->tableViewHist->setColumnWidth(3,100);
    ui->tableViewHist->setColumnWidth(4,70);

    connect(ui->listViewRab->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updHist(QModelIndex)));
    connect(modelProf,SIGNAL(sigUpd()),Models::instance()->relProf->model(),SLOT(refresh()));
    connect(modelRab,SIGNAL(sigUpd()),Models::instance()->relRab->model(),SLOT(refresh()));
    connect(modelHist,SIGNAL(sigUpd()),this,SLOT(updStat()));
    connect(modelHist,SIGNAL(sigUpd()),Models::instance()->relRab->model(),SLOT(refresh()));
}

EdtRabDialog::~EdtRabDialog()
{
    delete ui;
}

void EdtRabDialog::updHist(QModelIndex index)
{
    //ui->lineEditFio->setText(ui->listViewRab->model()->data(ui->listViewRab->model()->index(index.row(),1)).toString());
    id_rab=ui->listViewRab->model()->data(ui->listViewRab->model()->index(index.row(),0)).toInt();
    modelHist->setFilter("wire_empl_pos_h.id_empl="+QString::number(id_rab));
    modelHist->setDefaultValue(1,id_rab);
    modelHist->select();
    updStat();
}

void EdtRabDialog::updStat()
{
    QSqlQuery query;
    query.prepare("select e.first_name||' '||e.last_name||' '||e.middle_name, oj.nam "
                  "from wire_empl as e "
                  "left outer join "
                  "(select md.id_empl, p.nam from "
                  "(select id_empl, max(dat) as mdat from (select * from wire_empl_pos_h where dat<='"+QDate::currentDate().toString("yyyy-MM-dd")+"') as th group by id_empl) as md "
                  "inner join wire_empl_pos_h h on (h.id_empl=md.id_empl and h.dat=md.mdat and h.id_op<>3) "
                  "inner join wire_empl_pos as p on p.id=h.id_pos) as oj on e.id=oj.id_empl "
                  "where e.id= "+QString::number(id_rab)+" "
                  "order by e.first_name, e.last_name, e.middle_name");
    if (query.exec()){
        while (query.next()){
            ui->lineEditFio->setText(query.value(0).toString());
            ui->lineEditStat->setText(query.value(1).toString());
        }
    } else {
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    }

}
