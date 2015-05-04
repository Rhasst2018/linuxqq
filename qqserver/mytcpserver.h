#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#define DEBUG
#include <QTcpServer>
#include "mythread.h"

#ifdef DEBUG
#include <QDebug>
#endif


class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = 0);


private:


protected:
    void incomingConnection(int socketDescriptor);

private slots:




};

#endif // MYTCPSERVER_H
