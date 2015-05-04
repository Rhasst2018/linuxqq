#include "mainwidget.h"
#include "ui_mainwidget.h"


MainWidget::MainWidget(QString uid, QTcpSocket *TcpSocket, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->setWindowTitle("QQ For Linux");

    /*初始化UID*/
    M_uid = uid;

    /*设置TCP SOCKET*/
    M_TcpSocket = TcpSocket;

    /*设置昵称*/
    ui->NickNameLabel->text().clear();
    ui->IDlabel->text().clear();
    ui->IDlabel->setText(M_uid);

    ui->HeadPushButton->setFlat(true);

    M_nickname = "";

    /*固定窗口大小*/
    this->setMaximumSize(270,530);
    this->setMinimumSize(270,530);

    /*设置窗口起始位置*/
    this->move(1100,50);

    SendMessage("GETNICKNAME");
    SendMessage("GETLOCATION");
    SendMessage("FRIENDSLIST");

    connect(M_TcpSocket,SIGNAL(readyRead()),this,SLOT(ReadMessage()));
    connect(M_TcpSocket, SIGNAL(disconnected()), M_TcpSocket, SLOT(deleteLater()));
    //connect(M_TcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(DisplayError(QAbstractSocket::SocketError)));

    connect(ui->ChangePasswdButton,SIGNAL(clicked()),this,SLOT(ChangePasswdButton_Click_Slot()));
    connect(ui->AddFriendButton,SIGNAL(clicked()),this,SLOT(SearchBtnClicked()));
    connect(ui->HeadPushButton,SIGNAL(clicked()),this,SLOT(changeHeadPortrait()));
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::SendMessage(QString messageType)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;
    if(messageType == "GETNICKNAME" || messageType == "GETLOCATION" || messageType == "FRIENDSLIST")
    {
        out << messageType;
#ifdef DEBUG
        qDebug()<<"main widget tcpclient msg type:"<<messageType;
#endif
        out << M_uid;
    }
    out.device()->seek(0);
    out << qint64( data.size() - sizeof(qint64));
#ifdef DEBUG
    qDebug()<<"send message size:"<<(qint64( data.size() - sizeof(qint64)));
#endif
    M_TcpSocket->write(data);
}

void MainWidget::ReadMessage()
{
    qint64 TotalBytes = 0,bytesReceived = 0;
    QDataStream in(M_TcpSocket);
    in.setVersion(QDataStream::Qt_4_8); //设置数据流版本，与客户端数据流版本保持一致

    while(!in.atEnd()) {
        TotalBytes = 0;
        if(TotalBytes == 0)
        {
            if(M_TcpSocket->bytesAvailable() < (int)sizeof(qint64))
            {
#ifdef DEBUG
                qDebug()<<" no bytes available !";
#endif
            }
            in >> TotalBytes;
        }

        if(M_TcpSocket->bytesAvailable() < TotalBytes)
        {
#ifdef DEBUG
            qDebug()<<" the data is not full !";
#endif
            return ;
        }

        QString messageType;
        in >> messageType ;
#ifdef DEBUG
        qDebug() << "main widget read message type:"<<messageType;
#endif
        bytesReceived += messageType.size();
        if(messageType == "GETNICKNAME")
        {
#ifdef DEBUG
            qDebug() << "main widget set nickname";
#endif
            QString nickname;
            in >> nickname;
            M_nickname = nickname;
            bytesReceived += sizeof(nickname);
            ui->NickNameLabel->setText(nickname);
        }
        else if(messageType == "GETLOCATION")
        {
#ifdef DEBUG
            qDebug() << "main widget set headportrait";
#endif
            QString location;
            in >> location;
            bytesReceived += sizeof(location);
            /*设置头像图标*/
            ui->HeadPushButton->setIcon(QPixmap( location));
        }
        else if(messageType == "FRIENDSLIST")
        {
            RefreshFriendsList(&in);
        }
        else
        {
#ifdef DEBUG
            qDebug()<<"wrong message type";
#endif
        }
    }
//    disconnect(M_TcpSocket,SIGNAL(readyRead()),this,SLOT(ReadMessage()));

}

void MainWidget::ToolBtnClicked(QString fid, QString friend_nickname)
{
    bool chatbool = true;
    QMutableLinkedListIterator<QString> chatIterator(chatList);

    while(chatIterator.hasNext())
    {
        if(chatIterator.next() == fid)
        {
            chatbool = false;
            break;
        }
    }

    if(chatbool)
    {
        chatList << fid;
        ChatWindow *chat_window = new ChatWindow(fid,M_uid,M_nickname,friend_nickname);
        connect(chat_window,SIGNAL(chatClose(QString)),this,SLOT(chatClosed(QString)));
        chat_window->show();
    }
}

void MainWidget::SearchBtnClicked()
{
    AddFriendWidget *addfriend_widget = new AddFriendWidget(M_uid);
    connect(addfriend_widget,SIGNAL(addedFriendsSignal()),this,SLOT(addedFriendsSlot()));
    addfriend_widget->show();
}

void MainWidget::ChangePasswdButton_Click_Slot()
{
    ChangePasswdWidget *changepasswd_widget = new ChangePasswdWidget(M_uid);
    changepasswd_widget->show();
}

void MainWidget::DisplayError(QAbstractSocket::SocketError)
{
    QMessageBox::warning(this,"waring",M_TcpSocket->errorString(),QMessageBox::Ok);
    qDebug()<<M_TcpSocket->errorString();
    return;
}

void MainWidget::RefreshFriendsList(QDataStream *in)
{
#ifdef DEBUG
    qDebug() << "main widget generate friend list";
#endif
    QGroupBox *groupBox = new QGroupBox();
    QVBoxLayout *layout = new QVBoxLayout(groupBox);
    QString state = "[off-line]";

    layout->setMargin(10);
    layout->setAlignment(Qt::AlignLeft);

    //让服务端把在线列表和好友列表一起发过来，在线状态表放在前面
    onlineList.clear();
    *in >> onlineList;

    QString id, nickname, location;
    while(!(*in).atEnd())
    {
        *in >> id >> nickname >> location;
        if(id.isEmpty())
            return;
        //bytesReceived += sizeof(id);
        //bytesReceived += sizeof(nickname);
        //bytesReceived += sizeof(location);
        state = "[off-line]";

        foreach (const QString &str, onlineList)
            if(str == id)
            {
#ifdef DEBUG
                qDebug()<<str<<"---online";
#endif
                state = "[on-line]";
                break;
            }

        MyToolButton *toolButton = new MyToolButton(id, nickname);
        toolButton->setText(nickname+"\n"+id+" "+state);
        toolButton->setIcon( QPixmap(location) );
        toolButton->setIconSize(QSize(45,45));
        toolButton->setAutoRaise( TRUE );
        toolButton->setFixedWidth(200);
        toolButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon);
        if(state == "[off-line]")
            toolButton->setEnabled(false);
        if(state == "[on-line]")
            connect(toolButton, SIGNAL(MyToolButtonClicked(QString, QString)), this, SLOT(ToolBtnClicked(QString, QString)));
        layout->addWidget(toolButton);
    }
    layout->addStretch();
    ui->FriendListTabWidget->clear();
    ui->FriendListTabWidget->addTab(( QWidget* )groupBox , tr("Friends List" ));
    /*添加黑名单列表*/
    QGroupBox *black_groupBox = new QGroupBox();
    QVBoxLayout *black_layout = new QVBoxLayout(black_groupBox);
    black_layout->setMargin(10);
    black_layout->setAlignment(Qt::AlignHCenter);
    black_layout->addStretch();
    ui->FriendListTabWidget->addTab(( QWidget* )black_groupBox , tr("Black List" ));
}

void MainWidget::changeHeadPortrait()
{
    QString headPath = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                                     "./images",
                                                     tr("Images (*.png *.xpm *.jpg)"));
    if(headPath.isEmpty())
        return;

    ui->HeadPushButton->setIcon(QPixmap(headPath));

    std::string command = QString("cp "+headPath+" "+"./images").toStdString();
    system(command.c_str());
    QString headName;
    headName = headPath.mid(headPath.lastIndexOf("/")+1);
    headPath = "./images/"+headName;

    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;
    QString messageType = "CHANGEHEADLOCATION";  // message type
    out << messageType << M_uid << headPath;
    out.device()->seek(0);
    out << qint64(data.size() - sizeof(qint64));

    M_TcpSocket->write(data); // write message
}

void MainWidget::chatClosed(QString fid)   /*聊天窗口关闭后从聊天列表里删除*/
{
    if(chatList.removeOne(fid))
    {
#ifdef DEBUG
        qDebug()<<"remove chat success";
#endif
    }
    else
    {
#ifdef DEBUG
        qDebug()<<"remove chat failure";
#endif
    }
}


void MainWidget::closeEvent(QCloseEvent *)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;
    QString messageType = "OFFLINE";  // message type
    out << messageType << M_uid;
    out.device()->seek(0);
    out << qint64(data.size() - sizeof(qint64));

    M_TcpSocket->write(data); // write message
    M_TcpSocket->waitForBytesWritten(3000);
    M_TcpSocket->disconnectFromHost();
#ifdef DEBUG
    qDebug() << "send offline message";
#endif
}

void MainWidget::addedFriendsSlot()
{
#ifdef DEBUG
    qDebug() << "addedFriendsSlot socketDescriptor" << M_TcpSocket->socketDescriptor();
#endif
    SendMessage("FRIENDSLIST"); /*添加好友后，再发一次，用来刷新好友列表*/
}
