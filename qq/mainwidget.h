#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#define DEBUG

#include <QWidget>
#include <QToolBox>
#include <QGroupBox>
#include <QPixmap>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QtNetwork>
#include <QDataStream>
#include <QFileDialog>
#include "chatwindow.h"
#include "mytoolbutton.h"
#include "addfriendwidget.h"
#include "changepasswdwidget.h"

#ifdef DEBUG
#include <QDebug>
#endif

namespace Ui {
class MainWidget;
}


class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QString uid, QTcpSocket *TcpSocket, QWidget *parent = 0);
    ~MainWidget();

private:
    Ui::MainWidget *ui;
    QString M_uid;
    QTcpSocket *M_TcpSocket;
    QString M_nickname;
    QLinkedList<QString> chatList;
    QList<QString> onlineList;

private:
    void SendMessage(QString messageType);
    void RefreshFriendsList(QDataStream *);

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void ToolBtnClicked(QString fid, QString friend_nickname);
    void SearchBtnClicked();
    void ChangePasswdButton_Click_Slot();
    void DisplayError(QAbstractSocket::SocketError); //diaplay errrors
    void ReadMessage();
    void changeHeadPortrait();
    void chatClosed(QString);
    void addedFriendsSlot();
};

#endif // MAINWIDGET_H
