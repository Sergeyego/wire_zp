#ifndef MODELCHK_H
#define MODELCHK_H

#include <QObject>
#include <QSqlQueryModel>
#include <QMap>
#include <QVector>

class ModelChk: public QSqlQueryModel
{
    Q_OBJECT
public:
    ModelChk(QSqlQueryModel *src, QObject *parent);
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVector<int> getVals();
private:
    QSqlQueryModel *modelSrc;
    QMap <int,bool> chkMap;
};

#endif // MODELCHK_H
