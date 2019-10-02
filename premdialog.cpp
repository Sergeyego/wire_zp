#include "premdialog.h"
#include "ui_premdialog.h"

PremDialog::PremDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PremDialog)
{
    ui->setupUi(this);

    modelList = new DbTableModel("wire_rab_prem_n_list",this);
    modelList->addColumn("id",tr("id"),true,TYPE_INT);
    modelList->addColumn("nam",tr("Название"),false,TYPE_STRING);
    modelList->setSort("wire_rab_prem_n_list.nam");
    modelList->select();
    ui->tableViewList->setModel(modelList);
    ui->tableViewList->setColumnHidden(0,true);
    ui->tableViewList->setColumnWidth(1,200);

    modelCont = new DbTableModel("wire_rab_prem_n_cont",this);
    modelCont->addColumn("id",tr("id"),true,TYPE_INT);
    modelCont->addColumn("id_list",tr("id_list"),false,TYPE_INT);
    modelCont->addColumn("p_smen",tr("Выработка, %"),false,TYPE_DOUBLE,new QDoubleValidator(0,500,2,this));
    modelCont->addColumn("p_norm",tr("Премия, %"),false,TYPE_DOUBLE,new QDoubleValidator(0,500,2,this));
    modelCont->setSort("wire_rab_prem_n_cont.p_smen");
    ui->tableViewCont->setModel(modelCont);
    ui->tableViewCont->setColumnHidden(0,true);
    ui->tableViewCont->setColumnHidden(1,true);
    ui->tableViewCont->setColumnWidth(2,100);
    ui->tableViewCont->setColumnWidth(3,100);

    ui->tableViewLiter->verticalHeader()->setDefaultSectionSize(ui->tableViewLiter->verticalHeader()->fontMetrics().height()*1.5);
    ui->tableViewLiter->setModel(Models::instance()->modelOpNrm);
    ui->tableViewLiter->setColumnHidden(0,true);
    ui->tableViewLiter->setColumnWidth(1,350);

    modelPremK = new DbTableModel("wire_rab_prem_k",this);
    modelPremK->addColumn("id_liter",tr("id_liter"),true,TYPE_INT);
    modelPremK->addColumn("date",tr("Дата"),true,TYPE_DATE);
    modelPremK->addColumn("prem",tr("Премия, %"),false,TYPE_DOUBLE,new QDoubleValidator(0,500,2,this));
    modelPremK->setSort("wire_rab_prem_k.date");
    modelPremK->setDefaultValue(1,QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->tableViewK->setModel(modelPremK);
    ui->tableViewK->setColumnHidden(0,true);
    ui->tableViewK->setColumnWidth(1,100);
    ui->tableViewK->setColumnWidth(2,100);

    connect(ui->tableViewLiter->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(setCurrentLiter(QModelIndex)));
    connect(ui->tableViewList->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(setCurrentList(QModelIndex)));
    connect(modelList,SIGNAL(sigUpd()),Models::instance()->relPremList->model(),SLOT(refresh()));

    if (ui->tableViewList->model()->rowCount()){
        ui->tableViewList->selectRow(0);
    }

    if (ui->tableViewLiter->model()->rowCount()){
        ui->tableViewLiter->selectRow(0);
    }
}

PremDialog::~PremDialog()
{
    delete ui;
}

void PremDialog::setCurrentLiter(QModelIndex index)
{
    if (index.isValid()){
        int id_liter=ui->tableViewLiter->model()->data(ui->tableViewLiter->model()->index(index.row(),0),Qt::EditRole).toInt();
        QString liter=ui->tableViewLiter->model()->data(ui->tableViewLiter->model()->index(index.row(),1),Qt::EditRole).toString();
        ui->labelLiter->setText(liter+" :");
        modelPremK->setFilter("wire_rab_prem_k.id_liter = "+QString::number(id_liter));
        modelPremK->setDefaultValue(0,id_liter);
        modelPremK->select();
    }
}

void PremDialog::setCurrentList(QModelIndex index)
{
    if (!(index.row()==modelList->rowCount()-1 && modelList->isAdd()) && index.isValid()){
        int id_list=ui->tableViewList->model()->data(ui->tableViewList->model()->index(index.row(),0),Qt::EditRole).toInt();
        QString title=ui->tableViewList->model()->data(ui->tableViewList->model()->index(index.row(),1),Qt::EditRole).toString();
        ui->tableViewCont->setEnabled(true);
        modelCont->setFilter("wire_rab_prem_n_cont.id_list = "+QString::number(id_list));
        modelCont->setDefaultValue(1,id_list);
        modelCont->select();
        ui->groupBox->setTitle(title);
    } else {
        ui->tableViewCont->setEnabled(false);
        ui->groupBox->setTitle(tr("Не выбрано"));
    }
}

