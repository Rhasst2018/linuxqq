#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H


#define DEBUG

#include <QWidget>
#include <QPixmap>
#include <QMessageBox>
#include <QtNetwork>
#include "loginwidget.h"

#ifdef DEBUG
#include <QDebug>
#endif

namespace Ui {
class RegisterWidget;
}

class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWidget(QWidget *parent = 0);
    ~RegisterWidget();

private slots:
    void on_RegisterButton_clicked();
    void ReadMessage();
    void DisplayError(QAbstractSocket::SocketError); //diaplay errrors

private:
    Ui::RegisterWidget *ui;
    QTcpSocket *M_TcpSocket;
    qint64 M_TcpPort;
    //QPixmap pixmap;
private:
    void SendRegisterMessage();
    void ConnnectToServer();
};

#endif // REGISTERWIDGET_H
