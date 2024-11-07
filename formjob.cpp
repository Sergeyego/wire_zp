#include "formjob.h"
#include "ui_formjob.h"

FormJob::FormJob(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormJob)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    updTempTables();

    ui->comboBoxRab->setModel(Rels::instance()->relRab->model());

    if (!Rels::instance()->relZon->isInital()){
        Rels::instance()->relZon->refreshModel();
    }
    ui->comboBoxZon->setModel(Rels::instance()->relZon->model());
    ui->comboBoxZon->setEditable(false);

    modelJob = new ModelJob(this);
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

    connect(ui->checkBoxZon,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->checkBoxRab,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->comboBoxZon,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(ui->comboBoxRab,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));

    connect(ui->cmdTruncatePrem,SIGNAL(clicked(bool)),this,SLOT(truncatePrem()));

    upd();
}

FormJob::~FormJob()
{
    delete ui;
}

bool FormJob::updTempTables()
{
    QSqlQuery query;
    query.prepare("select * from wire_rx_lists(:d)");
    query.bindValue(":d",ui->dateEditBeg->date());
    bool ok=query.exec();
    if (!ok){
        QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
    }
    return ok;
}

void FormJob::upd()
{
    if (sender()==ui->pushButtonUpd){
        ui->comboBoxZon->blockSignals(true);
        Rels::instance()->relZon->refreshModel();
        ui->comboBoxZon->blockSignals(false);
        updTempTables();
        ui->comboBoxRab->blockSignals(true);
        modelJob->refreshRelsModel();
        ui->comboBoxRab->blockSignals(false);
    }
    int id_rab = ui->checkBoxRab->isChecked() ? ui->comboBoxRab->getCurrentData().val.toInt() : -1;
    int id_zon = ui->checkBoxZon->isChecked() ? ui->comboBoxZon->getCurrentData().val.toInt() : -1;
    modelJob->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),id_rab,id_zon);
}

void FormJob::truncatePrem()
{
    TruncatePremDialog d;
    d.exec();
    modelJob->select();
}

ModelJob::ModelJob(QWidget *parent) : DbTableModel("wire_rab_job",parent)
{
    addColumn("id","id");
    addColumn("dat",tr("Дата"));
    addColumn("id_sm",tr("Смена"),Rels::instance()->relSm);
    addColumn("id_rb",tr("Работник"),Rels::instance()->relRab);
    addColumn("id_line",tr("Оборуд."),Rels::instance()->relLine);
    addColumn("lid",tr("Вид работы"),Rels::instance()->relJobNam);
    addColumn("kvo",tr("Объем"));
    addColumn("chas_sm",tr("Час/см"));
    addColumn("koef_prem_kvo",tr("К.прем"));
    addColumn("extr_time",tr("Св.ур"));
    addColumn("chas_sn",tr("Для с/н, ч"));
    addColumn("prim",tr("Примечание"));
    setSort("wire_rab_job.datf, wire_rab_job.id");
    setSuffix("inner join wire_rab_nams on wire_rab_job.lid=wire_rab_nams.lid "
              "inner join wire_rab_liter on wire_rab_nams.id=wire_rab_liter.id ");
    setDefaultValue(7,11);
    setDefaultValue(8,1);
    setDecimals(6,3);
    setDecimals(8,2);

    //refreshState();
}

void ModelJob::refresh(QDate beg, QDate end, int id_rab, int id_zon)
{
    QString flt="";
    if (id_zon>0){
        flt+=" and  wire_rab_liter.id_zon= "+QString::number(id_zon);
    }
    if (id_rab>0){
        flt+=" and  wire_rab_job.id_rb= "+QString::number(id_rab);
    }
    setFilter("wire_rab_job.dat between '"+beg.toString("yyyy-MM-dd")+"' and '"+end.toString("yyyy-MM-dd")+"'"+flt);
    setDefaultValue(1,end);
    select();
}
