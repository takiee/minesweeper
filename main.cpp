#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    QApplication::addLibraryPath("./plugins");//解决exewen问题
    return a.exec();
}
