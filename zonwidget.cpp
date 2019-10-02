#include "zonwidget.h"

ZonCheckBox::ZonCheckBox(const QString &text, QWidget *parent) : QCheckBox(text,parent)
{

}

ZonWidget::ZonWidget(QWidget *parent) :
    QWidget(parent)
{
    QLabel *zlab = new QLabel(tr("Зоны:"));
    this->setLayout(new QVBoxLayout);
    this->layout()->addWidget(zlab);
    QSqlQuery query;
    int i=0;
    int id=0;
    query.prepare("select id, nam from wire_rab_zon order by nam");
    if (query.exec()){
        N=query.size();
        zonCheckBox = new ZonCheckBox*[N];
        while (query.next()) {
            zonCheckBox[i]= new ZonCheckBox(query.value(1).toString());
            id=query.value(0).toInt();
            zonCheckBox[i]->setId(id);
            if (id==1 || id==2 || id==5) zonCheckBox[i]->setChecked(true);
            this->layout()->addWidget(zonCheckBox[i]);
            i++;
        }
    } else qDebug()<<query.lastError().text();
    for(i=0;i<N;i++)
        connect(zonCheckBox[i],SIGNAL(clicked()),this,SLOT(check()));
}

QString ZonWidget::getSuf()
{
    QString suf="and (lt.id_zon=0 ";
    for (int i=0; i<N; i++)
        if (zonCheckBox[i]->isChecked()){
            suf+="or lt.id_zon= "+QString::number(zonCheckBox[i]->getId())+" ";
        }

    suf+=")";
    return suf;
}

bool ZonWidget::vol()
{
    bool b=false;
    for (int i=0; i<N; i++)
        if (zonCheckBox[i]->getId()==1 || zonCheckBox[i]->getId()==2){
            if(zonCheckBox[i]->isChecked()){
                b=true;
            }
        }
    return b;
}

bool ZonWidget::namoch()
{
    bool b=false;
    for (int i=0; i<N; i++)
        if (zonCheckBox[i]->getId()==3){
            if(zonCheckBox[i]->isChecked()){
                b=true;
            }
        }
    return b;
}

void ZonWidget::check()
{
    emit supd();
}

