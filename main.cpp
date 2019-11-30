#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont font("Microsoft YaHei", 12, 50, false);
    a.setFont(font);
    MainWindow w;
    w.show();

    return a.exec();
}
