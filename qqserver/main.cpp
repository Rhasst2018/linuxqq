#include "qqtcpserver.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QQTcpServer w;
    //w.show();

    return a.exec();
}
