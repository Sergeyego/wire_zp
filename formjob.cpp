#include "formjob.h"
#include "ui_formjob.h"

FormJob::FormJob(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormJob)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());
    ui->comboBoxRab->setModel(Models::instance()->relRab->proxyModel());
    ui->comboBoxRab->setModelColumn(1);
    ui->comboBoxZon->setModel(Models::instance()->relZon->model());
    ui->comboBoxZon->setModelColumn(1);

    modelJob = new ModelJob(ui->dateEditBeg->date(),ui->dateEditEnd->date(),this);
    modelJob->select();
    ui->tableViewJob->setModel(modelJob);
    ui->tableViewJob->setColumnHidden(0,true);
    ui->tableViewJob->setColumnWidth(1,80);
    ui->tableViewJob->setColumnWidth(2,80);
    ui->tableViewJob->setColumnWidth(3,240);
    ui->tableViewJob->setColumnWidth(4,100);
    ui->tableViewJob->setColumnWidth(5,320);
    ui->tableViewJob->setColumnWidth(6,60);
    ui->tableViewJob->setColumnWidth(7,60);
    ui->tableViewJob->setColumnWidth(8,60);
    ui->tableViewJob->setColumnWidth(9,70);
    ui->tableViewJob->setColumnWidth(10,70);
    ui->tableViewJob->setColumnWidth(11,240);

    connect(ui->checkBoxRab,SIGNAL(clicked(bool)),ui->comboBoxRab,SLOT(setEnabled(bool)));
    connect(ui->checkBoxZon,SIGNAL(clicked(bool)),ui->comboBoxZon,SLOT(setEnabled(bool)));

    connect(ui->dateEditBeg,SIGNAL(dateChanged(QDate)),modelJob,SLOT(setDateBeg(QDate)));
    connect(ui->dateEditEnd,SIGNAL(dateChanged(QDate)),modelJob,SLOT(setDateEnd(QDate)));
    connect(ui->checkBoxZon,SIGNAL(clicked(bool)),modelJob,SLOT(setFZon(bool)));
    connect(ui->checkBoxRab,SIGNAL(clicked(bool)),modelJob,SLOT(setFRb(bool)));
    connect(ui->comboBoxZon,SIGNAL(currentIndexChanged(int)),modelJob,SLOT(setPosZon(int)));
    connect(ui->comboBoxRab,SIGNAL(currentIndexChanged(int)),modelJob,SLOT(setPosRb(int)));
    connect(ui->cmdTruncatePrem,SIGNAL(clicked(bool)),this,SLOT(truncatePrem()));
}

FormJob::~FormJob()
{
    delete ui;
}

void FormJob::truncatePrem()
{
    TruncatePremDialog d;
    d.exec();
    modelJob->select();
}

void FormJob::updStat()
{
    modelJob->refreshState();
}
