#include "mainwindow.h"
#include <QApplication>
#include <QFileInfo>


int main(int argc, char *argv[])
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    QApplication a(argc, argv);

    QFileInfo file("/dev/video0");
    if(file.exists() == false)
    {
        QMessageBox::critical(nullptr, QObject::tr("拍摄程序"), QObject::tr("当前未检测到摄像头/高拍仪"));
        return 0;
    }
    MainWindow w;
    w.show();

    return a.exec();
}
