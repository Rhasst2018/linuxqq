#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#define DEBUG

#include <QWidget>
#include <QMovie>
#include "registerwidget.h"
#include "mainwidget.h"
#include <QMessageBox>
#include <QtNetwork>

#ifdef DEBUG
#include <QDebug>
#endif


namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = 0);
    ~LoginWidget();

private:
    Ui::LoginWidget *ui;
    QMovie *movie;
    QTcpSocket *M_TcpSocket;
    qint64 M_TcpPort;

private slots:
    void RegisterButton_Click_Slot();
    void LoginButton_Click_Slot();
    void DisplayError(QAbstractSocket::SocketError); //diaplay errrors
    void ReadMessage();

private:
    void SendLoginMessage();
    void ConnnectToServer();
};

#endif // LOGINWIDGET_H
