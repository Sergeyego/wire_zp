#include "tndialog.h"
#include "ui_tndialog.h"

TnDialog::TnDialog(bool nrm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TnDialog)
{
    ui->setupUi(this);
    QString t = nrm ? tr("Норма для группы") : tr("Тариф для группы");
    if (!nrm){
        ui->labelNrm->hide();
        ui->comboBoxNorm->hide();
    }
    this->setWindowTitle(t);
    ui->dateEdit->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->comboBoxNorm->setModel(Models::instance()->relPremList->model());
    ui->comboBoxNorm->setModelColumn(1);
}

TnDialog::~TnDialog()
{
    delete ui;
}

QDate TnDialog::getDate()
{
    return ui->dateEdit->date();
}

double TnDialog::getVal()
{
    return ui->lineEdit->text().toDouble();
}

int TnDialog::getIdList()
{
    return ui->comboBoxNorm->model()->data(ui->comboBoxNorm->model()->index(ui->comboBoxNorm->currentIndex(),0),Qt::EditRole).toInt();
}
