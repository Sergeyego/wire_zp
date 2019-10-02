#ifndef ZPSQLMODEM_H
#define ZPSQLMODEM_H

#include <QtSql>
#include <QMessageBox>

class zpSqlModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit zpSqlModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,int role = Qt::DisplayRole) const;
    void refresh(int id_rb);
    void setdate(QDate begDate, QDate endDate);
    QDate getDbeg() {return dBeg;}
    QDate getDend() {return dEnd;}
private:
    QDate dBeg;
    QDate dEnd;
    
signals:
    
public slots:
    
};

class ModelRabDate : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit ModelRabDate(QObject *parent = 0);
    void refresh(QDate begDate, QDate endDate);
};

#endif // ZPSQLMODEM_H
