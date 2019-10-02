#ifndef PREMDIALOG_H
#define PREMDIALOG_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "models.h"

namespace Ui {
class PremDialog;
}

class PremDialog : public QWidget
{
    Q_OBJECT
    
public:
    explicit PremDialog(QWidget *parent = 0);
    ~PremDialog();
    
private:
    Ui::PremDialog *ui;
    DbTableModel *modelPremK;
    DbTableModel *modelList;
    DbTableModel *modelCont;

private slots:
    void setCurrentLiter(QModelIndex index);
    void setCurrentList(QModelIndex index);
};

#endif // PREMDIALOG_H
