#ifndef TRUNCATEPREMDIALOG_H
#define TRUNCATEPREMDIALOG_H

#include <QDialog>
#include "rels.h"
#include <QtSql>
#include <QMessageBox>
#include <QCalendarWidget>

namespace Ui {
class TruncatePremDialog;
}

class TruncatePremDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TruncatePremDialog(QWidget *parent = nullptr);
    ~TruncatePremDialog();
    
private:
    Ui::TruncatePremDialog *ui;

private slots:
    void truncate();
signals:
    void sigUpd();
};

#endif // TRUNCATEPREMDIALOG_H
