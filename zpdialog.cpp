#include "zpdialog.h"
#include "ui_zpdialog.h"

ZpDialog::ZpDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ZpDialog)
{
    ui->setupUi(this);
    loadSettings();
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    modelRabDate = new ModelRabDate(this);
    modelRabDate->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
    ui->listViewRab->setModel(modelRabDate);
    ui->listViewRab->setModelColumn(5);
    ui->listViewRab->setCurrentIndex(ui->listViewRab->model()->index(0,0));

    zpmodel=new zpSqlModel;
    ui->tableViewZp->setModel(zpmodel);
    setModelDate();

    ui->tableViewZp->setColumnWidth(0,65);
    ui->tableViewZp->setColumnWidth(1,300);
    ui->tableViewZp->setColumnWidth(2,55);
    ui->tableViewZp->setColumnWidth(3,65);
    ui->tableViewZp->setColumnWidth(4,70);
    ui->tableViewZp->setColumnWidth(5,70);
    ui->tableViewZp->setColumnWidth(6,70);
    ui->tableViewZp->setColumnWidth(7,55);
    ui->tableViewZp->setColumnWidth(8,65);
    ui->tableViewZp->setColumnWidth(9,70);
    ui->tableViewZp->setColumnWidth(10,65);

    connect(ui->cmdCalc,SIGNAL(clicked()),this,SLOT(setModelDate()));
    connect(ui->listViewRab->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updTable()));
    connect(ui->cmdTabel,SIGNAL(clicked()),this,SLOT(genTabel()));
    connect(ui->cmdTabelShort,SIGNAL(clicked()),this,SLOT(genTabelShort()));

}

ZpDialog::~ZpDialog()
{
    saveSettings();
    delete ui;
}


void ZpDialog::updTable()
{
   int id_rb=ui->listViewRab->model()->data(ui->listViewRab->model()->index(ui->listViewRab->currentIndex().row(),0)).toInt();
   zpmodel->refresh(id_rb);
   ui->lineEditZp->clear();
   ui->lineEditPremKach->clear();
   ui->lineEditpremNorm->clear();
   ui->lineEditItogo->clear();

   QSqlQuery query;
   query.prepare("select sum(zp)+sum(extrtime), sum(premk), sum(premn), sum(total) from wire_calc_zp_rab(:id_rab,:dbeg,:dend)");
   query.bindValue(":id_rab", id_rb);
   query.bindValue(":dbeg", zpmodel->getDbeg());
   query.bindValue(":dend", zpmodel->getDend());
   if (query.exec()){
       while (query.next()) {
            ui->lineEditZp->setText(QLocale().toString(query.value(0).toDouble(),'f',2));
            ui->lineEditPremKach->setText(QLocale().toString(query.value(1).toDouble(),'f',2));
            ui->lineEditpremNorm->setText(QLocale().toString(query.value(2).toDouble(),'f',2));
            ui->lineEditItogo->setText(QLocale().toString(query.value(3).toDouble(),'f',2));
       }
   } else {
       QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
   }
}

void ZpDialog::setModelDate()
{
   modelRabDate->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
   zpmodel->setdate(ui->dateEditBeg->date(),ui->dateEditEnd->date());
   updTable();
}

void ZpDialog::genTabel()
{
    int rabcount=ui->listViewRab->model()->rowCount();
    int id_rab;
    bool ok=true;
    bool sn, pp;
    QString sError;
    QString firstName, lastName, middleName, prof;
    int kvoDay;
    double zp, dopl, extrtime, premk, premn, total;
    QSqlQuery query;

    QProgressDialog* pprd = new QProgressDialog(tr("Идет формирование списка..."),"", 0, rabcount, this);
    pprd->setCancelButton(0);
    pprd->setMinimumDuration(0);
    pprd->setWindowTitle(tr("Пожалуйста, подождите"));

    QString title=QString("Начисление заработной платы с %1 по %2").arg(ui->dateEditBeg->date().toString("dd.MM.yy")).arg(ui->dateEditEnd->date().toString("dd.MM.yy"));

    Document xlsx;
    Worksheet *ws=xlsx.currentWorksheet();
    XlsxPageSetup pageSetup;
    pageSetup.fitToPage=true;
    pageSetup.fitToWidth=1;
    pageSetup.fitToHeight=0;
    pageSetup.orientation=XlsxPageSetup::landscape;
    ws->setPageSetup(pageSetup);
    QFont defaultFont("Arial", 10);
    QFont titleFont("Arial", 10);
    titleFont.setBold(true);

    XlsxPageMargins margins=ws->pageMargins();
    margins.top=0.664583333333333;
    margins.bottom=0.817361111111111;
    ws->setPageMargins(margins);

    Format strFormat;
    strFormat.setBorderStyle(Format::BorderThin);
    strFormat.setFont(defaultFont);
    strFormat.setTextWarp(true);

    Format numFormat;
    numFormat.setBorderStyle(Format::BorderThin);
    numFormat.setFont(defaultFont);
    Format titleFormat;
    titleFormat.setBorderStyle(Format::BorderNone);
    titleFormat.setFont(titleFont);
    titleFormat.setTextWarp(true);
    titleFormat.setHorizontalAlignment(Format::AlignHCenter);
    titleFormat.setVerticalAlignment(Format::AlignVCenter);

    Format headerFormat;
    headerFormat.setBorderStyle(Format::BorderThin);
    headerFormat.setFont(titleFont);
    headerFormat.setTextWarp(true);
    headerFormat.setHorizontalAlignment(Format::AlignHCenter);
    headerFormat.setVerticalAlignment(Format::AlignVCenter);

    int m=1;
    ws->setColumnWidth(1,1,4);
    ws->setColumnWidth(2,2,15);
    ws->setColumnWidth(3,3,15);
    ws->setColumnWidth(4,4,15);
    ws->setColumnWidth(5,5,20);
    ws->setColumnWidth(6,6,11);
    ws->setColumnWidth(7,7,6);
    ws->setColumnWidth(8,8,9);
    ws->setColumnWidth(9,9,9);
    ws->setColumnWidth(10,10,10);
    ws->setColumnWidth(11,11,10);
    ws->setColumnWidth(12,12,10);
    ws->setColumnWidth(13,13,10);
    ws->setColumnWidth(14,14,10);
    ws->setColumnWidth(15,15,10);
    ws->setColumnWidth(16,16,10);

    for (int i=0; i<rabcount; i++){
        QCoreApplication::processEvents();
        pprd->setValue(i);
        id_rab=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,0)).toInt();
        firstName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,1)).toString();
        lastName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,2)).toString();
        middleName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,3)).toString();
        prof=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,6)).toString();

        ws->mergeCells(CellRange(m,1,m+1,1),headerFormat);
        ws->writeString(m,1,QString("№"),headerFormat);
        ws->mergeCells(CellRange(m,2,m,4),headerFormat);
        ws->writeString(m,2,QString("Фамилия Имя Отчество"),headerFormat);

        ws->mergeCells(CellRange(m+1,2,m+1,4),headerFormat);
        ws->writeString(m+1,2,QString("Наименование работ"),headerFormat);

        ws->mergeCells(CellRange(m,5,m+1,5),headerFormat);
        ws->writeString(m,5,QString("Профессия, разряд"),headerFormat);

        ws->mergeCells(CellRange(m,6,m+1,6),headerFormat);
        ws->writeString(m,6,QString("Начислено"),headerFormat);

        ws->mergeCells(CellRange(m,7,m+1,7),headerFormat);
        ws->writeString(m,7,QString("Р. дн."),headerFormat);

        ws->mergeCells(CellRange(m,8,m+1,8),headerFormat);
        ws->writeString(m,8,QString("Кол-во"),headerFormat);

        ws->mergeCells(CellRange(m,9,m+1,9),headerFormat);
        ws->writeString(m,9,QString("Тариф"),headerFormat);

        ws->mergeCells(CellRange(m,10,m+1,10),headerFormat);
        ws->writeString(m,10,QString("Зар.пл."),headerFormat);

        ws->mergeCells(CellRange(m,11,m+1,11),headerFormat);
        ws->writeString(m,11,QString("В т.ч.допл"),headerFormat);

        ws->mergeCells(CellRange(m,12,m+1,12),headerFormat);
        ws->writeString(m,12,QString("В средн за день"),headerFormat);

        ws->mergeCells(CellRange(m,13,m+1,13),headerFormat);
        ws->writeString(m,13,QString("Сверх уроч."),headerFormat);

        ws->mergeCells(CellRange(m,14,m,15),headerFormat);
        ws->writeString(m,14,QString("Премия"),headerFormat);
        ws->writeString(m+1,14,QString("Кач"),headerFormat);
        ws->writeString(m+1,15,QString("Норм"),headerFormat);

        ws->mergeCells(CellRange(m,16,m+1,16),headerFormat);
        ws->writeString(m,16,QString("К выдаче"),headerFormat);

        m+=2;
        query.prepare("select count(distinct datf), sum(zp) as zp, sum(dopl) as dopl, sum(extrtime) as ex, "
                      "sum(premk) as premk, sum(premn) as prenn, sum(total) as total "
                      "from wire_calc_zp_rab(:id_rab, :dbeg, :dend)");
        query.bindValue(":id_rab", id_rab);
        query.bindValue(":dbeg", zpmodel->getDbeg());
        query.bindValue(":dend", zpmodel->getDend());
        if (query.exec()){
            while (query.next()) {
                 kvoDay=query.value(0).toDouble();
                 zp=query.value(1).toDouble();
                 dopl=query.value(2).toDouble();
                 extrtime=query.value(3).toDouble();
                 premk=query.value(4).toDouble();
                 premn=query.value(5).toDouble();
                 total=query.value(6).toDouble();
            }
        } else {
            ok=false;
            sError=query.lastError().text();
            break;
        }

        if (!kvoDay) continue;

        ws->setRowHeight(m,m,35);

        numFormat.setNumberFormat("0");
        ws->writeNumeric(m,1,i+1,numFormat);

        ws->writeString(m,2,firstName,strFormat);
        ws->writeString(m,3,lastName,strFormat);
        ws->writeString(m,4,middleName,strFormat);
        ws->writeString(m,5,prof,strFormat);

        numFormat.setNumberFormat(QString("0.%1").arg((0),2,'d',0,QChar('0')));
        ws->writeNumeric(m,6,total,numFormat);

        numFormat.setNumberFormat("0");
        ws->writeNumeric(m,7,kvoDay,numFormat);

        ws->writeBlank(m,8,strFormat);
        ws->writeBlank(m,9,strFormat);
        ws->writeBlank(m,10,strFormat);
        ws->writeBlank(m,11,strFormat);

        if (kvoDay!=0){
            numFormat.setNumberFormat(QString("0.%1").arg((0),2,'d',0,QChar('0')));
            ws->writeNumeric(m,12,total/kvoDay,numFormat);
        } else {
            ws->writeBlank(m,12,strFormat);
        }

        ws->writeBlank(m,13,strFormat);
        ws->writeBlank(m,14,strFormat);
        ws->writeBlank(m,15,strFormat);
        ws->writeBlank(m,16,strFormat);

        m++;

        query.prepare("select z.nrab, z.kvo, z.tarif, z.zp, z.dopl, z.ex, z.premk, z.premn, z.total, z.sn, z.sm, p.is_cored "
                      "from ( "
                      "select nrab, sum(kvo) as kvo, tarif, sum(zp) as zp, sum(dopl) as dopl, sum(extrtime) as ex, "
                      "sum(premk) as premk, sum(premn) as premn, sum(total) as total, sum(COALESCE(sn,0)) as sn, sum(chas_sm) as sm, lid, "
                      "CASE WHEN (COALESCE(sn,0)<>0) THEN true ELSE false END as sob "
                      "from wire_calc_zp_rab(:id_rab, :dbeg, :dend) group by lid, nrab, tarif, sob "
                      ") as z "
                      "inner join wire_rab_nams as n on n.lid=z.lid "
                      "inner join provol as p on p.id=n.id_prov");
        query.bindValue(":id_rab", id_rab);
        query.bindValue(":dbeg", zpmodel->getDbeg());
        query.bindValue(":dend", zpmodel->getDend());

        int begm=m;
        if (query.exec()){
            while (query.next()) {

                QString nam=query.value(0).toString();
                QString chas=query.value(9).toString()+tr(" ч.");

                sn=query.value(9).toInt()>0;
                pp=query.value(11).toBool();
                if (sn) nam+=QString("*** (с/н ")+chas+QString(")");
                if (pp) nam+=QString("** (порошковая ")+chas+QString(")");

                ws->writeBlank(m,1,numFormat);

                ws->mergeCells(CellRange(m,2,m,7),numFormat);
                ws->writeString(m,2,nam,strFormat);

                numFormat.setNumberFormat(QString("0.%1").arg((0),3,'d',0,QChar('0')));
                ws->writeNumeric(m,8,query.value(1).toDouble(),numFormat);

                numFormat.setNumberFormat(QString("0.%1").arg((0),2,'d',0,QChar('0')));
                ws->writeNumeric(m,9,query.value(2).toDouble(),numFormat);
                ws->writeNumeric(m,10,query.value(3).toDouble(),numFormat);
                ws->writeNumeric(m,11,query.value(4).toDouble(),numFormat);
                ws->writeBlank(m,12,numFormat);
                ws->writeNumeric(m,13,query.value(5).toDouble(),numFormat);
                ws->writeNumeric(m,14,query.value(6).toDouble(),numFormat);
                ws->writeNumeric(m,15,query.value(7).toDouble(),numFormat);
                ws->writeNumeric(m,16,query.value(8).toDouble(),numFormat);

                m++;
            }
        } else {
            ok=false;
            sError=query.lastError().text();
            break;
        }

        ws->mergeCells(CellRange(begm,1,m-1,1),strFormat);
        ws->writeString(m,9,QString("ИТОГО:"),strFormat);

        numFormat.setNumberFormat(QString("0.%1").arg((0),2,'d',0,QChar('0')));
        ws->writeNumeric(m,10,zp,numFormat);
        ws->writeNumeric(m,11,dopl,numFormat);
        ws->writeBlank(m,12,numFormat);
        ws->writeNumeric(m,13,extrtime,numFormat);
        ws->writeNumeric(m,14,premk,numFormat);
        ws->writeNumeric(m,15,premn,numFormat);
        ws->writeNumeric(m,16,total,numFormat);

        m++;
        ws->insertRowBreak(m);
        m++;
    }

    QString year=QString::number(ui->dateEditBeg->date().year());
    QString month=QDate::longMonthName(ui->dateEditBeg->date().month(),QDate::StandaloneFormat);
    QString dat=month+" "+year+QString(" г.");

    QString headerData=QString("&LРасчет заработной платы за %1&C%2&R&D").arg(dat).arg(Models::instance()->org());
    QString footerData=QString("&L%1").arg(Models::instance()->sign());

    ws->setHeaderData(headerData);
    ws->setFooterData(footerData);

    delete pprd;
    if (!ok) {
        QMessageBox::critical(this,"Error",sError,QMessageBox::Cancel);
    } else {
        QDir dir(QDir::homePath());
        QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить файл"),
                                                        dir.path()+"/"+title+".xlsx",
                                                        QString::fromUtf8("Documents (*.xlsx)") );
        if (!filename.isEmpty()){
            xlsx.saveAs(filename);
        }
    }
}

void ZpDialog::genTabelShort()
{
    int rabcount=ui->listViewRab->model()->rowCount();
    int id_rab;
    bool ok=true;
    QString sError;
    QString firstName, lastName, middleName, prof;
    int kvoDay;
    double zp, ex, premk, premn, total;
    QSqlQuery query;

    QProgressDialog* pprd = new QProgressDialog(tr("Идет формирование списка..."),"", 0, rabcount, this);
    pprd->setCancelButton(0);
    pprd->setMinimumDuration(0);
    pprd->setWindowTitle(tr("Пожалуйста, подождите"));

    QString title=QString("Начисление заработной платы с %1 по %2").arg(ui->dateEditBeg->date().toString("dd.MM.yy")).arg(ui->dateEditEnd->date().toString("dd.MM.yy"));

    Document xlsx;
    Worksheet *ws=xlsx.currentWorksheet();
    XlsxPageSetup pageSetup;
    pageSetup.fitToPage=true;
    pageSetup.fitToWidth=1;
    pageSetup.fitToHeight=0;
    pageSetup.orientation=XlsxPageSetup::landscape;
    ws->setPageSetup(pageSetup);
    QFont defaultFont("Arial", 10);
    QFont titleFont("Arial", 10);
    titleFont.setBold(true);

    Format strFormat;
    strFormat.setBorderStyle(Format::BorderThin);
    strFormat.setFont(defaultFont);
    Format numFormat;
    numFormat.setBorderStyle(Format::BorderThin);
    numFormat.setFont(defaultFont);
    Format titleFormat;
    titleFormat.setBorderStyle(Format::BorderNone);
    titleFormat.setFont(titleFont);
    titleFormat.setTextWarp(true);
    titleFormat.setHorizontalAlignment(Format::AlignHCenter);
    titleFormat.setVerticalAlignment(Format::AlignVCenter);

    Format headerFormat;
    headerFormat.setBorderStyle(Format::BorderThin);
    headerFormat.setFont(titleFont);
    headerFormat.setTextWarp(true);
    headerFormat.setHorizontalAlignment(Format::AlignHCenter);
    headerFormat.setVerticalAlignment(Format::AlignVCenter);

    int m=1;
    ws->setColumnWidth(1,1,15);
    ws->setColumnWidth(2,2,15);
    ws->setColumnWidth(3,3,15);
    ws->setColumnWidth(4,4,40);
    ws->setColumnWidth(5,5,13);
    ws->setColumnWidth(6,6,13);
    ws->setColumnWidth(7,7,13);
    ws->setColumnWidth(8,8,13);
    ws->setColumnWidth(9,9,13);
    ws->setColumnWidth(10,10,13);

    ws->setRowHeight(m,m+1,40);
    ws->mergeCells(CellRange(m,1,m,10),titleFormat);
    ws->writeString(m,1,title,titleFormat);
    m++;
    ws->writeString(m,1,"Фамилия",headerFormat);
    ws->writeString(m,2,"Имя",headerFormat);
    ws->writeString(m,3,"Отчество",headerFormat);
    ws->writeString(m,4,"Профессия, разряд",headerFormat);
    ws->writeString(m,5,"Отработал дней",headerFormat);
    ws->writeString(m,6,"Зарплата",headerFormat);
    ws->writeString(m,7,"Св. уроч.",headerFormat);
    ws->writeString(m,8,"Премия за качество",headerFormat);
    ws->writeString(m,9,"Премия за норму",headerFormat);
    ws->writeString(m,10,"К выдаче",headerFormat);
    m++;

    for (int i=0; i<rabcount; i++){
        QCoreApplication::processEvents();
        pprd->setValue(i);
        id_rab=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,0),Qt::EditRole).toInt();
        firstName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,1)).toString();
        lastName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,2)).toString();
        middleName=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,3)).toString();
        prof=ui->listViewRab->model()->data(ui->listViewRab->model()->index(i,6)).toString();

        query.prepare("select count(distinct datf), sum(zp) as zp, sum(extrtime) as ex, "
                      "sum(premk) as premk, sum(premn) as premn, sum(total) as total "
                      "from wire_calc_zp_rab(:id_rab, :dbeg, :dend)");
        query.bindValue(":id_rab", id_rab);
        query.bindValue(":dbeg", zpmodel->getDbeg());
        query.bindValue(":dend", zpmodel->getDend());
        if (query.exec()){
            while (query.next()) {
                 kvoDay=query.value(0).toInt();
                 zp=query.value(1).toDouble();
                 ex=query.value(2).toDouble();
                 premk=query.value(3).toDouble();
                 premn=query.value(4).toDouble();
                 total=query.value(5).toDouble();
            }
        } else {
            ok=false;
            sError=query.lastError().text();
            break;
        }

        if (!kvoDay) continue;

        ws->writeString(m,1,firstName,strFormat);
        ws->writeString(m,2,lastName,strFormat);
        ws->writeString(m,3,middleName,strFormat);
        ws->writeString(m,4,prof,strFormat);
        numFormat.setNumberFormat("0");
        ws->writeNumeric(m,5,kvoDay,numFormat);
        numFormat.setNumberFormat(QString("0.%1").arg((0),2,'d',0,QChar('0')));
        ws->writeNumeric(m,6,zp,numFormat);
        ws->writeNumeric(m,7,ex,numFormat);
        ws->writeNumeric(m,8,premk,numFormat);
        ws->writeNumeric(m,9,premn,numFormat);
        ws->writeNumeric(m,10,total,numFormat);
        m++;

    }

    ws->write(m,6,QString("=SUM(F3:F%1)").arg(m-1),numFormat);
    ws->write(m,7,QString("=SUM(G3:G%1)").arg(m-1),numFormat);
    ws->write(m,8,QString("=SUM(H3:H%1)").arg(m-1),numFormat);
    ws->write(m,9,QString("=SUM(I3:I%1)").arg(m-1),numFormat);
    ws->write(m,10,QString("=SUM(J3:J%1)").arg(m-1),numFormat);

    delete pprd;

    if (!ok) {
        QMessageBox::critical(this,"Error",sError,QMessageBox::Cancel);
    } else {
        QDir dir(QDir::homePath());
        QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить файл"),
                                                        dir.path()+"/"+title+".xlsx",
                                                        QString::fromUtf8("Documents (*.xlsx)") );
        if (!filename.isEmpty()){
            xlsx.saveAs(filename);
        }
    }
}

void ZpDialog::loadSettings()
{
    QSettings settings("szsm", "wire_zp");
    this->restoreGeometry(settings.value("zp_geometry").toByteArray());

}

void ZpDialog::saveSettings()
{
    QSettings settings("szsm", "wire_zp");
    settings.setValue("zp_geometry", this->saveGeometry());
}
