#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSharedPointer>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
QByteArray MainWindow::nesFileByteArray;
QString MainWindow::chsNameLibrary[16][256];
QString MainWindow::chtNameLibrary[2][256];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowFlag(Qt::WindowCloseButtonHint);
    ui->setupUi(this);
    file = new QMenu(tr("&文件"), this);
    auto open = new QAction(tr("&打开"), this);
    auto save = new QAction(tr("&保存"), this);
    file->addAction(open);
    file->addAction(save);
    connect(open, &QAction::triggered, this, &MainWindow::openFile);
    connect(save, &QAction::triggered, this, &MainWindow::saveFile);
    menuBar()->addMenu(file);

    mapper = new QMenu(tr("Mapper"), this);
    mapperMessagebox = new QAction(tr("&修改Mapper"), this);
    mapperMessagebox->setEnabled(false);
    mapper->addAction(mapperMessagebox);
    connect(mapperMessagebox, &QAction::triggered, this, &MainWindow::modifyMapper);
    menuBar()->addMenu(mapper);
    tabWidget = ui->tabWidget;
    militaryCommander = new MilitaryCommander(this);
    militaryCommander->setVisible(true);

    formation = new Formation(this);
    formation->setVisible(true);

    ui->tabWidget->addTab(militaryCommander, tr("武将"));
    ui->tabWidget->resize(militaryCommander->size());

    ui->tabWidget->addTab(formation, tr("阵型"));
    ui->tabWidget->resize(formation->size());

    connect(this, &MainWindow::refreshMilitaryCommander,
            militaryCommander, &MilitaryCommander::refreshMiliaryCommanderToListView);
    QMessageBox::warning(this, tr("免责声明"), tr("一定要备份ROM, ROM因为本修改器损毁概不负责"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::readMapper() {
    int low = (nesFileByteArray[6] & 0xF0) >> 4;
    int high = nesFileByteArray[7] & 0xF0;
    int mapper = low | high;
    qDebug() << "mapper: " << mapper;
    return mapper;
}

void MainWindow::modifyMapper() {
    bool ok;
    char mapperValue = static_cast<char>(QInputDialog::getInt(this,
                         tr("请输入Mapper"),
                         tr("mapper: (0-255)"),
                         readMapper(),
                         0,
                         255,
                         1,
                         &ok,
                         Qt::WindowCloseButtonHint));
    if (ok) {
        char high = mapperValue & static_cast<char>(0xF0);
        char low = static_cast<char>((mapperValue & 0x0F) << 4);
        nesFileByteArray[6] = (nesFileByteArray[6] & static_cast<char>(0x0F)) | low;
        nesFileByteArray[7] = (nesFileByteArray[7] & static_cast<char>(0x0F)) | high;
        QMessageBox::information(this, tr("mapper"), tr("mapper修改成功, 请保存ROM"));
    }
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
            mapperMessagebox->setEnabled(true);
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
