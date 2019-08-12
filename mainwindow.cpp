#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSharedPointer>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
QByteArray MainWindow::nesFileByteArray;
QString MainWindow::chsNameLibrary[16][256];
QString MainWindow::chtNameLibrary[2][256];

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
    ui->tabWidget->resize(militaryCommander->size());
    connect(this, &MainWindow::refreshMilitaryCommander,
            militaryCommander, &MilitaryCommander::refreshMiliaryCommanderToListView);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile() {
    QString configPath = "config";

    QDir config{QCoreApplication::applicationDirPath() + "/" + configPath};
    if (!config.exists()) {
        qDebug() << config.path();
        QMessageBox::warning(nullptr, "警告", "没有配置文件目录，请检查");
        QApplication::exit(-1);
    } else {
        for (int i = 0x0; i <= 0xF; ++i) {
            auto chsNameFileName = QString("%1/%2/b%3.dat")
                    .arg(QCoreApplication::applicationDirPath())
                    .arg(configPath)
                    .arg(i,1,16);
            QFile file{chsNameFileName};
            file.open(QIODevice::OpenModeFlag::ReadOnly);
            QTextStream stream{&file};
            stream.setCodec("utf-8");
            auto textSet = stream.readAll().replace('\n', ' ').split(" ", QString::SkipEmptyParts);
            for(auto const& text: textSet) {
                auto index = QString("%1%2").arg(text[0]).arg(text[1]).toInt(nullptr, 16);
                qDebug() << index << " " << QString(text[2]);
                chsNameLibrary[i][index] = QString(text[2]);
            }
        }

        for (int i = 1; i <= 2; ++i) {
            auto chtNameFileName = QString("%1/%2/name%3.dat")
                    .arg(QCoreApplication::applicationDirPath())
                    .arg(configPath)
                    .arg(i,1,16);
            QFile file{chtNameFileName};
            file.open(QIODevice::OpenModeFlag::ReadOnly);
            QTextStream stream{&file};
            stream.setCodec("utf-8");
            auto textSet = stream.readAll().replace('\n', ' ').remove(' ').split("", QString::SkipEmptyParts);
            int index = 0;
            for(auto const& text: textSet) {
                qDebug() << text;
                chtNameLibrary[i-1][index++] = text;
            }
        }
    }

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
