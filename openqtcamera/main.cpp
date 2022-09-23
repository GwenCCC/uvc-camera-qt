#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
