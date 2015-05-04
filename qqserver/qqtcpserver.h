#ifndef QQTCPSERVER_H
#define QQTCPSERVER_H

#define DEBUG
#include <QWidget>
#include <QtNetwork>
#include <QMap>
#include "mytcpserver.h"
#include "sqlconnection.h"

#ifdef DEBUG
#include <QDebug>
#endif

namespace Ui {
class QQTcpServer;
}


class QQTcpServer : public QWidget
{
    Q_OBJECT

public:
    explicit QQTcpServer(QWidget *parent = 0);
    ~QQTcpServer();

private:
    Ui::QQTcpServer *ui;
    qint64 M_TcpPort;
    qint64 M_BlockSize;
    qint64 M_TotalBytes;
    qint64 M_BytesWritten;
    qint64 M_ytesToWrite;
    MyTcpServer * M_TcpServer;
    QTcpSocket *M_ClientConnection;

    QMap<QString, QTcpSocket*> M_connectionList;

private:
    void InitServer();
    void InitTcpListen();

private slots:


};

#endif // QQTCPSERVER_H
