#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QFile>
#include <QDebug>
#include <QTabWidget>
#include "militarycommander.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void saveFile();

private:
    Ui::MainWindow *ui;
    QTabWidget* tabWidget;
    MilitaryCommander* militaryCommander;
    QString nesFileName;
    QByteArray nesFileByteArray;
};

#endif // MAINWINDOW_H
