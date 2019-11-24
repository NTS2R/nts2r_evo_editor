#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QFile>
#include <QDebug>
#include <QTabWidget>
#include "militarycommander.h"
#include "formation.h"
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
    int readMapper();

private:
    Ui::MainWindow *ui;
    QTabWidget* tabWidget;
    MilitaryCommander* militaryCommander;
    Formation* formation;
    QString nesFileName;
    QMenu* file;
    QMenu* mapper;
    QMenu* exportExcel;
    QAction* mapperMessagebox;
    QAction* exportExcelForMerge;
};

#endif // MAINWINDOW_H
