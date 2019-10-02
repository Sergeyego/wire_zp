#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "models.h"
#include "modeljob.h"
#include "zpdialog.h"
#include "premdialog.h"
#include "jobtypedialog.h"
#include "edttndialog.h"
#include "edtrabdialog.h"
#include "tabwidget.h"
#include "formjob.h"
#include "formrepnorm.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    TabWidget *tabWidget;

    FormJob *formJob;
    ZpDialog *zpDialog;
    EdtRabDialog *edtRabDialog;
    JobTypeDialog *jobTypeDialog;
    EdtTNDialog *edtTNDialog;
    PremDialog *premDialog;
    FormRepNorm *formRepNorm;

private slots:
    void loadSettings();
    void saveSettimgs();
    void tabChangeSlot();
};

#endif // MAINWINDOW_H
