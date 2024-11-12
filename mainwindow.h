#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "rels.h"
#include "formrab.h"
#include "formjob.h"
#include "formliter.h"
#include "formrepnorm.h"
#include "formpremmon.h"
#include "formcalcwage.h"
#include "formtn.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    bool exist(QObject *a);
    void actAction(QAction *a, void (MainWindow::*sl)());
    void addSubWindow(QWidget *w, QObject *a);
    bool setActiveSubWindow(QString t);
    QMap <QString,QAction*> actions;
    void loadSettings();
    void saveSettimgs();

private slots:
    void closeTab(int index);
    void newFormRab();
    void newFormJob();
    void newFormLiter();
    void newFormRepNorm();
    void newFormPremMon();
    void newFormCalcWage();
    void newFormTN();
};

#endif // MAINWINDOW_H
