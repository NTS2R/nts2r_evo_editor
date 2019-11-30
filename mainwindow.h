#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QFile>
#include <QDebug>
#include <QTabWidget>
#include "militarycommander.h"
#include "formation.h"
#include "about.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    static QByteArray nesFileByteArray;
    static QString chsNameLibrary[16][256];
    static QString chtNameLibrary[2][256];
    static QMap<QString, QString> weaponName;
    static QMap<QString, QString> dixingName;
    QSize sizeHint() const override;
signals:
    void refreshMilitaryCommander();

private slots:
    void openFile();
    void saveFile();
    void modifyMapper();
    void exportExcelForMergeFunc();
    void importMilitary();
    void importSpecial();
    void aboutShow();
    int readMapper();
    QString getBuildVersion();

private:
    Ui::MainWindow *ui;
    QTabWidget* tabWidget;
    MilitaryCommander* militaryCommander;
    Formation* formation;
    QString nesFileName;
    QMenu* file;
    QMenu* mapper;
    QMenu* exportImportExcel;
    QAction* mapperMessagebox;
    QAction* exportExcelForMerge;
    QAction* importMilitaryExcel;
    QAction* importSpecialExcel;
};

#endif // MAINWINDOW_H
