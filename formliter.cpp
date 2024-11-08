#include "formliter.h"
#include "ui_formliter.h"

FormLiter::FormLiter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLiter)
{
    ui->setupUi(this);

    sqlExecutor = new ProgressExecutor(this);

    modelLiter = new DbTableModel("wire_rab_liter", this);
    modelLiter->addColumn("id","id");
    modelLiter->addColumn("naim",tr("Наименоване"));
    modelLiter->addColumn("id_ed",tr("Ед.изм."),Rels::instance()->relEd);
    modelLiter->addColumn("id_zon",tr("Участок"),Rels::instance()->relZon);
    modelLiter->addColumn("id_conn",tr("Вид связи"),Rels::instance()->relConn);
    modelLiter->addColumn("is_nrm",tr("Нормир."));
    modelLiter->addColumn("k_dopl",tr("К.допл."));
    modelLiter->setDecimals(6,2);

    ui->tableViewLiter->setModel(modelLiter);
    ui->tableViewLiter->setColumnHidden(0,true);
    ui->tableViewLiter->setColumnWidth(1,320);
    ui->tableViewLiter->setColumnWidth(2,60);
    ui->tableViewLiter->setColumnWidth(3,150);
    ui->tableViewLiter->setColumnWidth(4,290);
    ui->tableViewLiter->setColumnWidth(5,70);
    ui->tableViewLiter->setColumnWidth(6,70);
    ui->tableViewLiter->setColumnWidth(7,70);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->pushButtonFullUpd,SIGNAL(clicked(bool)),this,SLOT(fullUpd()));
    connect(ui->radioButtonNam,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->radioButtonZon,SIGNAL(clicked(bool)),this,SLOT(upd()));

    upd();
}

FormLiter::~FormLiter()
{
    delete ui;
}

void FormLiter::upd()
{
    if (sender()==ui->pushButtonUpd){
        modelLiter->refreshRelsModel();
    }
    if (ui->radioButtonNam->isChecked()){
        modelLiter->setSort("wire_rab_liter.naim");
    } else {
        modelLiter->setSort("wire_rab_zon.nam, wire_rab_liter.naim");
    }
    modelLiter->select();
}

void FormLiter::fullUpd()
{
    sqlExecutor->setQuery("select * from wire_rx_nam_total()");
    sqlExecutor->start();
    modelLiter->refreshRelsModel();
    upd();
}
