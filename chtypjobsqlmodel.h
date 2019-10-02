#ifndef CHTYPJOBSQLMODEL_H
#define CHTYPJOBSQLMODEL_H

#include <QtSql>
#include <QVector>

class ChTypJobSqlModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit ChTypJobSqlModel(QObject *parent = 0);
    //Qt::ItemFlags flags(const QModelIndex &index) const;
    void refresh(bool evid, int id_vid, bool emarka, QVector<int> id_provol, bool ediam, QVector<int> id_diam, bool epack, QVector<int> id_pack);
    QVariant data(const QModelIndex &index,int role = Qt::DisplayRole) const;

private:
    bool fvid;
    bool fmarka;
    bool fdiam;
    bool fpack;
    bool notNull;
    int vidId;
    QVector<int> provolId;
    QVector<int> diamId;
    QVector<int> packId;
    
signals:
    void sUpd();
    
public slots:
    void setNotNull(bool val);
    void refreshCur();
    
};

#endif // CHTYPJOBSQLMODEL_H
