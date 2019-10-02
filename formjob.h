#ifndef FORMJOB_H
#define FORMJOB_H

#include <QWidget>
#include "modeljob.h"
#include "truncatepremdialog.h"

namespace Ui {
class FormJob;
}

class FormJob : public QWidget
{
    Q_OBJECT

public:
    explicit FormJob(QWidget *parent = 0);
    ~FormJob();

private:
    Ui::FormJob *ui;
    ModelJob *modelJob;
private slots:
    void truncatePrem();

public slots:
    void updStat();
};

#endif // FORMJOB_H
