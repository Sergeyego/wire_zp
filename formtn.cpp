#include "formtn.h"
#include "ui_formtn.h"

FormTN::FormTN(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTN)
{
    ui->setupUi(this);

    sqlExecutor = new ProgressExecutor(this);

    ui->comboBoxJob->setModel(Rels::instance()->relLiter->model());
    ui->comboBoxJob->setEditable(false);

    modelMark = new ModelZon(tr("Марка"),Rels::instance()->relMark->model(),true,this);
    ui->tableViewMark->setModel(modelMark);
    ui->tableViewMark->setColumnWidth(0,200);

    modelDiam = new ModelZon(tr("Диаметр"),Rels::instance()->relDiam->model(),true,this);
    ui->tableViewDiam->setModel(modelDiam);
    ui->tableViewDiam->setColumnWidth(0,200);

    modelSpool = new ModelZon(tr("Носитель"),Rels::instance()->relSpool->model(),true,this);
    ui->tableViewSpool->setModel(modelSpool);
    ui->tableViewSpool->setColumnWidth(0,200);

    modelStat = new ModelRo(this);
    ui->tableViewStat->setModel(modelStat);
    modelStat->setDecimalForColumn(2,2);
    modelStat->setDecimalForColumn(3,3);

    modelTarifs=new DbTableModel("wire_rab_pay",this);
    modelTarifs->addColumn("id","id");
    modelTarifs->addColumn("lid","lid");
    modelTarifs->addColumn("tarif",tr("Тариф"));
    modelTarifs->addColumn("dat",tr("Дата"));
    modelTarifs->setSort("wire_rab_pay.dat");
    modelTarifs->setDefaultValue(3,QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    modelTarifs->setDecimals(2,2);

    ui->tableViewTarif->setModel(modelTarifs);
    ui->tableViewTarif->setColumnHidden(0,true);
    ui->tableViewTarif->setColumnHidden(1,true);
    ui->tableViewTarif->setColumnWidth(2,80);
    ui->tableViewTarif->setColumnWidth(3,90);

    modelNorms=new DbTableModel("wire_rab_norms",this);
    modelNorms->addColumn("id","id");
    modelNorms->addColumn("lid","lid");
    modelNorms->addColumn("norms",tr("Норма"));
    modelNorms->addColumn("dat",tr("Дата"));
    modelNorms->setSort("wire_rab_norms.dat");
    modelNorms->setDefaultValue(3,QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    modelNorms->setDecimals(2,3);

    ui->tableViewNorm->setModel(modelNorms);
    ui->tableViewNorm->setColumnHidden(0,true);
    ui->tableViewNorm->setColumnHidden(1,true);
    ui->tableViewNorm->setColumnWidth(2,80);
    ui->tableViewNorm->setColumnWidth(3,90);

    connect(ui->tableViewMark->horizontalHeader(),SIGNAL(sectionClicked(int)),modelMark,SLOT(checkAll()));
    connect(ui->tableViewDiam->horizontalHeader(),SIGNAL(sectionClicked(int)),modelDiam,SLOT(checkAll()));
    connect(ui->tableViewSpool->horizontalHeader(),SIGNAL(sectionClicked(int)),modelSpool,SLOT(checkAll()));
    connect(ui->checkBoxJob,SIGNAL(clicked(bool)),ui->comboBoxJob,SLOT(setEnabled(bool)));

    connect(modelMark,SIGNAL(supd()),this,SLOT(upd()));
    connect(modelDiam,SIGNAL(supd()),this,SLOT(upd()));
    connect(modelSpool,SIGNAL(supd()),this,SLOT(upd()));
    connect(ui->checkBoxJob,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->comboBoxJob,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(ui->checkBoxNull,SIGNAL(clicked(bool)),this, SLOT(upd()));

    connect(ui->tableViewStat->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updTN(QModelIndex)));
    connect(modelTarifs,SIGNAL(sigUpd()),this,SLOT(updStat()));
    connect(modelNorms,SIGNAL(sigUpd()),this,SLOT(updStat()));

    connect(ui->pushButtonTarif,SIGNAL(clicked(bool)),this,SLOT(setGrpTarif()));
    connect(ui->pushButtonNorm,SIGNAL(clicked(bool)),this,SLOT(setGrpNorm()));

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updFull()));
    connect(sqlExecutor,SIGNAL(finished()),this,SLOT(upd()));

    upd();
}

FormTN::~FormTN()
{
    delete ui;
}

void FormTN::upd()
{
    QString flt="where n.id_prov in "+modelMark->getStr();
    flt+=" and n.id_diam in "+modelDiam->getStr();
    flt+=" and wpk.id in "+modelSpool->getStr();
    if (ui->checkBoxJob->isChecked()){
        flt+=" and n.id = "+ui->comboBoxJob->getCurrentData().val.toString();
    }
    QString join = ui->checkBoxNull->isChecked() ? "inner" : "left";

    QSqlQuery query;
    query.prepare("select n.lid, n.fnam, t.tarif, j.norm "
                  "from wire_rab_nams as n "
                  "inner join provol p on p.id = n.id_prov "
                  "inner join diam d on d.id  = n.id_diam "
                  "inner join wire_pack_kind wpk on wpk.id = n.id_pack "+
                  join+" join wire_tarifs(:d1)  as t on t.lid=n.lid "
                  "left join wire_norms(:d2) as j on j.lid=n.lid "+flt+
                  "order by n.fnam");
    query.bindValue(":d1",QDate::currentDate());
    query.bindValue(":d2",QDate::currentDate());
    if (modelStat->execQuery(query)){
        modelStat->setHeaderData(1,Qt::Horizontal,tr("Наименование работ"));
        modelStat->setHeaderData(2,Qt::Horizontal,tr("Тариф, руб"));
        modelStat->setHeaderData(3,Qt::Horizontal,tr("Норма, т"));
        ui->tableViewStat->setColumnHidden(0,true);
        ui->tableViewStat->setColumnWidth(1,400);
        ui->tableViewStat->setColumnWidth(2,80);
        ui->tableViewStat->setColumnWidth(3,80);
        bool rk=ui->tableViewStat->model()->rowCount();
        if (rk){
            ui->tableViewStat->selectRow(0);
        }
        ui->tableViewTarif->setEnabled(rk);
        ui->tableViewNorm->setEnabled(rk);
        ui->pushButtonTarif->setEnabled(rk);
        ui->pushButtonNorm->setEnabled(rk);
    }
}

void FormTN::updStat()
{
    ui->tableViewStat->selectionModel()->blockSignals(true);
    int selectrow=ui->tableViewStat->selectionModel()->currentIndex().row();
    QString old_lid=ui->tableViewStat->model()->data(ui->tableViewStat->model()->index(selectrow,0),Qt::EditRole).toString();
    modelStat->select();
    QString lid=ui->tableViewStat->model()->data(ui->tableViewStat->model()->index(selectrow,0),Qt::EditRole).toString();
    if (old_lid==lid && selectrow>=0){
        ui->tableViewStat->selectRow(selectrow);
    }
    ui->tableViewStat->selectionModel()->blockSignals(false);
}

void FormTN::updTN(QModelIndex index)
{
    QVariant lid=-1;
    lid=ui->tableViewStat->model()->data(ui->tableViewStat->model()->index(index.row(),0),Qt::EditRole);

    modelTarifs->setFilter("wire_rab_pay.lid="+lid.toString());
    modelTarifs->setDefaultValue(1,lid);
    modelTarifs->select();

    modelNorms->setFilter("wire_rab_norms.lid="+lid.toString());
    modelNorms->setDefaultValue(1,lid);
    modelNorms->select();

}

void FormTN::setGrpTarif()
{
    TnDialog d;
    d.setWindowTitle(tr("Тариф для группы"));
    bool ok;
    if (d.exec()==QDialog::Accepted){
        if (d.getVal()>0){
            for (int i=0; i<modelStat->rowCount(); i++){
                QSqlQuery query;
                query.prepare("insert into wire_rab_pay (lid, dat, tarif) values (:lid, :dat, :tarif)");
                query.bindValue(":lid",modelStat->data(modelStat->index(i,0),Qt::EditRole).toLongLong());
                query.bindValue(":dat",d.getDate());
                query.bindValue(":tarif",d.getVal());
                ok=query.exec();
                if (!ok){
                    QMessageBox::critical(this,"Error", query.lastError().text(), QMessageBox::Ok);
                    break;
                }
            }
        }
        updStat();
        modelTarifs->select();
    }
}

void FormTN::setGrpNorm()
{
    TnDialog d;
    d.setWindowTitle(tr("Норма для группы"));
    bool ok;
    if (d.exec()==QDialog::Accepted){
        if (d.getVal()>0){
            for (int i=0; i<modelStat->rowCount(); i++){
                QSqlQuery query;
                query.prepare("insert into wire_rab_norms (lid, dat, norms) values (:lid, :dat, :norms)");
                query.bindValue(":lid",modelStat->data(modelStat->index(i,0),Qt::EditRole).toLongLong());
                query.bindValue(":dat",d.getDate());
                query.bindValue(":norms",d.getVal());
                ok=query.exec();
                if (!ok){
                    QMessageBox::critical(this,"Error", query.lastError().text(), QMessageBox::Ok);
                    break;
                }
            }
        }
        updStat();
        modelNorms->select();
    }
}

void FormTN::updFull()
{
    sqlExecutor->setQuery("Select * from wire_rx_nam_total()");
    sqlExecutor->start();
    Rels::instance()->relMark->refreshModel();
    Rels::instance()->relDiam->refreshModel();
    Rels::instance()->relSpool->refreshModel();
    ui->comboBoxJob->blockSignals(true);
    Rels::instance()->relLiter->refreshModel();
    ui->comboBoxJob->blockSignals(false);
}

