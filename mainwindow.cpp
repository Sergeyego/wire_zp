#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    actAction(ui->actionRab,&MainWindow::newFormRab);
    actAction(ui->actionJob,&MainWindow::newFormJob);

    loadSettings();
    QObject::connect(ui->tabWidget,&QTabWidget::tabCloseRequested,this,&MainWindow::closeTab);
}

MainWindow::~MainWindow()
{
    saveSettimgs();
    delete Rels::instance();
    delete ui;
}

bool MainWindow::exist(QObject *a)
{
    bool b=false;
    QAction *action = qobject_cast<QAction *>(a);
    if (action){
        b=setActiveSubWindow(action->text());
    }
    return b;
}

void MainWindow::actAction(QAction *a, void (MainWindow::*sl)())
{
    connect(a, &QAction::triggered, this, sl);
    actions.insert(a->text(),a);
}

void MainWindow::addSubWindow(QWidget *w, QObject *a)
{
    w->setAttribute(Qt::WA_DeleteOnClose);
    QAction *action = qobject_cast<QAction *>(a);
    if (action){
        w->setWindowTitle(action->text());
    }
    ui->tabWidget->addTab(w,w->windowTitle());
    ui->tabWidget->setCurrentWidget(w);
}

bool MainWindow::setActiveSubWindow(QString t)
{
    bool b=false;
    for (int i=0; i<ui->tabWidget->count(); i++){
        if (ui->tabWidget->tabText(i)==t){
            ui->tabWidget->setCurrentIndex(i);
            b=true;
            break;
        }
    }
    return b;
}

void MainWindow::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    this->restoreGeometry(settings.value("main_geometry").toByteArray());
    this->restoreState(settings.value("main_state").toByteArray());
    QString opentab=settings.value("main_opentab").toString();
    QString current=settings.value("main_currenttab").toString();

    if (!opentab.isEmpty()){
        QStringList l=opentab.split("|");
        foreach (QString a, l) {
            if (actions.contains(a)){
                actions.value(a)->trigger();
            }
        }
    }
    setActiveSubWindow(current);
}

void MainWindow::saveSettimgs()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("main_state", this->saveState());
    settings.setValue("main_geometry", this->saveGeometry());
    QString opentab, currenttab;
    for (int i=0; i<ui->tabWidget->count(); i++){
        if (!opentab.isEmpty()){
            opentab+="|";
        }
        opentab+=ui->tabWidget->tabText(i);
    }
    currenttab=ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    settings.setValue("main_opentab", opentab);
    settings.setValue("main_currenttab",currenttab);
}

void MainWindow::closeTab(int index)
{
    ui->tabWidget->widget(index)->close();
}

void MainWindow::newFormRab()
{
    if (!exist(sender())){
        addSubWindow(new FormRab(),sender());
    }
}

void MainWindow::newFormJob()
{
    if (!exist(sender())){
        addSubWindow(new FormJob(),sender());
    }
}
