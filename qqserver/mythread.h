#ifndef MYTHREAD_H
#define MYTHREAD_H

#define DEBUG

#include <QThread>
#include <QtNetwork>
#include <QDataStream>
#include "sqlconnection.h"
#include <QMap>

#ifdef DEBUG
#include <QDebug>
#endif

struct ConnectionList
{
    QString connectType;
    QString uid;
    QString fid;
    QTcpSocket *connectSocket;
};

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(int SocketDescriptor,QObject *parent );
    ~MyThread();

    void run();

private:
    int M_SocketDescriptor;
    QTcpSocket *M_TcpSocket;
    static QList<ConnectionList > connectionList;

private:
    void PrintconnectionList();
    QTcpSocket* getDestClient(QString connectType,QString reciver,QString sender);
    bool CheckPasswd(QString uid, QString pwd);
    bool ChangePasswd(QString uid, QString new_pwd, QString old_pwd);
    bool Register(QString nickname, QString pwd);
    bool AddFriend(QString uid, QString fid);
    QString GetNickName(QString uid);
    QString GetLocation(QString  uid);
    void updateHeadLocation(QString uid, QString headLocation);
    void SendLoginMessage(bool login, QString uid);
    void SendChangePasswdMessage(bool modified, QString uid);
    void SendRegisterMessage(bool registed, QString uid);
    void SendAddFriendMessage(QString uid, bool add);
    void SendNickNameMessage(QString uid, QString nickname);
    void SendLocationMessage(QString uid, QString location);
    void SendFriendsListMessage(QString uid);
    void SendChatMessage(QString fid,QString uid,QString msg);
    void removeConnect(QString connectType, QString uid, QString fid);
    void SendChatCloseMessage(QString uid);

signals:
    void error(QTcpSocket::SocketError socketError);


protected slots:
    void readMessage();


};

#endif // MYTHREAD_H
