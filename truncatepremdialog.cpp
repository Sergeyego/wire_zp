#include "truncatepremdialog.h"
#include "ui_truncatepremdialog.h"

TruncatePremDialog::TruncatePremDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TruncatePremDialog)
{
    ui->setupUi(this);
    ui->dateEdit_beg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));    
    ui->dateEdit_end->setDate(QDate::currentDate());
    ui->comboBoxRab->setModel(Rels::instance()->relRab->model());
    ui->comboBoxRab->setModelColumn(1);
    connect(ui->cmdGo,SIGNAL(clicked()),this,SLOT(truncate()));
    connect(ui->cmdClose,SIGNAL(clicked()),this,SLOT(close()));
}

TruncatePremDialog::~TruncatePremDialog()
{
    delete ui;
}

void TruncatePremDialog::truncate()
{
    if (ui->lineEditKoef->text().isEmpty()) return;
    QSqlQuery query;
    query.prepare("update wire_rab_job set koef_prem_kvo= ? "
                  "where id_rb= ?  and datf>= ? and datf <= ?");
    query.addBindValue(ui->lineEditKoef->text().toDouble());
    query.addBindValue(ui->comboBoxRab->model()->data(ui->comboBoxRab->model()->index(ui->comboBoxRab->currentIndex(),0)).toInt());
    query.addBindValue(ui->dateEdit_beg->date());
    query.addBindValue(ui->dateEdit_end->date());
    query.exec();
    if (query.lastError().isValid()){
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    } else {
        QMessageBox::information(this,tr("Коэффициент установлен"),tr("Коэффициент установлен"),QMessageBox::Ok);
    }
}
