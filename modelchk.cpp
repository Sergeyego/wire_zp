#include "modelchk.h"

ModelChk::ModelChk(QSqlQueryModel *src, QObject *parent) :modelSrc(src), QSqlQueryModel(parent)
{
}

QVariant ModelChk::data(const QModelIndex &item, int role) const
{
    if (role==Qt::CheckStateRole && item.column()==1){
        return chkMap.value(item.row(),false) ? Qt::Checked : Qt::Unchecked;
    }
    return modelSrc->data(item,role);
}

int ModelChk::rowCount(const QModelIndex &parent) const
{
    return modelSrc->rowCount(parent);
}

int ModelChk::columnCount(const QModelIndex &parent) const
{
    return modelSrc->columnCount(parent);
}

bool ModelChk::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok=false;
    if (index.column()==1 && role==Qt::CheckStateRole){
        chkMap[index.row()]=value.toBool();
        ok=true;
        emit dataChanged(index,index);
    }
    return ok;
}

Qt::ItemFlags ModelChk::flags(const QModelIndex &index) const
{
    if (index.column()==1){
        return Qt::ItemIsEditable | Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    } else {
        return Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    }
}

QVariant ModelChk::headerData(int section, Qt::Orientation orientation, int role) const
{
    return /*modelSrc->headerData(section,orientation,role)*/QVariant();
}

QVector<int> ModelChk::getVals()
{
    QVector<int> r;
    QMap<int, bool>::const_iterator i = chkMap.constBegin();
    while (i != chkMap.constEnd()) {
        if (i.value()){
            r.push_back(modelSrc->data(modelSrc->index(i.key(),0),Qt::EditRole).toInt());
        }
        ++i;
    }
    return r;
}
