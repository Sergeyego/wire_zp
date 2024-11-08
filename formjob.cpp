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
    chkPrim();

    connect(ui->checkBoxRab,SIGNAL(clicked(bool)),ui->comboBoxRab,SLOT(setEnabled(bool)));
    connect(ui->checkBoxZon,SIGNAL(clicked(bool)),ui->comboBoxZon,SLOT(setEnabled(bool)));
    connect(ui->checkBoxPrim,SIGNAL(clicked(bool)),this,SLOT(chkPrim()));

    connect(ui->checkBoxZon,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->checkBoxRab,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->comboBoxZon,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(ui->comboBoxRab,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));

    connect(ui->tableViewJob->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(chkRab(int)));
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
        modelJob->refreshState();
    }
    int id_rab = ui->checkBoxRab->isChecked() ? ui->comboBoxRab->getCurrentData().val.toInt() : -1;
    int id_zon = ui->checkBoxZon->isChecked() ? ui->comboBoxZon->getCurrentData().val.toInt() : -1;
    modelJob->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),id_rab,id_zon);
    if (ui->tableViewJob->model()->rowCount()){
        ui->tableViewJob->setCurrentIndex(ui->tableViewJob->model()->index(ui->tableViewJob->model()->rowCount()-1,1));
        ui->tableViewJob->scrollToBottom();
    }
    ui->tableViewJob->setFocus();
}

void FormJob::truncatePrem()
{
    TruncatePremDialog d;
    d.exec();
    modelJob->select();
}

void FormJob::chkPrim()
{
    ui->tableViewJob->setColumnHidden(11,!ui->checkBoxPrim->isChecked());
}

void FormJob::chkRab(int row)
{
    if (row==3){
        modelJob->submit();
        int id_brig = modelJob->getIdBrig()>0 ? -1 : ui->tableViewJob->model()->data(ui->tableViewJob->model()->index(ui->tableViewJob->currentIndex().row(),row),Qt::EditRole).toInt();
        modelJob->setIdBrig(id_brig);
        if (id_brig>0 && ui->checkBoxRab->isChecked()){
            ui->checkBoxRab->setChecked(false);
            ui->checkBoxRab->clicked();
        } else {
            upd();
        }
    }
}

ModelJob::ModelJob(QWidget *parent) : DbTableModel("wire_rab_job",parent)
{
    stateExecutor = new Executor(this);
    id_brig=-1;
    dBeg=QDate::currentDate().addDays(-QDate::currentDate().day()+1);
    connect(stateExecutor,SIGNAL(finished()),this,SLOT(stFinished()));
    refreshState();

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
}

QVariant ModelJob::data(const QModelIndex &index, int role) const
{
    if (index.column()==9){
        if (role==Qt::CheckStateRole){
            return this->data(index,Qt::EditRole).toBool() ? Qt::Checked : Qt::Unchecked;
        }
        if (role==Qt::DisplayRole){
            return this->data(index,Qt::EditRole).toBool() ? tr("Да") : tr("Нет");
        }
        if (role==Qt::TextAlignmentRole){
            return Qt::AlignLeft;
        }
    }
    if (role==Qt::BackgroundRole){
        int area = colorState.value(DbTableModel::data(this->index(index.row(),5),Qt::EditRole).toString());
        if(area == 0) return QVariant(QColor(255,170,170)); else
            if(area == 1) return QVariant(QColor(Qt::yellow)); else
                if(area == 2) return QVariant(QColor(255,200,100));
        if (index.column()==3 && id_brig>0){
            return QVariant(QColor(255,255,175));
        }
    }
    return DbTableModel::data(index,role);
}

void ModelJob::refresh(QDate beg, QDate end, int id_rab, int id_zon)
{
    dBeg=beg;
    QString flt="";
    if (id_zon>0){
        flt+=" and  wire_rab_liter.id_zon= "+QString::number(id_zon);
    }
    if (id_rab>0){
        id_brig=-1;
        flt+=" and  wire_rab_job.id_rb= "+QString::number(id_rab);
    }
    if (id_brig>0){
        flt+=" and  wire_rab_job.id_rb= "+QString::number(id_brig);
    }
    setFilter("wire_rab_job.dat between '"+beg.toString("yyyy-MM-dd")+"' and '"+end.toString("yyyy-MM-dd")+"'"+flt);
    setDefaultValue(1,end);
    select();
}

void ModelJob::setIdBrig(int id)
{
    id_brig=id;
    if (id_brig>0){
        setDefaultValue(3,id_brig);
    } else {
        setDefaultValue(3,this->nullVal(3));
    }
}

int ModelJob::getIdBrig()
{
    return id_brig;
}

void ModelJob::refreshState()
{
    QString query = QString("select r.lid, t.tarif, n.norm, l.is_nrm from wire_rab_nams as r "
                            "inner join wire_rab_liter as l on r.id=l.id "
                            "left outer join wire_tarifs('%1') as t on r.lid=t.lid "
                            "left outer join wire_norms('%2') as n on r.lid=n.lid").arg(dBeg.toString("yyyy-MM-dd")).arg(dBeg.toString("yyyy-MM-dd"));
    stateExecutor->setQuery(query);
    stateExecutor->start();
}

void ModelJob::stFinished()
{
    QVector<QVector<QVariant>> data = stateExecutor->getData();
    colorState.clear();
    for (QVector<QVariant> row : data){
        bool is_nrm=row.at(3).toBool();
        bool trf=row.at(1).toDouble()!=0.0;
        bool nrm=row.at(2).toDouble()!=0.0;
        colorState[row.at(0).toString()]=(trf ? 1 : 0)+((nrm && is_nrm)||(!is_nrm) ? 2 : 0);
    }
    emit dataChanged(this->index(0,0),this->index(this->rowCount()-1,this->columnCount()-1));
}
