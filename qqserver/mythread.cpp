#include "mythread.h"

QList<ConnectionList > MyThread::connectionList;

MyThread::MyThread(int SocketDescriptor, QObject *parent) :
    QThread(parent),M_SocketDescriptor(SocketDescriptor)
{
    M_SocketDescriptor = SocketDescriptor;
}

void MyThread::run()
{
    M_TcpSocket = new QTcpSocket();
    if(!M_TcpSocket->setSocketDescriptor(M_SocketDescriptor))
    {
        emit error(M_TcpSocket->error());
        return;
    }
#ifdef DEBUG
    qDebug()<<M_SocketDescriptor;
    qDebug()<<"bytes available:"<<M_TcpSocket->bytesAvailable();
#endif

    //关联接受客户端发来数据的槽
    connect(M_TcpSocket, SIGNAL(disconnected()), M_TcpSocket, SLOT(deleteLater()));
    connect(M_TcpSocket,SIGNAL(disconnected()),this,SLOT(quit()));
    connect(M_TcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage()),Qt::BlockingQueuedConnection);
//    connect(M_TcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage()));
    exec();
    //socket断开连接，线程退出

}

MyThread::~MyThread()
{
    emit finished();

    qDebug()<<"A Thread Quit!";
}

void MyThread::readMessage()
{

    qint64 TotalBytes = 0,bytesReceived = 0;
    QDataStream in(M_TcpSocket);

#ifdef DEBUG
    qDebug()<<"M+++++++++++++++++++++";
    qDebug()<<"......"<<M_TcpSocket;
    qDebug()<<M_TcpSocket->localAddress()<<" "<<M_TcpSocket->localPort();
    qDebug()<<"N++++++++++++++++++++++";
#endif
    if( M_TcpSocket->socketDescriptor() < 0)
        return;
#ifdef DEBUG
    qDebug()<<"MS+++++++++++++++++++++";
#endif

#ifdef DEBUG
    qDebug()<<"socketdescriptor:"<<M_TcpSocket->socketDescriptor();
#endif
    in.setVersion(QDataStream::Qt_4_8); //设置数据流版本，与客户端数据流版本保持一致

#ifdef DEBUG
    qDebug()<<" acquire client connetion ok !";
#endif
    while(!in.atEnd()) {
        TotalBytes = 0;
        bytesReceived = 0;
        if(TotalBytes == 0)
        {
            if(M_TcpSocket->bytesAvailable() < (int)sizeof(qint64))
            {
#ifdef DEBUG
                qDebug()<<" no bytes available !";
#endif
                return ;
            }
            in >> TotalBytes;
#ifdef DEBUG
            qDebug()<<" blocksize:"<<TotalBytes;
#endif
        }

        if(M_TcpSocket->bytesAvailable() < TotalBytes)
        {
#ifdef DEBUG
            qDebug()<<" the data is not full !";
#endif
            return ;
        }

        QString messageType;

        in >> messageType;
        bytesReceived += (qint64)sizeof(messageType);

#ifdef DEBUG
        qDebug() <<"msg type:"<< messageType;
#endif
        if( messageType == "CONNECT" )
        {
            QString connectType;
            ConnectionList connList;

            in >> connectType ;
            bytesReceived += (qint64)connectType.size();

            if(connectType == "mainWidget"
                    || connectType == "addFriend" || connectType == "changePasswd")
            {
                QString uid;
                in >> uid;
                connList.uid = uid;
            }
            else if(connectType == "chat")
            {
                QString uid,fid;
                in >> uid >> fid;
                connList.uid = uid;
                connList.fid = fid;
            }
            else if(connectType == "register")   /* 不同客户端同时注册会出问题，还需修改*/
            {

            }
            else
            {
#ifdef DEBUG
                qDebug() <<"connectType error";
#endif
                return;
            }
            connList.connectType = connectType;
            connList.connectSocket = M_TcpSocket;

            connectionList.append(connList);

            if(connList.connectType == "mainWidget")
            {
                foreach (ConnectionList conn, connectionList)
                {
                    if(conn.uid != connList.uid)   /*向其它登录用户发送在线列表，表示此用户上线*/
                        SendFriendsListMessage(conn.uid);
                }
            }

            PrintconnectionList();
        }
        else if(messageType == "LOGIN")
        {
            QString uid,pwd;
            in >> uid >> pwd; // receive user id and passwd
            bool login = CheckPasswd(uid, pwd);
#ifdef DEBUG
            qDebug()<<"login flag:"<<login;
#endif
            SendLoginMessage(login,uid);
        }
        else if( messageType == "CHAT" )
        {
            QString fid,uid,msg;
#ifdef DEBUG
            qDebug()<<" tcpserver start to read message!";
#endif
            in >> fid >> uid >> msg;
            bytesReceived += (qint64)fid.size();
            bytesReceived += (qint64)uid.size();
            bytesReceived += (qint64)msg.size();
#ifdef DEBUG
            qDebug() <<"tcp dest client id:"<< fid;
            qDebug() <<"tcp src client id:"<< uid;
            qDebug() <<"tcp client msg:"<< msg;
            qDebug()<<" tcpserver end read mesg!";
#endif

            SendChatMessage(fid, uid, msg);

        }
        else if(messageType == "SENDFILE")
        {
            QString fid, uid, currentFile;
            qint64 fileSize;
            in >> fid >> uid >> currentFile >> fileSize;
#ifdef DEBUG
            qDebug()<<"fid:"<<fid<<"uid:"<<uid;
            qDebug()<<"currentFile:"<<currentFile<<"filesize:"<<fileSize;
#endif
            bytesReceived += (qint64)fid.size();
            bytesReceived += (qint64)uid.size();
            bytesReceived += (qint64)currentFile.size();
            bytesReceived += (qint64)sizeof(qint64);
            bytesReceived += fileSize;
#ifdef DEBUG
            qDebug()<<"type size:"<<(qint64)messageType.size();
            qDebug()<<"fid size:"<<(qint64)fid.size();
            qDebug()<<"uid size:"<<(qint64)uid.size();
            qDebug()<<"currentfile size:"<<(qint64)currentFile.size();
            qDebug()<<"filesize size:"<<sizeof(qint64);
#endif
            QByteArray block; //用于暂存我们要发送的数据
            QDataStream out(&block,QIODevice::WriteOnly);
            //使用数据流写入数据
            out.setVersion(QDataStream::Qt_4_8);
            //设置数据流的版本,客户端和服务器端使用的版本要相同
#ifdef DEBUG
            qDebug()<<"filesize:"<<fileSize;
#endif
            out <<qint64(0);
            QString send_messageType;
            send_messageType = "SENDFILE";
            out << send_messageType;
            out << fid << uid << currentFile << fileSize;
//            out << M_TcpSocket->read(fileSize);
            out << M_TcpSocket->readAll();

            if(bytesReceived < TotalBytes )
            {
                qDebug()<<"byteReceived:"<<bytesReceived;
                qDebug()<<"TotalBytes:"<<TotalBytes;
                QByteArray Block;
                Block = M_TcpSocket->readAll();
                qDebug()<<" Block size:"<<(qint64)Block.size();
                Block.resize(0);
            }
            out.device()->seek(0);
            out << (qint64)(block.size() - sizeof(qint64));
#ifdef DEBUG
            qDebug()<<"server send file size:"<<(qint64)(block.size() - sizeof(qint64));
#endif
            QTcpSocket *ClientSocket = getDestClient("chat",fid,uid);
            connect(ClientSocket,SIGNAL(disconnected()),ClientSocket,SLOT(deleteLater()));
            ClientSocket->write(block);
        }
        else if(messageType == "CHANGEPASSWD")
        {
            QString uid,new_pwd,old_pwd;
            in >> uid >> old_pwd >> new_pwd; // receive user id and passwds
            bool modified = ChangePasswd(uid, new_pwd, old_pwd);
            SendChangePasswdMessage(modified, uid);
        }
        else if(messageType == "REGISTER")
        {
            QString nickname, pwd;
            in >> nickname >> pwd;
            Register(nickname, pwd);
        }
        else if(messageType == "GETNICKNAME")
        {
            QString uid;
            in >> uid;
            SendNickNameMessage(uid, GetNickName(uid));
        }
        else if(messageType == "GETLOCATION")
        {
            QString uid;
            in >> uid;
            SendLocationMessage(uid, GetLocation(uid));
        }
        else if(messageType == "FRIENDSLIST")
        {
            QString uid;
            in >> uid;
            SendFriendsListMessage(uid);
        }
        else if(messageType == "ADDFRIEND")
        {
            QString uid,fid;
            in >> uid >> fid; // receive user id and friend id
            bool add = AddFriend(uid, fid);
            SendAddFriendMessage(uid, add);
        }
        else if(messageType == "CHANGEHEADLOCATION")
        {
            QString uid,headLocation;
            in >> uid >> headLocation;

            updateHeadLocation(uid,headLocation);
        }
        else if(messageType == "OFFLINE")   /*下线通知*/
        {
            QString uid;
            in >> uid;
            removeConnect("mainWidget", uid, NULL);
#ifdef DEBUG
            qDebug() << "receive offline message";
            PrintconnectionList();
#endif
            foreach (const ConnectionList conn, connectionList)
            {
                if(conn.connectType == "mainWidget")
                    SendFriendsListMessage(conn.uid);      /*向其它用户发送在线列表，通知用户下线*/
            }
        }
        else if(messageType == "CLOSECHAT")   /*关闭聊天窗口通通知*/
        {
            QString uid, fid;
            in >> uid >> fid;
            removeConnect("chat", uid, fid);
        }
        else
        {
#ifdef DEBUG
            qDebug()<<" wrong message messageType !";
#endif

        }
    }

}

void MyThread::PrintconnectionList()
{
#ifdef DEBUG
    qDebug()<<"Print Map....";

    foreach (const ConnectionList &conn, connectionList)
    {
        qDebug() << " TcpClient connectType: " << conn.connectType;
        qDebug() << " TcpClient uid: "<<conn.uid;
    }
#endif
}


QTcpSocket* MyThread::getDestClient(QString connectType, QString reciver, QString sender)
{
    if(reciver != NULL && sender != NULL)
    {
        foreach (const ConnectionList conn, connectionList)
            if(conn.connectType == connectType && conn.uid == reciver && conn.fid == sender)
                return conn.connectSocket;
    }
    else if(sender == NULL)
    {
        foreach (const ConnectionList conn, connectionList)
            if(conn.connectType == connectType && conn.uid == reciver)
                return conn.connectSocket;
    }
    else if(reciver == NULL)    /*此时是注册的情况，发送者也是空*/
    {
        foreach (const ConnectionList conn, connectionList)
            if(conn.connectType == connectType)
                return conn.connectSocket;
    }
    else
    {
#ifdef DEBUG
        qDebug()<<"not exist the id!";
#endif
    }
    return NULL;
}

bool MyThread::CheckPasswd(QString uid, QString pwd)
{
    QSqlQuery query;
    query.prepare("select * from usr_info where uid = :uid and pwd = :pwd ");
    query.bindValue(":uid",uid);
    query.bindValue(":pwd",pwd);
    query.exec();
    if(query.next())
        return true;
    return false;
}

bool MyThread::ChangePasswd(QString uid, QString new_pwd, QString old_pwd)
{
    QSqlQuery query;
    query.prepare("select * from usr_info where uid =:uid and pwd =:pwd");
    query.bindValue(":uid",uid);
    query.bindValue(":pwd",old_pwd);
    query.exec();
    if(query.next())
    {
        query.clear();
        query.prepare("update usr_info set pwd = :pwd where uid =:uid");
        query.bindValue(":uid",uid);
        query.bindValue(":pwd",new_pwd);
        if(query.exec())
        {
            if(query.isActive())
                return true;
            else
                return false;
        }
    }
    return false;
}

bool MyThread::Register(QString nickname, QString pwd)
{
    QSqlQuery query;
    QString uid;

    query.exec("select * from usr_info");
    uid = QString::number(query.size() + 1);

    query.prepare("insert into usr_info values(:uid,:pwd,:nickname)");
    query.bindValue(":uid",uid);
    query.bindValue(":pwd",pwd);
    query.bindValue(":nickname",nickname);
    if(query.exec())
    {
        QSqlQuery query2;
        query2.prepare("insert into usr_head_portrait values(:uid,:head_portrait_location)");
        query2.bindValue(":uid",uid);
        query2.bindValue(":head_portrait_location","./images/head1.jpg");
        query2.exec();

        SendRegisterMessage(true,uid);

        return true;
    }
    SendRegisterMessage(false,uid);
    return false;

}

bool MyThread::AddFriend(QString uid, QString fid)
{
    QSqlQuery query;
    query.prepare("select * from usr_info where uid = :uid");
    query.bindValue(":uid",fid);
    query.exec();
    if(query.next())
    {
        QSqlQuery query2;
        query2.prepare("insert into usr_friendslist values(:uid,:fid)");
        query2.bindValue(":fid",fid);
        query2.bindValue(":uid",uid);
        if(query2.exec())
        {
            return true;
        }
    }
    return false;
}

QString MyThread::GetNickName(QString uid)
{
    QSqlQuery query;
    query.prepare("select * from usr_info where uid = :uid");
    query.bindValue(":uid",uid);
    query.exec();
    if(query.next())
        return query.value(2).toString();
    return 0;
}

QString MyThread::GetLocation(QString uid)
{
    QSqlQuery query;
    query.prepare("select * from usr_head_portrait where uid = :uid");
    query.bindValue(":uid",uid);
    query.exec();
    if(query.next())
        return query.value(1).toString();
    return 0;
}

void MyThread::SendLoginMessage(bool login, QString uid)
{
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream out(&block,QIODevice::WriteOnly);
    //使用数据流写入数据
    out.setVersion(QDataStream::Qt_4_8);
    //设置数据流的版本,客户端和服务器端使用的版本要相同
    out << (qint64) 0;
    QString messageType = "LOGIN";
    out << messageType;
    out << login;
    out.device()->seek(0);
    out << (qint64)(block.size() - sizeof(qint64));
#ifdef DEBUG
    qDebug()<<" bytes send:"<<(qint64)(block.size() - sizeof(qint64));
#endif
    QTcpSocket *ClientSocket = getDestClient("mainWidget", uid, NULL);
    connect(ClientSocket,SIGNAL(disconnected()),ClientSocket,SLOT(deleteLater()));
    ClientSocket->write(block);

    if(!login)   /*没有登录成功则删除此连接*/
    {
        ClientSocket->waitForBytesWritten(3000);
        removeConnect("mainWidget", uid, NULL);
    }
}

void MyThread::SendChangePasswdMessage(bool modified, QString uid)
{
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream out(&block,QIODevice::WriteOnly);
    //使用数据流写入数据
    out.setVersion(QDataStream::Qt_4_8);
    //设置数据流的版本,客户端和服务器端使用的版本要相同
    out << (qint64) 0;
    QString messageType = "CHANGEPASSWD";
    out << messageType;
    out << modified;
    out.device()->seek(0);
    out << (qint64)(block.size() - sizeof(qint64));
    QTcpSocket *ClientSocket = getDestClient("changePasswd", uid, NULL);
    connect(ClientSocket,SIGNAL(disconnected()),ClientSocket,SLOT(deleteLater()));
    ClientSocket->write(block);

    ClientSocket->waitForBytesWritten(3000);
    removeConnect("changePasswd", uid, NULL);
}

void MyThread::SendRegisterMessage(bool registed,QString uid)
{
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream out(&block,QIODevice::WriteOnly);
    //使用数据流写入数据
    out.setVersion(QDataStream::Qt_4_8);
    //设置数据流的版本,客户端和服务器端使用的版本要相同
    out << (qint64) 0;
    QString messageType = "REGISTER";
    out << messageType;
    out << registed;
    if(registed)
        out << uid;
    out.device()->seek(0);
    out << (qint64)(block.size() - sizeof(qint64));
    QTcpSocket *ClientSocket = getDestClient("register", NULL, NULL);
    connect(ClientSocket,SIGNAL(disconnected()),ClientSocket,SLOT(deleteLater()));
    ClientSocket->write(block);

    ClientSocket->waitForBytesWritten(3000);
    removeConnect("register", NULL, NULL);
}

void MyThread::SendNickNameMessage(QString uid, QString nickname)
{
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream out(&block,QIODevice::WriteOnly);
    //使用数据流写入数据
    out.setVersion(QDataStream::Qt_4_8);
    //设置数据流的版本,客户端和服务器端使用的版本要相同
    out << (qint64) 0;
    QString messageType = "GETNICKNAME";
    out << messageType;
    out << nickname;
    out.device()->seek(0);
    out << (qint64)(block.size() - sizeof(qint64));
#ifdef DEBUG
    qDebug()<<"send nickname size:"<<(qint64)(block.size() - sizeof(qint64));
#endif
    QTcpSocket *ClientSocket = getDestClient("mainWidget", uid, NULL);
    connect(ClientSocket,SIGNAL(disconnected()),ClientSocket,SLOT(deleteLater()));
    ClientSocket->write(block);
}

void MyThread::SendLocationMessage(QString uid, QString location)
{
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream out(&block,QIODevice::WriteOnly);
    //使用数据流写入数据
    out.setVersion(QDataStream::Qt_4_8);
    //设置数据流的版本,客户端和服务器端使用的版本要相同
    out << (qint64) 0;
    QString messageType = "GETLOCATION";
    out << messageType;
    out << location;
    out.device()->seek(0);
    out << (qint64)(block.size() - sizeof(qint64));
#ifdef DEBUG
    qDebug()<<"send location size:"<<(qint64)(block.size() - sizeof(qint64));
#endif
    QTcpSocket *ClientSocket = getDestClient("mainWidget", uid, NULL);
    connect(ClientSocket,SIGNAL(disconnected()),ClientSocket,SLOT(deleteLater()));
    ClientSocket->write(block);
}

void MyThread::SendAddFriendMessage(QString uid, bool add)
{
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream out(&block,QIODevice::WriteOnly);
    //使用数据流写入数据
    out.setVersion(QDataStream::Qt_4_8);
    //设置数据流的版本,客户端和服务器端使用的版本要相同
    out << (qint64) 0;
    QString messageType = "ADDFRIEND";
    out << messageType;
    out << add;
    out.device()->seek(0);
    out << (qint64)(block.size() - sizeof(qint64));
    QTcpSocket *ClientSocket = getDestClient("addFriend", uid, NULL);
    connect(ClientSocket,SIGNAL(disconnected()),ClientSocket,SLOT(deleteLater()));
    ClientSocket->write(block);

    ClientSocket->waitForBytesWritten(3000);

#ifdef DEBUG
    qDebug() << "addfriend messages write back";
#endif
    removeConnect("addFriend", uid, NULL);
}

void MyThread::SendFriendsListMessage(QString uid)
{
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream out(&block,QIODevice::WriteOnly);
    //使用数据流写入数据
    out.setVersion(QDataStream::Qt_4_8);
    //设置数据流的版本,客户端和服务器端使用的版本要相同
    out << (qint64) 0;
    QString messageType = "FRIENDSLIST";
    out << messageType;

    //    发送在线状态列表
    QList<QString> onlineList;
    foreach (const ConnectionList conn, connectionList)
    {
#ifdef DEBUG
        qDebug()<<"id,type:"<<conn.uid<<" "<<conn.connectType;
#endif
        if(conn.connectType == "mainWidget")
            onlineList.append(conn.uid);
    }
    out << onlineList;

    QSqlQuery query;
    query.prepare("select * from usr_friendslist where uid = :uid");
    query.bindValue(":uid",uid);
    query.exec();
    while(query.next())
    {
        QString fid = query.value(1).toString();
        out << fid;
        QSqlQuery info_query;
        info_query.prepare("select * from usr_info where uid = :uid");
        info_query.bindValue(":uid",fid);
        info_query.exec();
        if(info_query.next())
        {
            QString friend_nickname = info_query.value(2).toString();
            out << friend_nickname;
        }
        QSqlQuery head_query;
        head_query.prepare("select * from usr_head_portrait where uid = :uid");
        head_query.bindValue(":uid",fid);
        head_query.exec();
        if(head_query.next())
        {
            QString friend_location = head_query.value(1).toString();
            out << friend_location;
        }
    }
    out.device()->seek(0);
    out << (qint64)(block.size() - sizeof(qint64));
#ifdef DEBUG
    qDebug()<<"send friendlist size:"<<(qint64)(block.size() - sizeof(qint64));
#endif
    QTcpSocket *ClientSocket = getDestClient("mainWidget", uid, NULL);
    connect(ClientSocket,SIGNAL(disconnected()),ClientSocket,SLOT(deleteLater()));
    ClientSocket->write(block);
}


void MyThread::SendChatMessage(QString fid,QString uid,QString msg)
{
    QSqlQuery query;
    QString nickname = "";
    query.prepare("select * from usr_info where uid = :uid");
    query.bindValue(":uid", uid);
    query.exec();
    if(query.next())
        nickname = query.value(2).toString();
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream out(&block,QIODevice::WriteOnly);
    //使用数据流写入数据
    out.setVersion(QDataStream::Qt_4_8);
    //设置数据流的版本,客户端和服务器端使用的版本要相同
    out<<(qint64) 0;
    QString messageType = "CHAT";
    out << messageType;
    out << uid << nickname << msg;
    out.device()->seek(0);
    out << (qint64)(block.size() - sizeof(qint64));
    QTcpSocket *ClientSocket = getDestClient("chat", fid, uid);
    if(ClientSocket == NULL)
    {
        SendChatCloseMessage(uid);
        return;
    }
    connect(ClientSocket,SIGNAL(disconnected()),ClientSocket,SLOT(deleteLater()));
    ClientSocket->write(block);
}

void MyThread::updateHeadLocation(QString uid, QString headLocation)
{
    QSqlQuery query;
    query.prepare("update usr_head_portrait set head_portrait_location = :head_portrait_location where uid = :uid");
    query.bindValue(":head_portrait_location",headLocation);
    query.bindValue(":uid",uid);
    query.exec();
}

void MyThread::removeConnect(QString connectType, QString uid, QString fid)
{
    if(uid != NULL && fid != NULL)
    {
        int i=0;
        foreach (const ConnectionList conn, connectionList)
        {
            if(conn.connectType == connectType && conn.uid == uid && conn.fid == fid)
            {
                conn.connectSocket->disconnectFromHost();
                connectionList.removeAt(i);
    #ifdef DEBUG
                qDebug() << "remove uid " << conn.uid;
                qDebug() << "remove connectType " << conn.connectType;
    #endif
                break;
            }
            i++;
        }
    }
    else if(fid == NULL)
    {
        int i=0;
        foreach (const ConnectionList conn, connectionList)
        {
            if(conn.connectType == connectType && conn.uid == uid)
            {
                conn.connectSocket->disconnectFromHost();
                connectionList.removeAt(i);
    #ifdef DEBUG
                qDebug() << "remove uid " << conn.uid;
                qDebug() << "remove connectType " << conn.connectType;
    #endif
                break;
            }
            i++;
        }
    }
    else if(uid == NULL)
    {
        int i=0;
        foreach (const ConnectionList conn, connectionList)
        {
            if(conn.connectType == connectType)
            {
                conn.connectSocket->disconnectFromHost();
                connectionList.removeAt(i);
    #ifdef DEBUG
                qDebug() << "remove uid " << conn.uid;
                qDebug() << "remove connectType " << conn.connectType;
    #endif
                break;
            }
            i++;
        }
    }
    else
    {
#ifdef DEBUG
        qDebug()<<"not exist the id!";
#endif
    }
}

void MyThread::SendChatCloseMessage(QString uid)
{
    QByteArray block; //用于暂存我们要发送的数据
    QDataStream out(&block,QIODevice::WriteOnly);
    //使用数据流写入数据
    out.setVersion(QDataStream::Qt_4_8);
    //设置数据流的版本,客户端和服务器端使用的版本要相同
    out << (qint64) 0;
    QString messageType = "CHATCLOSED";
    out <<messageType;
    out.device()->seek(0);
    out << (qint64)(block.size() - sizeof(qint64));
    QTcpSocket *ClientSocket = getDestClient("chat", uid, NULL);
    connect(ClientSocket,SIGNAL(disconnected()),ClientSocket,SLOT(deleteLater()));
    ClientSocket->write(block);
}
