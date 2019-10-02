#include "jobtypedialog.h"
#include "ui_jobtypedialog.h"

JobTypeDialog::JobTypeDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JobTypeDialog)
{
    ui->setupUi(this);
    jobModel = new DbTableModel("wire_rab_liter",this);
    jobModel->addColumn("id","id",true,TYPE_INT);
    jobModel->addColumn("naim",tr("Наименоване"),false,TYPE_STRING);
    jobModel->addColumn("id_ed",tr("Ед.изм."),false,TYPE_STRING,NULL,Models::instance()->relEd);
    jobModel->addColumn("id_zon",tr("Участок"),false,TYPE_STRING,NULL,Models::instance()->relZon);
    jobModel->addColumn("id_conn",tr("Вид связи"),false,TYPE_STRING,NULL,Models::instance()->relConn);
    jobModel->addColumn("is_nrm",tr("Нормир."),false,TYPE_BOOL);
    jobModel->addColumn("k_dopl",tr("К.допл."),false,TYPE_DOUBLE,new QDoubleValidator(0,100,2,this));
    jobModel->setSort("wire_rab_liter.id_zon, wire_rab_liter.naim");
    jobModel->select();
    ui->tableView->setModel(jobModel);
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnWidth(1,320);
    ui->tableView->setColumnWidth(2,60);
    ui->tableView->setColumnWidth(3,150);
    ui->tableView->setColumnWidth(4,290);
    ui->tableView->setColumnWidth(5,70);
    ui->tableView->setColumnWidth(6,70);
    ui->tableView->setColumnWidth(7,70);

    connect(ui->cmdUpd,SIGNAL(clicked()),this,SLOT(updJob()));
    connect(jobModel,SIGNAL(sigUpd()),Models::instance()->modelJob,SLOT(refresh()));
    connect(jobModel,SIGNAL(sigUpd()),Models::instance()->modelOpNrm,SLOT(refresh()));
}

JobTypeDialog::~JobTypeDialog()
{
    delete ui;
}

void JobTypeDialog::updJob()
{
    QSqlQuery queryFullUpd;
    queryFullUpd.prepare("Select * from wire_rx_nam_total()");
    if (!queryFullUpd.exec()){
       QMessageBox::critical(this,"Error", queryFullUpd.lastError().text(), QMessageBox::Ok);
    } else {
       QMessageBox::information(this,tr("Обновление завершено"), tr("Обновление успешно завершено"), QMessageBox::Ok);
    }
    Models::instance()->relTypeJob->model()->refresh();
}
