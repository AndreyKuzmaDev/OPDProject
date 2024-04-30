#include "mainwindow.h"
#include <QApplication>
#include <QApplication>
#include <QStyleFactory>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
       a.setWindowIcon(QIcon(":/new/prefix1/hheh.png"));

    w.show();



    return a.exec();
}
