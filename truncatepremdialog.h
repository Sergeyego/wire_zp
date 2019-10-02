#ifndef TRUNCATEPREMDIALOG_H
#define TRUNCATEPREMDIALOG_H

#include <QDialog>
#include "models.h"

namespace Ui {
class TruncatePremDialog;
}

class TruncatePremDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TruncatePremDialog(QWidget *parent = 0);
    ~TruncatePremDialog();
    
private:
    Ui::TruncatePremDialog *ui;

private slots:
    void truncate();
signals:
    void sigUpd();
};

#endif // TRUNCATEPREMDIALOG_H
