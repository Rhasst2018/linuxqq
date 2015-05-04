#include "addfriendwidget.h"
#include "ui_addfriendwidget.h"

AddFriendWidget::AddFriendWidget(QString uid, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddFriendWidget)
{
    ui->setupUi(this);

    this->setWindowTitle("Add Friend");

    /*初始化UID*/
    M_uid = uid;

    /*固定窗口大小*/
    this->setMaximumSize(297,200);
    this->setMinimumSize(297,200);
    this->move(500,250);

    /*信号和槽*/
    connect(ui->SubmitButton,SIGNAL(clicked()),this,SLOT(SubmitBtnClicked()));

}

AddFriendWidget::~AddFriendWidget()
{
    delete ui;
}

void AddFriendWidget::SubmitBtnClicked()
{
    if(ui->UsrIDLineEdit->text().isEmpty())
    {
        QMessageBox::information(this,"information","please input the usr id!",QMessageBox::Ok);
        return;
    }

    ConnnectToServer();
    SendAddFriendMessage();
}

void AddFriendWidget::ConnnectToServer()
{
    M_TcpSocket = new QTcpSocket();
    connect(M_TcpSocket, SIGNAL(readyRead()), this, SLOT(ReadMessage()));
    connect(M_TcpSocket, SIGNAL(disconnected()), M_TcpSocket, SLOT(deleteLater()));

    M_TcpSocket->abort(); //取消原有连接
    M_TcpSocket->connectToHost("127.0.0.1",6666);
    M_TcpSocket->waitForConnected();

    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;
    QString messageType = "CONNECT";  // message type
    QString connectType = "addFriend";
    out << messageType << connectType << M_uid;
    out.device()->seek(0);
    out << qint64(data.size() - sizeof(qint64));
    M_TcpSocket->write(data); // write message
}

void AddFriendWidget::SendAddFriendMessage()
{
    QString fid = ui->UsrIDLineEdit->text();

    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;

    QString messageType = "ADDFRIEND";  // message type
    out << messageType << M_uid << fid;
    out.device()->seek(0);
    out << qint64(data.size() - sizeof(qint64));

    M_TcpSocket->write(data); // write message
}

void AddFriendWidget::ReadMessage()
{
    qint64 TotalBytes = 0,bytesReceived = 0;
    QDataStream in(M_TcpSocket);
    in.setVersion(QDataStream::Qt_4_8); //设置数据流版本，与客户端数据流版本保持一致

    if(TotalBytes == 0)
    {
        if(M_TcpSocket->bytesAvailable() < (int)sizeof(qint64))
        {
#ifdef DEBUG
            qDebug()<<" no bytes available !";
#endif
            return;
        }
        in >> TotalBytes;
    }

    if(M_TcpSocket->bytesAvailable() < TotalBytes)
    {
#ifdef DEBUG
        qDebug()<<" the data is not full !";
#endif
        return;
    }

    QString messageType;
    bool add;
    in >> messageType ;
    if(messageType != "ADDFRIEND")
    {
#ifdef DEBUG
        qDebug()<<" wrong message type !";
#endif
        return;
    }
    bytesReceived += messageType.size();

    in >> add;
    bytesReceived += sizeof(add);

    if(add)
    {
        QMessageBox::information(this,tr("infomation"),tr("add friend successfully!"),QMessageBox::Ok);
        emit addedFriendsSignal();
        this->close();
        return;
    }
    else
    {
        QMessageBox::information(this,tr("infomation"),tr("add friend failed!"),QMessageBox::Ok);
        this->close();
        return;
    }
}
