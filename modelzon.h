#ifndef MODELZON_H
#define MODELZON_H

#include <QObject>
#include "modelro.h"
#include "db/dbtablemodel.h"

class ModelZon : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ModelZon(QString name, QAbstractItemModel *srcModel, bool chkAll, QObject *parent = nullptr);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &item, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
    void setSel(QSet<int> set);
    QString getStr();
    QSet<int> getSel();
    void setViewColumn(int c);

public slots:
    void checkAll();
    void checkAll(bool b);
private:
    QSet<int> sel;
    bool is_checked;
    QString nam;
    bool checkFlg;
    void setCheckAll();
    int viewCol;
private slots:
    void updFinished();
signals:
    void supd();
};

#endif // MODELZON_H
