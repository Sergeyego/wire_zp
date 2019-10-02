#ifndef JOBTYPEDIALOG_H
#define JOBTYPEDIALOG_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "models.h"

namespace Ui {
class JobTypeDialog;
}

class JobTypeDialog : public QWidget
{
    Q_OBJECT
    
public:
    explicit JobTypeDialog(QWidget *parent = 0);
    ~JobTypeDialog();
    
private:
    Ui::JobTypeDialog *ui;
    DbTableModel *jobModel;

private slots:
    void updJob();
};

#endif // JOBTYPEDIALOG_H
