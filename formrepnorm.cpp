#include "formrepnorm.h"
#include "ui_formrepnorm.h"


FormRepNorm::FormRepNorm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRepNorm)
{
    ui->setupUi(this);

    ui->dateBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEnd->setDate(QDate::currentDate());

    ui->comboBox->setModel(Models::instance()->relSm->model());
    ui->comboBox->setModelColumn(1);
    ui->comboBox->setEnabled(false);

    zonWidget = new ZonWidget(this);
    ui->horizontalLayoutZon->addWidget(zonWidget);

    jobmodel = new JobSqlModel(this);

    proxyJobModel = new QSortFilterProxyModel(this);
    proxyJobModel->setSourceModel(jobmodel);

    ui->jobView->setModel(proxyJobModel);

    connect(ui->checkSm,SIGNAL(clicked()),this,SLOT(chSm()));
    connect(ui->cmdRefresh, SIGNAL(clicked()),this,SLOT(upd()));
    connect(ui->radioButtonEmp,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->radioButtonLine,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->cmdOtchPer,SIGNAL(clicked()),this,SLOT(goRep()));
    connect(zonWidget,SIGNAL(supd()),this,SLOT(upd()));
    connect(ui->jobView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updTotal(QModelIndex)));
}

FormRepNorm::~FormRepNorm()
{
    delete ui;
}

QString FormRepNorm::getProf(int id_rb, QDate date)
{
    QSqlQuery query;
    QString prof=Models::instance()->relRab->data(QString::number(id_rb)).toString();
    query.prepare("select r.nam , r.id_empl from ( "
                  "select md.id_empl, p.nam from "
                  "(select id_empl, max(dat) as mdat from (select * from wire_empl_pos_h where dat<= :date ) as th group by id_empl) as md "
                  "inner join wire_empl_pos_h as h on (h.id_empl=md.id_empl and h.dat=md.mdat and h.id_op<>3) "
                  "inner join wire_empl_pos as p on p.id=h.id_pos "
                  ") as r where r.id_empl = :id_rb");
    query.bindValue(":date",date);
    query.bindValue(":id_rb",id_rb);
    if (!query.exec()){
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    } else {
        while (query.next()){
            prof+=" "+query.value(0).toString();
        }
    }
    return prof;
}

bool FormRepNorm::getTotalVip(double &vip, int &d, double &kvo, const int id_rb, const int id_sm)
{
    QSqlQuery query;
    QString sufSm;
    if (id_sm!=-1){
        sufSm=QString(" and j.id_sm = "+QString::number(id_sm));
    }
    query.prepare("select "
                  "sum((j.kvo_fact*100)/((select n.norms from wire_rab_norms as n where n.dat=(select max(dat) from wire_rab_norms where dat<=j.dat and lid=j.lid) and n.lid=j.lid)*j.chas_sm/11)) "
                  "/count(distinct j.dat), count(distinct j.dat), sum(j.kvo_fact) "
                  "from wire_rab_job j "
                  "inner join wire_rab_nams nr on nr.lid=j.lid "
                  "inner join wire_rab_liter lt on nr.id=lt.id "
                  "where lt.is_nrm=true and j.dat between :d1 and :d2 and id_rb = :id_rb "+zonWidget->getSuf()+sufSm);
    query.bindValue(":d1",ui->dateBeg->date());
    query.bindValue(":d2",ui->dateEnd->date());
    query.bindValue(":id_rb",id_rb);
    bool ok=query.exec();
    if (!ok){
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    } else {
        while (query.next()){
            vip=query.value(0).toDouble();
            d=query.value(1).toInt();
            kvo=query.value(2).toDouble();
        }
    }
    return ok;
}

void FormRepNorm::chSm()
{
    ui->comboBox->setEnabled(ui->checkSm->isChecked());
}

void FormRepNorm::saveXls()
{
    int i=0;
    int id_rb=0, id_rb_old=-1;
    if (jobmodel->rowCount()){

        QString title=QString("Выполнение норм с %1 по %2").arg(ui->dateBeg->date().toString("dd.MM.yy")).arg(ui->dateEnd->date().toString("dd.MM.yy"));

        Document xlsx;
        Worksheet *ws=xlsx.currentWorksheet();
        XlsxPageSetup pageSetup;
        pageSetup.fitToPage=true;
        pageSetup.fitToWidth=1;
        pageSetup.fitToHeight=0;
        pageSetup.orientation=XlsxPageSetup::portrait;
        ws->setPageSetup(pageSetup);

        XlsxPageMargins margins=ws->pageMargins();
        margins.bottom=0.928472222222222;
        ws->setPageMargins(margins);

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
        ws->setColumnWidth(1,1,10);
        ws->setColumnWidth(2,2,15);
        ws->setColumnWidth(3,3,50);
        ws->setColumnWidth(4,4,11);
        ws->setColumnWidth(5,5,10);
        ws->setColumnWidth(6,6,10);
        ws->setColumnWidth(7,7,13);
        ws->setColumnWidth(8,8,13);

        for(i=0;i<(jobmodel->rowCount());i++){
            //QString emp=jobmodel->data(jobmodel->index(i,4),Qt::EditRole).toString();
            QDate date=jobmodel->data(jobmodel->index(i,1),Qt::EditRole).toDate();
            QString line=jobmodel->data(jobmodel->index(i,3),Qt::EditRole).toString();
            QString vid=jobmodel->data(jobmodel->index(i,6),Qt::EditRole).toString();
            double otr=jobmodel->data(jobmodel->index(i,5),Qt::EditRole).toDouble();
            double norm11=jobmodel->data(jobmodel->index(i,7),Qt::EditRole).toDouble();
            double norm=jobmodel->data(jobmodel->index(i,8),Qt::EditRole).toDouble();
            double fact=jobmodel->data(jobmodel->index(i,9),Qt::EditRole).toDouble();
            double vip=jobmodel->data(jobmodel->index(i,10),Qt::EditRole).toDouble();

            id_rb=jobmodel->data(jobmodel->index(i,12),Qt::EditRole).toInt();
            if (id_rb_old!=id_rb){
                if (m>1){
                    ws->insertRowBreak(m);
                    ws->setRowHeight(m,m,20);
                    ws->writeString(m,1,QString("Итого"),strFormat);
                    double vip, kvo;
                    int d;
                    getTotalVip(vip,d,kvo,id_rb_old);
                    numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
                    ws->writeNumeric(m,8,vip,numFormat);
                    m++;
                }
                ws->setRowHeight(m,m+1,40);
                ws->mergeCells(CellRange(m,1,m,8),titleFormat);
                ws->writeString(m,1,title+"\n"+getProf(id_rb,ui->dateEnd->date()) ,titleFormat);
                m++;
                ws->writeString(m,1,"Дата",headerFormat);
                ws->writeString(m,2,"Оборудование",headerFormat);
                ws->writeString(m,3,"Вид работ",headerFormat);
                ws->writeString(m,4,"Отработал часов",headerFormat);
                ws->writeString(m,5,"Норма за 11 ч, т",headerFormat);
                ws->writeString(m,6,"Норма за отраб. Часы, т",headerFormat);
                ws->writeString(m,7,"Фактическое выполнение, т",headerFormat);
                ws->writeString(m,8,"% Выполнения",headerFormat);
                m++;
            }
            ws->setRowHeight(m,m,20);
            ws->writeString(m,1,date.toString("dd.MM.yy"),strFormat);
            ws->writeString(m,2,line,strFormat);
            ws->writeString(m,3,vid,strFormat);

            numFormat.setNumberFormat("0");
            ws->writeNumeric(m,4,otr,numFormat);

            numFormat.setNumberFormat(QString("0.%1").arg((0),3,'d',0,QChar('0')));
            ws->writeNumeric(m,5,norm11,numFormat);

            ws->writeNumeric(m,6,norm,numFormat);
            ws->writeNumeric(m,7,fact,numFormat);

            numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
            ws->writeNumeric(m,8,vip,numFormat);

            id_rb_old=id_rb;            
            m++;
        }
        ws->insertRowBreak(m);
        ws->setRowHeight(m,m,20);
        ws->writeString(m,1,QString("Итого"),strFormat);
        double vip, kvo;
        int d;
        getTotalVip(vip,d,kvo,id_rb_old);
        numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
        ws->writeNumeric(m,8,vip,numFormat);

        QString footerData=QString("&L%1").arg(Models::instance()->sign());
        ws->setFooterData(footerData);

        QDir dir(QDir::homePath());
        QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить файл"),
                                                        dir.path()+"/"+title+".xlsx",
                                                        QString::fromUtf8("Documents (*.xlsx)") );
        if (!filename.isEmpty()){
            if (filename.right(5)!=".xlsx"){
                filename+=".xlsx";
            }
            xlsx.saveAs(filename);
        }
    }
}

void FormRepNorm::saveXlsPer()
{
    int i=0;
    int id_rb=0, id_rb_old=-1;
    if (jobmodel->rowCount()){

        QString period, af11, fperiod;
        if (ui->dateBeg->date()==ui->dateEnd->date()){
            period="за "+ui->dateBeg->date().toString("d MMMM yyyy")+"г.";
            fperiod=ui->dateBeg->date().toString("d MMMM yyyy")+"г.";
            af11=" за 11ч.";
        } else {
            period="с "+ui->dateBeg->date().toString("dd.MM.yy")+" по "+ui->dateEnd->date().toString("dd.MM.yy");
            fperiod=period;
        }

        QString title=QString("Отчет работы структурного подразделения \"Цех по производству сварочной проволоки\" ")+period;

        Document xlsx;
        Worksheet *ws=xlsx.currentWorksheet();
        XlsxPageSetup pageSetup;
        pageSetup.fitToPage=true;
        pageSetup.fitToWidth=1;
        if (ui->dateBeg->date()==ui->dateEnd->date()){
            pageSetup.fitToHeight=1;
        } else {
            pageSetup.fitToHeight=0;
        }
        pageSetup.orientation=XlsxPageSetup::landscape;
        ws->setPageSetup(pageSetup);

        XlsxPageMargins margins=ws->pageMargins();
        margins.bottom=0.970138888888889;
        ws->setPageMargins(margins);

        QFont defaultFont("Arial", 10);
        QFont titleFont("Arial", 10);
        titleFont.setBold(true);

        Format strFormat;
        strFormat.setBorderStyle(Format::BorderThin);
        strFormat.setFont(defaultFont);
        strFormat.setVerticalAlignment(Format::AlignVCenter);
        Format numFormat;
        numFormat.setBorderStyle(Format::BorderThin);
        numFormat.setFont(defaultFont);
        numFormat.setVerticalAlignment(Format::AlignVCenter);
        Format sumFormat=numFormat;
        sumFormat.setFont(titleFont);

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

        int m=1, m_beg=1, m_line=1, m_otr=1;
        QString smena, smena_old;
        QString line, line_old;
        QDate date, date_old;
        int otr, otr_old;

        ws->setColumnWidth(1,1,16);
        ws->setColumnWidth(2,2,15);
        ws->setColumnWidth(3,3,51);
        ws->setColumnWidth(4,4,11);
        ws->setColumnWidth(5,5,10);
        ws->setColumnWidth(6,6,13);
        ws->setColumnWidth(7,7,13);
        ws->setColumnWidth(8,8,13);
        ws->setColumnWidth(9,9,13);
        ws->setColumnWidth(10,10,40);

        ws->mergeCells(CellRange(m,1,m,10),titleFormat);
        ws->setRowHeight(m,m,50);
        ws->writeString(m,1,title,titleFormat);
        m++;

        for(i=0;i<(jobmodel->rowCount());i++){
            QString emp=jobmodel->data(jobmodel->index(i,4),Qt::EditRole).toString();
            date=jobmodel->data(jobmodel->index(i,1),Qt::EditRole).toDate();
            smena=jobmodel->data(jobmodel->index(i,2),Qt::EditRole).toString();
            line=jobmodel->data(jobmodel->index(i,3),Qt::EditRole).toString();
            QString vid=jobmodel->data(jobmodel->index(i,6),Qt::EditRole).toString();
            otr=jobmodel->data(jobmodel->index(i,5),Qt::EditRole).toInt();
            double norm11=jobmodel->data(jobmodel->index(i,7),Qt::EditRole).toDouble();
            //double norm=jobmodel->data(jobmodel->index(i,8),Qt::EditRole).toDouble();
            double fact=jobmodel->data(jobmodel->index(i,9),Qt::EditRole).toDouble();
            double vip=jobmodel->data(jobmodel->index(i,10),Qt::EditRole).toDouble();
            QString prim=jobmodel->data(jobmodel->index(i,11),Qt::EditRole).toString();
            id_rb=jobmodel->data(jobmodel->index(i,12),Qt::EditRole).toInt();
            int id_ed=jobmodel->data(jobmodel->index(i,13),Qt::EditRole).toInt();

            if (id_ed!=1){
                vid+=" - "+QString::number(fact)+" "+Models::instance()->relEd->data(QString::number(id_ed)).toString();
            }

            if (smena_old!=smena){
                if (m!=2){
                    ws->mergeCells(CellRange(m_line,2,m-1,2),strFormat);
                    ws->writeString(m_line,2,line_old,strFormat);//Оборуд
                    ws->mergeCells(CellRange(m_beg,1,m-1,1),strFormat);
                    numFormat.setNumberFormat("0");
                    ws->mergeCells(CellRange(m_otr,4,m-1,4),numFormat);
                    ws->mergeCells(CellRange(m_beg,7,m-1,7),sumFormat);
                    ws->mergeCells(CellRange(m_beg,9,m-1,9),sumFormat);
                    double total=0, kvo=0;
                    int day=0;
                    getTotalVip(total,day,kvo,id_rb_old);
                    sumFormat.setNumberFormat(QString("0.%1").arg((0),3,'d',0,QChar('0')));
                    ws->writeNumeric(m_beg,7,kvo,sumFormat);//выполнение итого
                    sumFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
                    ws->writeNumeric(m_beg,9,total,sumFormat);//общий процент
                }
                ws->mergeCells(CellRange(m,1,m,10),titleFormat);
                ws->setRowHeight(m,m,50);
                ws->writeString(m,1,smena,titleFormat);
                m++;

                ws->setRowHeight(m,m,50);
                ws->writeString(m,1,"ФИО работника",headerFormat);
                ws->writeString(m,2,"Оборудование",headerFormat);
                ws->writeString(m,3,"Вид работ",headerFormat);
                ws->writeString(m,4,"Кол-во отработанных часов",headerFormat);
                ws->writeString(m,5,"Норма за 11 ч (т)",headerFormat);
                ws->writeString(m,6,"Фактическое выполнение по маркам"+af11+" (т)",headerFormat);
                ws->writeString(m,7,"Фактическое выполнение"+af11+" (т)",headerFormat);
                ws->writeString(m,8,"% выполнения по маркам"+af11,headerFormat);
                ws->writeString(m,9,"Общий % выполнения"+af11,headerFormat);
                ws->writeString(m,10,"Примечание",headerFormat);
                m++;
                m_beg=m, m_line=m, m_otr=m;
            }

            if (ui->dateBeg->date()!=ui->dateEnd->date()){
                vid=date.toString("dd.MM.yy")+" "+vid;
            }

            if (line_old!=line || smena_old!=smena){
                ws->mergeCells(CellRange(m_line,2,m-1,2),strFormat);
                ws->writeString(m_line,2,line_old,strFormat);//Оборуд
                m_line=m;
            }

            if (id_rb_old!=id_rb|| smena_old!=smena){
                ws->mergeCells(CellRange(m_beg,1,m-1,1),strFormat);
                ws->mergeCells(CellRange(m_beg,7,m-1,7),sumFormat);
                ws->mergeCells(CellRange(m_beg,9,m-1,9),sumFormat);
                double total=0, kvo=0;
                int day=0;
                getTotalVip(total,day,kvo,id_rb_old);
                sumFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
                ws->writeNumeric(m_beg,9,total,sumFormat);//общий процент
                sumFormat.setNumberFormat(QString("0.%1").arg((0),3,'d',0,QChar('0')));
                ws->writeNumeric(m_beg,7,kvo,sumFormat);//выполнение итого
                m_beg=m;
            }

            if (date_old!=date || id_rb_old!=id_rb || otr_old!=otr|| smena_old!=smena){
                numFormat.setNumberFormat("0");
                ws->mergeCells(CellRange(m_otr,4,m-1,4),numFormat);
                m_otr=m;
            }

            ws->setRowHeight(m,m,20);
            ws->writeString(m,1,emp,strFormat);//фио
            ws->writeString(m,3,vid,strFormat);//вид работ
            numFormat.setNumberFormat("0");
            ws->writeNumeric(m,4,otr,numFormat);//к-во часов
            ws->writeString(m,5,smena,strFormat);//смена
            numFormat.setNumberFormat(QString("0.%1").arg((0),3,'d',0,QChar('0')));
            ws->writeNumeric(m,5,norm11,numFormat);//норма за 11 часов
            if (id_ed==1){
                ws->writeNumeric(m,6,fact,numFormat);//факт. выполнение
            } else {
                ws->writeBlank(m,6,numFormat);
            }
            numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
            ws->writeNumeric(m,8,vip,numFormat);//выполнение по маркам
            ws->writeString(m,10,prim,strFormat);//примечание

            smena_old=smena;
            id_rb_old=id_rb;
            line_old=line;
            date_old=date;
            otr_old=otr;
            m++;
        }

        ws->mergeCells(CellRange(m_line,2,m-1,2),strFormat);
        ws->writeString(m_line,2,line_old,strFormat);//Оборуд
        ws->mergeCells(CellRange(m_beg,1,m-1,1),strFormat);
        numFormat.setNumberFormat("0");
        ws->mergeCells(CellRange(m_otr,4,m-1,4),numFormat);
        ws->mergeCells(CellRange(m_beg,7,m-1,7),sumFormat);
        ws->mergeCells(CellRange(m_beg,9,m-1,9),sumFormat);
        double total=0, kvo=0;
        int day=0;
        getTotalVip(total,day,kvo,id_rb_old);
        sumFormat.setNumberFormat(QString("0.%1").arg((0),3,'d',0,QChar('0')));
        ws->writeNumeric(m_beg,7,kvo,sumFormat);//выполнение итого
        sumFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
        ws->writeNumeric(m_beg,9,total,sumFormat);//общий процент

        QString footerData=QString("&L%1").arg(Models::instance()->signRep());
        ws->setFooterData(footerData);

        QString name;
        if (zonWidget->vol() && !zonWidget->namoch()){
            name=QString("волочильщики ")+fperiod;
        } else if (zonWidget->namoch() && !zonWidget->vol()){
            name=QString("намотчики ")+fperiod;
        } else {
            name=QString("Отчет работы структурного подразделения цех по производству сварочной проволоки ")+period;
        }

        QDir dir(QDir::homePath());
        QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить файл"),
                                                        dir.path()+"/"+name+".xlsx",
                                                        QString::fromUtf8("Documents (*.xlsx)") );
        if (!filename.isEmpty()){
            if (filename.right(5)!=".xlsx"){
                filename+=".xlsx";
            }
            xlsx.saveAs(filename);
        }
    }
}

void FormRepNorm::updTotal(QModelIndex index)
{
    int id_rb=ui->jobView->model()->data(ui->jobView->model()->index(index.row(),12),Qt::EditRole).toInt();
    int id_sm=-1;

    if (ui->checkSm->isChecked()){
        id_sm=ui->comboBox->model()->data(ui->comboBox->model()->index(ui->comboBox->currentIndex(),0),Qt::EditRole).toInt();
    }

    double vip=0, kvo=0;
    int d=0;
    getTotalVip(vip,d,kvo,id_rb);
    ui->lineEditVip->setText(QLocale().toString(vip,'f',2));
    ui->lineEditDay->setText(QString::number(d));
}

void FormRepNorm::goRep()
{
    if (ui->radioButtonLine->isChecked()){
        saveXlsPer();
    } else {
        saveXls();
    }
}

void FormRepNorm::upd()
{
    Models::instance()->refreshOrg();
    bool by_line=ui->radioButtonLine->isChecked();
    int id_sm=ui->comboBox->model()->data(ui->comboBox->model()->index(ui->comboBox->currentIndex(),0),Qt::EditRole).toInt();
    jobmodel->refresh(by_line, zonWidget->getSuf(),ui->checkSm->isChecked(),id_sm,ui->dateBeg->date(),ui->dateEnd->date());
    ui->jobView->setColumnHidden(0,true);
    ui->jobView->setColumnWidth(1,70);
    ui->jobView->setColumnWidth(2,100);
    ui->jobView->setColumnWidth(3,105);
    ui->jobView->setColumnWidth(4,130);
    ui->jobView->setColumnWidth(5,50);
    ui->jobView->setColumnWidth(6,360);
    ui->jobView->setColumnWidth(7,60);
    ui->jobView->setColumnWidth(8,60);
    ui->jobView->setColumnWidth(9,60);
    ui->jobView->setColumnWidth(10,60);
    ui->jobView->setColumnWidth(11,240);
    ui->jobView->setColumnHidden(12,true);
    ui->jobView->setColumnHidden(13,true);
}
