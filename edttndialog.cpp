#include "edttndialog.h"
#include "ui_edttndialog.h"

EdtTNDialog::EdtTNDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EdtTNDialog)
{
    ui->setupUi(this);
    ui->comboBoxType->setModel(Models::instance()->modelJob);
    ui->comboBoxType->setModelColumn(1);

    modelMark = new ModelChk(Models::instance()->modelProvol,this);
    ui->tableViewMar->setModel(modelMark);
    ui->tableViewMar->verticalHeader()->setDefaultSectionSize(ui->tableViewMar->verticalHeader()->fontMetrics().height()*1.5);
    ui->tableViewMar->setColumnHidden(0,true);
    ui->tableViewMar->setColumnWidth(1,200);

    modelDiam = new ModelChk(Models::instance()->modelDiam,this);
    ui->tableViewDiam->setModel(modelDiam);
    ui->tableViewDiam->verticalHeader()->setDefaultSectionSize(ui->tableViewDiam->verticalHeader()->fontMetrics().height()*1.5);
    ui->tableViewDiam->setColumnHidden(0,true);
    ui->tableViewDiam->setColumnWidth(1,200);

    modelPack = new ModelChk(Models::instance()->modelPack,this);
    ui->tableViewPack->setModel(modelPack);
    ui->tableViewPack->verticalHeader()->setDefaultSectionSize(ui->tableViewPack->verticalHeader()->fontMetrics().height()*1.5);
    ui->tableViewPack->setColumnHidden(0,true);
    ui->tableViewPack->setColumnHidden(2,true);
    ui->tableViewPack->setColumnWidth(1,200);

    jobModel = new ChTypJobSqlModel(this);

    ui->tableViewJob->setModel(jobModel);
    //ui->tableViewJob->setMenuEnabled(false);
    ui->tableViewJob->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewJob->verticalHeader()->hide();
    ui->tableViewJob->setColumnHidden(0,true);
    ui->tableViewJob->setColumnWidth(1,360);
    ui->tableViewJob->setColumnWidth(2,80);
    ui->tableViewJob->setColumnWidth(3,80);

    modelTarifs=new DbTableModel("wire_rab_pay",this);
    modelTarifs->addColumn("id","id",true,TYPE_INT);
    modelTarifs->addColumn("lid","lid",false,TYPE_STRING);
    modelTarifs->addColumn("tarif",tr("Тариф"),false,TYPE_DOUBLE,new QDoubleValidator(0,1000000,2,this));
    modelTarifs->addColumn("dat",tr("Дата"),false,TYPE_DATE);
    modelTarifs->setSort("wire_rab_pay.dat");
    modelTarifs->setDefaultValue(3,QDate::currentDate().addDays(-QDate::currentDate().day()+1));

    ui->tableViewTarif->setModel(modelTarifs);
    ui->tableViewTarif->setColumnHidden(0,true);
    ui->tableViewTarif->setColumnHidden(1,true);
    ui->tableViewTarif->setColumnWidth(2,80);
    ui->tableViewTarif->setColumnWidth(3,90);

    modelNorms=new DbTableModel("wire_rab_norms",this);
    modelNorms->addColumn("id","id",true,TYPE_INT);
    modelNorms->addColumn("lid","lid",false,TYPE_STRING);
    modelNorms->addColumn("norms",tr("Норма"),false,TYPE_DOUBLE,new QDoubleValidator(0,1000000,3,this));
    modelNorms->addColumn("dat",tr("Дата"),false,TYPE_DATE);
    modelNorms->addColumn("id_list",tr("Премия за норму"),false,TYPE_STRING,NULL,Models::instance()->relPremList);
    modelNorms->setSort("wire_rab_norms.dat");
    modelNorms->setDefaultValue(3,QDate::currentDate().addDays(-QDate::currentDate().day()+1));

    ui->tableViewNorm->setModel(modelNorms);
    ui->tableViewNorm->setColumnHidden(0,true);
    ui->tableViewNorm->setColumnHidden(1,true);
    ui->tableViewNorm->setColumnWidth(2,80);
    ui->tableViewNorm->setColumnWidth(3,90);
    ui->tableViewNorm->setColumnWidth(4,170);

    connect(ui->checkBoxType,SIGNAL(clicked(bool)),ui->comboBoxType,SLOT(setEnabled(bool)));
    connect(ui->checkBoxType,SIGNAL(clicked(bool)),ui->pushButtonGrN,SLOT(setEnabled(bool)));
    connect(ui->checkBoxType,SIGNAL(clicked(bool)),ui->pushButtonGrT,SLOT(setEnabled(bool)));

    connect(ui->checkBoxMark,SIGNAL(clicked(bool)),ui->tableViewMar,SLOT(setEnabled(bool)));
    connect(ui->checkBoxDiam,SIGNAL(clicked(bool)),ui->tableViewDiam,SLOT(setEnabled(bool)));
    connect(ui->checkBoxSrc,SIGNAL(clicked(bool)),ui->tableViewPack,SLOT(setEnabled(bool)));

    connect(ui->checkBoxType,SIGNAL(clicked()),this,SLOT(refresh()));
    connect(ui->checkBoxMark,SIGNAL(clicked()),this,SLOT(refresh()));
    connect(ui->checkBoxDiam,SIGNAL(clicked()),this,SLOT(refresh()));
    connect(ui->checkBoxSrc,SIGNAL(clicked()),this,SLOT(refresh()));

    connect(ui->comboBoxType,SIGNAL(currentIndexChanged(int)),this,SLOT(refresh()));
    connect(modelMark,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(refresh()));
    connect(modelDiam,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(refresh()));
    connect(modelPack,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(refresh()));
    connect(ui->checkBoxNull,SIGNAL(clicked(bool)),jobModel,SLOT(setNotNull(bool)));

    connect(ui->tableViewJob->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updTN(QModelIndex)));
    connect(modelTarifs,SIGNAL(sigUpd()),this,SLOT(updTblJob()));
    connect(modelNorms,SIGNAL(sigUpd()),this,SLOT(updTblJob()));
    connect(ui->cmdUpdateJob,SIGNAL(clicked()),this,SLOT(updJob()));

    connect(ui->pushButtonGrT,SIGNAL(clicked(bool)),this,SLOT(setGrpTarif()));
    connect(ui->pushButtonGrN,SIGNAL(clicked(bool)),this,SLOT(setGrpNorm()));
}

EdtTNDialog::~EdtTNDialog()
{
    delete ui;
}

void EdtTNDialog::refresh()
{
    int id_vid= ui->comboBoxType->model()->data(ui->comboBoxType->model()->index(ui->comboBoxType->currentIndex(),0)).toInt();
    jobModel->refresh(ui->checkBoxType->isChecked(),id_vid, ui->checkBoxMark->isChecked(), modelMark->getVals(), ui->checkBoxDiam->isChecked(), modelDiam->getVals(), ui->checkBoxSrc->isChecked(),modelPack->getVals());
    if (ui->tableViewJob->model()->rowCount())
        ui->tableViewJob->selectRow(0);
}

void EdtTNDialog::updTN(QModelIndex index)
{
    QVariant lid=-1;
    QString lbl=tr("Не выбрано");
    if (index.isValid()){
        lid=ui->tableViewJob->model()->data(ui->tableViewJob->model()->index(index.row(),0),Qt::EditRole);
        lbl=ui->tableViewJob->model()->data(ui->tableViewJob->model()->index(index.row(),1),Qt::DisplayRole).toString();
        ui->groupBoxTarif->setEnabled(true);
        ui->groupBoxNorm->setEnabled(true);
    } else {
        ui->groupBoxTarif->setEnabled(false);
        ui->groupBoxNorm->setEnabled(false);
    }

    modelTarifs->setFilter("wire_rab_pay.lid="+lid.toString());
    modelTarifs->setDefaultValue(1,lid);
    modelTarifs->select();

    modelNorms->setFilter("wire_rab_norms.lid="+lid.toString());
    modelNorms->setDefaultValue(1,lid);
    modelNorms->select();

    ui->labelJob->setText(lbl);
}

void EdtTNDialog::updTblJob()
{
    ui->tableViewJob->selectionModel()->blockSignals(true);
    int selectrow=ui->tableViewJob->selectionModel()->currentIndex().row();
    QString old_lid=jobModel->data(jobModel->index(selectrow,0),Qt::EditRole).toString();
    jobModel->refreshCur();
    QString lid=jobModel->data(jobModel->index(selectrow,0),Qt::EditRole).toString();
    if (old_lid==lid && selectrow>=0)
        ui->tableViewJob->selectRow(selectrow);
    ui->tableViewJob->selectionModel()->blockSignals(false);
    Models::instance()->relTypeJob->model()->refresh();
}

void EdtTNDialog::updJob()
{
    QSqlQuery queryFullUpd;
    queryFullUpd.prepare("Select * from wire_rx_nam_total()");
    if (!queryFullUpd.exec()){
       QMessageBox::critical(this,"Error", queryFullUpd.lastError().text(), QMessageBox::Ok);
    } else {
       QMessageBox::information(this,tr("Обновление завершено"), tr("Обновление успешно завершено"), QMessageBox::Ok);
    }
    Models::instance()->modelJob->refresh();
    Models::instance()->relTypeJob->model()->refresh();
    refresh();
}

void EdtTNDialog::setGrpTarif()
{
    TnDialog d(false);
    bool ok;
    if (d.exec()==QDialog::Accepted){
        if (d.getVal()>0){
            for (int i=0; i<jobModel->rowCount(); i++){
                QSqlQuery query;
                query.prepare("insert into wire_rab_pay (lid, dat, tarif) values (:lid, :dat, :tarif)");
                query.bindValue(":lid",jobModel->data(jobModel->index(i,0),Qt::EditRole).toLongLong());
                query.bindValue(":dat",d.getDate());
                query.bindValue(":tarif",d.getVal());
                ok=query.exec();
                if (!ok){
                    QMessageBox::critical(this,"Error", query.lastError().text(), QMessageBox::Ok);
                    break;
                }
                //qDebug()<<jobModel->data(jobModel->index(i,0),Qt::EditRole).toString();
            }
        }
        updTblJob();
    }
}

void EdtTNDialog::setGrpNorm()
{
    TnDialog d(true);
    bool ok;
    if (d.exec()==QDialog::Accepted){
        if (d.getVal()>0){
            for (int i=0; i<jobModel->rowCount(); i++){
                QSqlQuery query;
                query.prepare("insert into wire_rab_norms (lid, dat, norms, id_list) values (:lid, :dat, :norms, :id_list)");
                query.bindValue(":lid",jobModel->data(jobModel->index(i,0),Qt::EditRole).toLongLong());
                query.bindValue(":dat",d.getDate());
                query.bindValue(":norms",d.getVal());
                query.bindValue(":id_list",d.getIdList());
                ok=query.exec();
                if (!ok){
                    QMessageBox::critical(this,"Error", query.lastError().text(), QMessageBox::Ok);
                    break;
                }
                //qDebug()<<jobModel->data(jobModel->index(i,0),Qt::EditRole).toString();
            }
        }
        updTblJob();
    }
}
