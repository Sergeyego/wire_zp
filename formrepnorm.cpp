#include "formrepnorm.h"
#include "ui_formrepnorm.h"


FormRepNorm::FormRepNorm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRepNorm)
{
    ui->setupUi(this);

    ui->dateBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEnd->setDate(QDate::currentDate());

    modelZon = new ModelZon(tr("Участки"),Rels::instance()->relZon->model(),false, this);
    QSet<int> set;
    set<<1<<2<<3<<7<<8<<9;
    modelZon->setSel(set);
    ui->tableViewZon->setModel(modelZon);
    ui->tableViewZon->setColumnWidth(0,270);

    modelJob = new ModelRepJob(this);

    ui->jobView->setModel(modelJob);

    connect(ui->cmdRefresh, SIGNAL(clicked()),this,SLOT(upd()));
    connect(ui->radioButtonEmp,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->radioButtonLine,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->radioButtonRep,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->cmdOtchPer,SIGNAL(clicked()),this,SLOT(goRep()));
    connect(modelZon,SIGNAL(supd()),this,SLOT(upd()));
    connect(ui->jobView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updTotal(QModelIndex)));
    connect(ui->tableViewZon->horizontalHeader(),SIGNAL(sectionClicked(int)),modelZon,SLOT(checkAll()));
    updSign();
}

FormRepNorm::~FormRepNorm()
{
    delete ui;
}

QString FormRepNorm::getProf(int id_rb, QDate date)
{
    QSqlQuery query;
    QString prof;
    query.prepare("select we.snam, kj.nam "
                  "from wire_empl we "
                  "left join kamin_get_empl(:date,:date) as k on k.id_empl = we.id_kamin "
                  "left join kamin_job kj on kj.id = k.id_job "
                  "where we.id = :id_rb");
    query.bindValue(":date",date);
    query.bindValue(":id_rb",id_rb);
    if (!query.exec()){
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    } else {
        if (query.next()){
            prof=query.value(0).toString()+" - "+query.value(1).toString();
        }
    }
    return prof;
}

bool FormRepNorm::getTotalVip(double &vip, int &d, double &kvo, const int id_rb)
{
    QSqlQuery query;
    query.prepare("select "
                  "sum((j.kvo*100)/get_norm_wire(j.dat,j.lid,j.chas_sm))/count(distinct j.dat), count(distinct j.dat), sum(j.kvo) "
                  "from wire_rab_job j "
                  "inner join wire_rab_nams nr on nr.lid=j.lid "
                  "inner join wire_rab_liter lt on nr.id=lt.id "
                  "where lt.is_nrm=true and j.dat between :d1 and :d2 and id_rb = :id_rb and lt.id_zon in "+modelZon->getStr());
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

void FormRepNorm::saveXls()
{
    int i=0;
    int id_rb=0, id_rb_old=-1;
    if (modelJob->rowCount()){

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
        margins.bottom=0.817361111111111;
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
        ws->setColumnWidth(2,2,18);
        ws->setColumnWidth(3,3,62);
        ws->setColumnWidth(4,4,11);
        ws->setColumnWidth(5,5,10);
        ws->setColumnWidth(6,6,10);
        ws->setColumnWidth(7,7,13);
        ws->setColumnWidth(8,8,13);
        ws->setColumnWidth(9,9,13);

        QDate date_old;
        int m_date=3;
        double sumVip=0;

        for(i=0;i<(modelJob->rowCount());i++){
            //QString emp=modelJob->data(modelJob->index(i,4),Qt::EditRole).toString();
            QDate date=modelJob->data(modelJob->index(i,1),Qt::EditRole).toDate();
            QString line=modelJob->data(modelJob->index(i,3),Qt::EditRole).toString();
            QString vid=modelJob->data(modelJob->index(i,6),Qt::EditRole).toString();
            double otr=modelJob->data(modelJob->index(i,5),Qt::EditRole).toDouble();
            double norm11=modelJob->data(modelJob->index(i,7),Qt::EditRole).toDouble();
            double norm=modelJob->data(modelJob->index(i,8),Qt::EditRole).toDouble();
            double fact=modelJob->data(modelJob->index(i,9),Qt::EditRole).toDouble();
            double vip=modelJob->data(modelJob->index(i,10),Qt::EditRole).toDouble();
            int id_ed=modelJob->data(modelJob->index(i,13),Qt::EditRole).toInt();

            id_rb=modelJob->data(modelJob->index(i,12),Qt::EditRole).toInt();

            if (id_ed!=1){
                vid+=" - "+QString::number(fact)+" "+Rels::instance()->relEd->getDisplayValue(id_ed);
            }

            if (id_rb_old!=id_rb){
                if (m>1){
                    numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
                    ws->mergeCells(CellRange(m_date,9,m-1,9),numFormat);
                    ws->writeNumeric(m_date,9,sumVip,numFormat);
                    sumVip=0;

                    ws->insertRowBreak(m);
                    ws->setRowHeight(m,m,20);
                    ws->writeString(m,1,QString("Итого"),strFormat);
                    double vip=0, kvo=0;
                    int d;
                    getTotalVip(vip,d,kvo,id_rb_old);
                    numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
                    ws->writeNumeric(m,9,vip,numFormat);
                    m++;
                }
                ws->setRowHeight(m,m+1,40);
                ws->mergeCells(CellRange(m,1,m,9),titleFormat);
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
                ws->writeString(m,9,"% Выполнения за смену",headerFormat);
                m++;
                m_date=m;
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

            if (id_ed==1){
                ws->writeNumeric(m,7,fact,numFormat);
            } else {
                ws->writeBlank(m,7,numFormat);
            }

            numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
            ws->writeNumeric(m,8,vip,numFormat);

            if (date_old!=date || id_rb_old!=id_rb){
                if (m>1){
                    numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
                    ws->mergeCells(CellRange(m_date,9,m-1,9),numFormat);
                    ws->writeNumeric(m_date,9,sumVip,numFormat);
                    sumVip=0;
                    m_date=m;
                }
            }

            sumVip+=vip;
            id_rb_old=id_rb;
            date_old=date;
            m++;
        }

        numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
        ws->mergeCells(CellRange(m_date,9,m-1,9),numFormat);
        ws->writeNumeric(m_date,9,sumVip,numFormat);
        ws->insertRowBreak(m);
        ws->setRowHeight(m,m,20);
        ws->writeString(m,1,QString("Итого"),strFormat);
        double vip=0, kvo=0;
        int d;
        getTotalVip(vip,d,kvo,id_rb_old);
        numFormat.setNumberFormat(QString("0.%1").arg((0),1,'d',0,QChar('0')));
        ws->writeNumeric(m,9,vip,numFormat);

        QString footerData=QString("&L%1").arg(signRep);
        ws->setFooterData(footerData);

        QDir dir(QDir::homePath());
        QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить файл"),
                                                        dir.path()+"/"+title+".xlsx",
                                                        QString::fromUtf8("Documents (*.xlsx)") );
        if (!filename.isEmpty()){
            xlsx.saveAs(filename);
        }
    }
}

void FormRepNorm::saveXlsPer()
{
    int i=0;
    int id_rb=0, id_rb_old=-1;
    if (modelJob->rowCount()){

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
        int otr=0, otr_old=0;

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

        for(i=0;i<(modelJob->rowCount());i++){
            QString emp=modelJob->data(modelJob->index(i,4),Qt::EditRole).toString();
            date=modelJob->data(modelJob->index(i,1),Qt::EditRole).toDate();
            smena=modelJob->data(modelJob->index(i,2),Qt::EditRole).toString();
            line=modelJob->data(modelJob->index(i,3),Qt::EditRole).toString();
            QString vid=modelJob->data(modelJob->index(i,6),Qt::EditRole).toString();
            otr=modelJob->data(modelJob->index(i,5),Qt::EditRole).toInt();
            double norm11=modelJob->data(modelJob->index(i,7),Qt::EditRole).toDouble();
            //double norm=jobmodel->data(jobmodel->index(i,8),Qt::EditRole).toDouble();
            double fact=modelJob->data(modelJob->index(i,9),Qt::EditRole).toDouble();
            double vip=modelJob->data(modelJob->index(i,10),Qt::EditRole).toDouble();
            QString prim=modelJob->data(modelJob->index(i,11),Qt::EditRole).toString();
            id_rb=modelJob->data(modelJob->index(i,12),Qt::EditRole).toInt();
            int id_ed=modelJob->data(modelJob->index(i,13),Qt::EditRole).toInt();

            if (id_ed!=1){
                vid+=" - "+QString::number(fact)+" "+Rels::instance()->relEd->getDisplayValue(id_ed);
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

        QString footerData=QString("&L%1").arg(signRep);
        ws->setFooterData(footerData);

        QString name;
        QSet<int> set = modelZon->getSel();
        if ((set.contains(1) || set.contains(2)) && !set.contains(3)){
            name=QString("волочильщики ")+fperiod;
        } else if (!(set.contains(1) || set.contains(2)) && set.contains(3)){
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
    } else if(ui->radioButtonEmp->isChecked()) {
        saveXls();
    } else {
        QString tit="Выполнение норм цех проволоки с "+ui->dateBeg->date().toString("dd.MM.yy")+" по "+ui->dateEnd->date().toString("dd.MM.yy");
        ui->jobView->save(tit,-1,true);
    }
}

void FormRepNorm::updSign()
{
    QSqlQuery query;
    query.prepare("select sign_rep from hoz where id=11");
    if (query.exec()){
        while (query.next()) {
            signRep=query.value(0).toString();
        }
    } else {
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    }
}

void FormRepNorm::upd()
{
    if (sender()==ui->cmdRefresh){
        updSign();
    }
    int typ=0;
    if (ui->radioButtonEmp->isChecked()){
        typ=1;
    } else if (ui->radioButtonRep->isChecked()){
        typ=2;
    }
    modelJob->refresh(typ, modelZon->getStr(), ui->dateBeg->date(), ui->dateEnd->date());
    ui->jobView->setColumnHidden(0,true);
    ui->jobView->setColumnHidden(12,true);
    ui->jobView->setColumnHidden(13,true);
    ui->jobView->resizeToContents();
}

ModelRepJob::ModelRepJob(QWidget *parent) : ModelRo(parent)
{

}

void ModelRepJob::refresh(int typ, QString zonSuf, QDate dbeg, QDate dend)
{
    QString qu;
    if (typ==0 || typ==1){
        QString order = (typ==1)? " order by fio, j.datf, l.snam" : " order by sm, l.id, fio, j.datf";
        qu = QString("select j.id, j.datf, "
                     "(CASE WHEN exists "
                     "(select rj.id from wire_rab_job as rj "
                     "inner join wire_rab_nams as rn on rj.lid=rn.lid "
                     "inner join wire_rab_liter as wrl on wrl.id=rn.id "
                     "where rj.dat=j.dat and rj.id_rb=j.id_rb and wrl.id_zon=6) "
                     "THEN 'ночная смена' ELSE 'дневная смена' END) as sm, "
                     "l.snam, e.first_name||' '||substr(e.last_name,1,1)||'. '||substr(e.middle_name,1,1)||'.' as fio, j.chas_sm, nr.fnam, "
                     "get_norm_wire(j.dat,j.lid,11) as norms, get_norm_wire(j.dat,j.lid,j.chas_sm) as nrm, j.kvo, "
                     "j.kvo*100/get_norm_wire(j.dat,j.lid,j.chas_sm) as vip, "
                     "j.prim, j.id_rb, lt.id_ed "
                     "from wire_rab_job j "
                     "inner join wire_line l on l.id=j.id_line "
                     "inner join wire_empl e on e.id=j.id_rb "
                     "inner join wire_rab_nams nr on nr.lid=j.lid "
                     "inner join wire_rab_liter lt on nr.id=lt.id "
                     "where j.dat between :d1 and :d2 "
                     "and lt.id_zon in "+zonSuf+order);
    } else {
        qu=QString("select null, null, null, null, we.snam, null, null, null, null, null, "
                   "sum((j.kvo*100)/get_norm_wire(j.dat,j.lid,j.chas_sm))/count(distinct j.dat), null, j.id_rb, null "
                   "from wire_rab_job j "
                   "inner join wire_empl we on we.id = j.id_rb "
                   "inner join wire_rab_nams nr on nr.lid=j.lid "
                   "inner join wire_rab_liter lt on nr.id=lt.id "
                   "where lt.is_nrm=true and j.dat between :d1 and :d2 and lt.id_zon in "+zonSuf+
                   "group by we.snam, j.id_rb order by we.snam");
    }
    QSqlQuery query;
    query.prepare(qu);
    query.bindValue(":d1",dbeg);
    query.bindValue(":d2",dend);
    if (execQuery(query)){
        setHeaderData(1, Qt::Horizontal,tr("Дата"));
        setHeaderData(2, Qt::Horizontal,tr("Смена"));
        setHeaderData(3, Qt::Horizontal,tr("Оборуд."));
        setHeaderData(4, Qt::Horizontal,tr("Ф.И.О. работника"));
        setHeaderData(5, Qt::Horizontal,tr("Отр,ч."));
        setHeaderData(6, Qt::Horizontal,tr("Задание"));
        setHeaderData(7, Qt::Horizontal,tr("Нор.11"));
        setHeaderData(8, Qt::Horizontal,tr("Норма"));
        setHeaderData(9, Qt::Horizontal,tr("Вып."));
        setHeaderData(10, Qt::Horizontal,tr("% вып."));
        setHeaderData(11, Qt::Horizontal,tr("Примечание"));
    }
}
