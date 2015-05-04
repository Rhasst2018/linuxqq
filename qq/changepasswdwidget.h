#ifndef CHANGEPASSWDWIDGET_H
#define CHANGEPASSWDWIDGET_H

#define DEBUG

#include <QWidget>
#include <QMessageBox>
#include <QtNetwork>
#include "loginwidget.h"

#ifdef DEBUG
#include <QDebug>
#endif

namespace Ui {
class ChangePasswdWidget;
}

class ChangePasswdWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChangePasswdWidget(QString uid,QWidget *parent = 0);
    ~ChangePasswdWidget();

private slots:
    void on_SubmitButton_clicked();
    void ReadMessage();

private:
    Ui::ChangePasswdWidget *ui;
    QTcpSocket *M_TcpSocket;
    qint64 M_TcpPort;
    QString M_uid;

private:
    void SendChangePasswdMessage();
    void ConnnectToServer();
};

#endif // CHANGEPASSWDWIDGET_H
