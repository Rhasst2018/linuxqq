#include "mytcpserver.h"

MyTcpServer::MyTcpServer(QObject *parent)
        : QTcpServer(parent)
{

}
void MyTcpServer::incomingConnection(int socketDescriptor)
{
    qDebug()<<"incoming connection :"<<socketDescriptor;
    MyThread *Mythread = new MyThread(socketDescriptor,this);


    connect(Mythread,SIGNAL(finished()),Mythread,SLOT(deleteLater()),Qt::QueuedConnection);

    Mythread->start();
}




