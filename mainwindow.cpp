#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tabWidget = new TabWidget(this);
    this->setCentralWidget(tabWidget);

    formJob = new FormJob();
    zpDialog = new ZpDialog();
    edtRabDialog = new EdtRabDialog();
    jobTypeDialog = new JobTypeDialog();
    edtTNDialog = new EdtTNDialog();
    premDialog = new PremDialog();
    formRepNorm = new FormRepNorm();

    tabWidget->addTabAction(formJob,ui->actionJob);
    tabWidget->addTabAction(zpDialog,ui->actionCalcZp);
    tabWidget->addTabAction(edtRabDialog,ui->actionRab);
    tabWidget->addTabAction(jobTypeDialog,ui->actionJobType);
    tabWidget->addTabAction(edtTNDialog,ui->actionTN);
    tabWidget->addTabAction(premDialog,ui->actionPrem);
    tabWidget->addTabAction(formRepNorm,ui->actionRepNorm);

    tabWidget->loadSettings();

    loadSettings();

    connect(ui->actionUpd,SIGNAL(triggered(bool)),Models::instance(),SLOT(refresh()));
    connect(tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChangeSlot()));
    connect(ui->actionFltRab,SIGNAL(triggered(bool)),Models::instance(),SLOT(setRabFilter(bool)));
}

MainWindow::~MainWindow()
{
    tabWidget->saveSettings();
    saveSettimgs();
    delete formJob;
    delete zpDialog;
    delete edtRabDialog;
    delete jobTypeDialog;
    delete edtTNDialog;
    delete premDialog;
    delete formRepNorm;

    delete Models::instance();
    delete ui;
}

void MainWindow::loadSettings()
{
    QSettings settings("szsm", "wire_zp");
    this->restoreGeometry(settings.value("main_geometry").toByteArray());
}

void MainWindow::saveSettimgs()
{
    QSettings settings("szsm", "wire_zp");
    settings.setValue("main_geometry", this->saveGeometry());
}

void MainWindow::tabChangeSlot()
{
    if (tabWidget->currentWidget()==formJob){
        //qDebug()<<"job!";
        formJob->updStat();
    }
}
