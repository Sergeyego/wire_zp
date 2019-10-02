#ifndef MODELJOB_H
#define MODELJOB_H

#include "db/dbtablemodel.h"
#include "models.h"
#include <QObject>

class ModelJob : public DbTableModel
{
    Q_OBJECT
public:
    explicit ModelJob(QDate beg, QDate end, QObject *parent);
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
public slots:
    bool select();    
    void refreshState();
    void setDateBeg(QDate val);
    void setDateEnd(QDate val);
    void setPosZon(int p);
    void setPosRb(int p);
    void setFZon(bool val);
    void setFRb(bool val);
private:
    QDate date_beg;
    QDate date_end;
    int pos_rb;
    int pos_zon;
    bool fzon;
    bool frb;
    QMap <QString,int> colorState;
};

#endif // MODELJOB_H
