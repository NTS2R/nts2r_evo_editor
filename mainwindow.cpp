#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSharedPointer>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QMap>
#include <QJsonParseError>
#include <QJsonObject>

QByteArray MainWindow::nesFileByteArray;
QString MainWindow::chsNameLibrary[16][256];
QString MainWindow::chtNameLibrary[2][256];
QMap<QString, QString> MainWindow::weaponName;
QMap<QString, QString> MainWindow::dixingName;

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

    exportImportExcel = new QMenu(tr("导出/导入"), this);
    exportExcelForMerge = new QAction(tr("&导出修改/信息表"), this);
    exportExcelForMerge->setEnabled(false);
    exportImportExcel->addAction(exportExcelForMerge);
    connect(exportExcelForMerge, &QAction::triggered, this, &MainWindow::exportExcelForMergeFunc);
    importMilitaryExcel = new QAction(tr("&导入武将信息"), this);
    importMilitaryExcel->setEnabled(false);
    exportImportExcel->addAction(importMilitaryExcel);
    connect(importMilitaryExcel, &QAction::triggered, this, &MainWindow::importMilitary);
    importSpecialExcel = new QAction(tr("&导入特殊表"), this);
    importSpecialExcel->setEnabled(false);
    exportImportExcel->addAction(importSpecialExcel);
    connect(importSpecialExcel, &QAction::triggered, this, &MainWindow::importSpecial);
    menuBar()->addMenu(exportImportExcel);

    tabWidget = ui->tabWidget;
    militaryCommander = new MilitaryCommander(this);
    militaryCommander->setVisible(true);

    formation = new Formation(this);
    formation->setVisible(true);

    ui->tabWidget->setStyleSheet("QTabBar::tab { height: 20px; width: 50px; }");

    ui->tabWidget->addTab(militaryCommander, tr("武将"));
//    ui->tabWidget->resize(militaryCommander->size());

    ui->tabWidget->addTab(formation, tr("阵型"));
//    ui->tabWidget->resize(formation->size());
//    ui->tabWidget->resize()


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

        auto weaponFileNameJson = QString("%1/%2/degrade.dat")
                .arg(QCoreApplication::applicationDirPath())
                .arg(configPath);
        QFile loadFile{weaponFileNameJson};

        if(!loadFile.open(QIODevice::ReadOnly)) {
            qDebug() << "could't open projects json";
            return;
        }

        QByteArray allData = loadFile.readAll();
        QJsonParseError json_error;
        QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));

        if(json_error.error != QJsonParseError::NoError) {
            qDebug() << "json error!";
            return;
        }
        QJsonObject rootObj = jsonDoc.object();
        QStringList keys = rootObj.keys();
        for(int i = 0; i < keys.size(); i++) {
            auto key = keys.at(i);
            weaponName.insert(key, rootObj.value(key).toString());
            qDebug() << key << " -> " << rootObj.value(key).toString();
        }
        militaryCommander->weaponComboBox->clear();
        for (auto iter = weaponName.begin();
             iter != weaponName.end();
             ++iter) {
            auto key = iter.key();
            auto keyNumber = key.toInt(nullptr, 16);
            auto value = key + "/" + weaponName[key];
            if (keyNumber < 0x80) {
                militaryCommander->weaponComboBox->insertItem(keyNumber, value);
            }
        }

        auto dixingFileNameJson = QString("%1/%2/hero_terrian_name.dat")
                .arg(QCoreApplication::applicationDirPath())
                .arg(configPath);
        QFile dixingFile{dixingFileNameJson};
        if(!dixingFile.open(QIODevice::ReadOnly)) {
            qDebug() << "could't open projects json";
            return;
        }

        QByteArray dixingAllData = dixingFile.readAll();
        QJsonParseError dixingJsonError;
        QJsonDocument dixingJsonDoc(QJsonDocument::fromJson(dixingAllData, &dixingJsonError));

        if(dixingJsonError.error != QJsonParseError::NoError) {
            qDebug() << "dixing json error!";
            return;
        }

        QJsonObject dixingRootObj = dixingJsonDoc.object();
        QStringList dixingKeys = dixingRootObj.keys();
        for(int i = 0; i < dixingKeys.size(); i++) {
            auto key = dixingKeys.at(i);
            dixingName.insert(key, dixingRootObj.value(key).toString());
            qDebug() << key << " -> " << dixingRootObj.value(key).toString();
        }

        for (auto iter = dixingName.begin();
             iter != dixingName.end();
             ++iter) {
            auto key = iter.key();
            auto keyNumber = key.toInt(nullptr, 16);
            auto value = key + "/" + dixingName[key];
            militaryCommander->dixingComboBox->insertItem(keyNumber, value);
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
            exportExcelForMerge->setEnabled(true);
            importMilitaryExcel->setEnabled(true);
            importSpecialExcel->setEnabled(true);
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

void MainWindow::exportExcelForMergeFunc() {
    qDebug() << "Export";
    auto excelFileName = nesFileName.replace(".nes", ".xlsx");
    militaryCommander->exportMilitary(excelFileName);
}

QSize MainWindow::sizeHint() const {
    return QSize{800, 600};
}

void MainWindow::importMilitary() {
    QFileDialog fileDialog;
    fileDialog.setWindowTitle(tr("武将 xlsx"));
    fileDialog.setDirectory(".");
    fileDialog.setNameFilter(tr("xlsx(*.xlsx)"));
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setViewMode(QFileDialog::Detail);
    if (fileDialog.exec()) {
        auto selectFileList = fileDialog.selectedFiles();
        if (selectFileList.length() != 1) {
        } else {
            auto excelFileName = selectFileList.first();
            militaryCommander->importMilitary(excelFileName);
        }
    }
}

void MainWindow::importSpecial() {
    QFileDialog fileDialog;
    fileDialog.setWindowTitle(tr("特殊 xlsx"));
    fileDialog.setDirectory(".");
    fileDialog.setNameFilter(tr("xlsx(*.xlsx)"));
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setViewMode(QFileDialog::Detail);
    if (fileDialog.exec()) {
        auto selectFileList = fileDialog.selectedFiles();
        if (selectFileList.length() != 1) {
        } else {
            auto excelFileName = selectFileList.first();
            militaryCommander->importSpecial(excelFileName);
        }
    }
}
