#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSharedPointer>
#include <QFileDialog>
#include <QHBoxLayout>

QByteArray MainWindow::nesFileByteArray;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    auto file = new QMenu(tr("&文件"), this);
    auto open = new QAction(tr("&打开"), this);
    auto save = new QAction(tr("&保存"), this);
    file->addAction(open);
    file->addAction(save);
    connect(open, &QAction::triggered, this, &MainWindow::openFile);
    connect(save, &QAction::triggered, this, &MainWindow::saveFile);
    menuBar()->addMenu(file);
    tabWidget = ui->tabWidget;
    militaryCommander = new MilitaryCommander(this);
    militaryCommander->setVisible(true);

    ui->tabWidget->addTab(militaryCommander, tr("武将"));

    connect(this, &MainWindow::refreshMilitaryCommander,
            militaryCommander, &MilitaryCommander::refreshMiliaryCommanderToListView);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile() {
    QFileDialog fileDialog;
    fileDialog.setWindowTitle(tr("打开 nes"));
    fileDialog.setDirectory(".");
    fileDialog.setNameFilter(tr("Nes(*.nes)"));
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setViewMode(QFileDialog::Detail);
    if (fileDialog.exec()) {
        auto selectFileList = fileDialog.selectedFiles();
        if (selectFileList.length() != 1) {
        } else {
            nesFileName = selectFileList.first();
            QScopedPointer<QFile> nesFile{new QFile{nesFileName}};
            nesFile->open(QIODevice::OpenModeFlag::ReadOnly);
            nesFileByteArray = nesFile->readAll();
            qDebug() << nesFileByteArray.length();
            nesFile->close();
            emit refreshMilitaryCommander();
        }
    }
}

void MainWindow::saveFile() {
    if (nesFileByteArray.length() != 0)
    {
        QScopedPointer<QFile> nesFile{new QFile{nesFileName}};
        nesFile->open(QIODevice::OpenModeFlag::Truncate | QIODevice::OpenModeFlag::ReadWrite);
        nesFile->write(nesFileByteArray);
        nesFile->close();
        qDebug() << "Save";
    }
}
