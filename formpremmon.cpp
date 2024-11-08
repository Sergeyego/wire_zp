#include "formpremmon.h"
#include "ui_formpremmon.h"

FormPremMon::FormPremMon(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPremMon)
{
    ui->setupUi(this);

    ui->listViewZon->setModel(Rels::instance()->relZon->model());
    ui->listViewZon->setModelColumn(1);

    modelPremNorm = new DbTableModel("wire_rab_prem_n",this);
    modelPremNorm->addColumn("id_zon",tr("id_zon"));
    modelPremNorm->addColumn("dat",tr("Дата"));
    modelPremNorm->addColumn("id_premn",tr("Показатель"),Rels::instance()->relList);
    modelPremNorm->setSort("wire_rab_prem_n.dat");
    ui->tableViewNorm->setModel(modelPremNorm);
    ui->tableViewNorm->setColumnHidden(0,true);
    ui->tableViewNorm->setColumnWidth(1,100);
    ui->tableViewNorm->setColumnWidth(2,200);

    modelList = new DbTableModel("wire_rab_prem_n_list");
    modelList->addColumn("id",tr("id"));
    modelList->addColumn("nam",tr("Название"));
    modelList->setSort("wire_rab_prem_n_list.id");
    ui->tableViewList->setModel(modelList);
    ui->tableViewList->setColumnHidden(0,true);
    ui->tableViewList->setColumnWidth(1,200);
    mapper = new DbMapper(ui->tableViewList,this);
    mapper->addMapping(ui->lineEditNam,1);
    ui->horizontalLayoutMap->insertWidget(0,mapper);
    mapper->addEmptyLock(ui->tableViewCont);

    modelListData = new DbTableModel("wire_rab_prem_n_cont",this);
    modelListData->addColumn("id",tr("id"));
    modelListData->addColumn("id_list",tr("id_list"));
    modelListData->addColumn("p_smen",tr("Процент выполнения"));
    modelListData->addColumn("p_norm",tr("Премия норма, %"));
    modelListData->addColumn("p_kach",tr("Премия качество, %"));
    modelListData->addColumn("p_mon",tr("Премия ежемес., %"));
    modelListData->setSort("wire_rab_prem_n_cont.p_smen");
    ui->tableViewCont->setModel(modelListData);
    ui->tableViewCont->setColumnHidden(0,true);
    ui->tableViewCont->setColumnHidden(1,true);
    ui->tableViewCont->setColumnWidth(2,150);
    ui->tableViewCont->setColumnWidth(3,150);
    ui->tableViewCont->setColumnWidth(4,150);
    ui->tableViewCont->setColumnWidth(5,150);

    connect(Rels::instance()->relZon->model(),SIGNAL(searchFinished(QString)),this,SLOT(updZonFinished()));
    connect(ui->listViewZon->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updZonCont(QModelIndex)));

    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updListCont(int)));
    connect(modelList,SIGNAL(sigUpd()),Rels::instance()->relList,SLOT(refreshModel()));

    connect(ui->radioButtonNorm,SIGNAL(clicked(bool)),this,SLOT(setPageNorm()));
    connect(ui->radioButtonList,SIGNAL(clicked(bool)),this,SLOT(setPageList()));

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));

    if (!Rels::instance()->relZon->isInital()){
        Rels::instance()->relZon->refreshModel();
    } else {
        updZonFinished();
    }

    modelList->select();

}

FormPremMon::~FormPremMon()
{
    delete ui;
}

void FormPremMon::upd()
{
    modelList->select();
    Rels::instance()->relZon->refreshModel();
    Rels::instance()->relList->refreshModel();
}

void FormPremMon::updZonFinished()
{
    if (ui->listViewZon->model()->rowCount()){
        ui->listViewZon->setCurrentIndex(ui->listViewZon->model()->index(0,1));
    }
}

void FormPremMon::updZonCont(QModelIndex ind)
{
    int id_zon = ui->listViewZon->model()->data(ui->listViewZon->model()->index(ind.row(),0),Qt::EditRole).toInt();
    modelPremNorm->setFilter("wire_rab_prem_n.id_zon = "+QString::number(id_zon));
    modelPremNorm->setDefaultValue(0,id_zon);
    modelPremNorm->setDefaultValue(1,QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    modelPremNorm->select();
}

void FormPremMon::updListCont(int ind)
{
    int id_list = mapper->modelData(ind,0).toInt();
    modelListData->setFilter("wire_rab_prem_n_cont.id_list = "+QString::number(id_list));
    modelListData->setDefaultValue(1,id_list);
    modelListData->select();
}

void FormPremMon::setPageNorm()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void FormPremMon::setPageList()
{
    ui->stackedWidget->setCurrentIndex(1);
}
