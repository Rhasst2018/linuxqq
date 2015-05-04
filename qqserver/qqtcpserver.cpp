#include "qqtcpserver.h"
#include "ui_qqtcpserver.h"

QQTcpServer::QQTcpServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QQTcpServer)
{
    ui->setupUi(this);

    M_TcpPort = 6666;
    M_TcpServer = new MyTcpServer();

    if(!SqlConnection())
        return;

    InitTcpListen();

    InitServer();

}

QQTcpServer::~QQTcpServer()
{
    delete ui;
}

void QQTcpServer::InitServer()
{
    //M_BlockSize = 0;
    M_TotalBytes = 0;
    M_BytesWritten = 0;
    //M_BytesToWrite = 0;

}

void QQTcpServer::InitTcpListen()
{
    while (!M_TcpServer->isListening() &&
           !M_TcpServer->listen(QHostAddress::Any,M_TcpPort))
    {
        qDebug() << M_TcpServer->errorString();
        this->close();
        return;
    }
}
