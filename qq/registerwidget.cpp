#include "registerwidget.h"
#include "ui_registerwidget.h"

RegisterWidget::RegisterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterWidget)
{
    ui->setupUi(this);

    this->setWindowTitle("Register QQ");
    //pixmap.load("./images/register.png");
    //ui->PictureLabel->setPixmap(pixmap);
    //ui->PictureLabel->resize(pixmap.size());
    this->move(500,250);

    /*设置密码显示模式*/
    ui->PasswdLineEdit->setEchoMode(QLineEdit::Password);
    ui->CheckPasswdLineEdit->setEchoMode(QLineEdit::Password);

    /*设置TCP连接*/
    M_TcpPort = 6666;
    M_TcpSocket = new QTcpSocket();

    connect(M_TcpSocket, SIGNAL(readyRead()), this, SLOT(ReadMessage()));
    connect(M_TcpSocket, SIGNAL(disconnected()), M_TcpSocket, SLOT(deleteLater()));
   // connect(M_TcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(DisplayError(QAbstractSocket::SocketError)));

}

RegisterWidget::~RegisterWidget()
{
    delete ui;
}

void RegisterWidget::on_RegisterButton_clicked()
{
    if(ui->UsrNickNameLineEdit->text() == ""
            || ui->PasswdLineEdit->text() == "" || ui->CheckPasswdLineEdit->text() == "")
    {
        QMessageBox::information(this,"infomation","register information can't be empty!",QMessageBox::Ok);
        ui->UsrNickNameLineEdit->setFocus();
        return;
    }

    QString pwd = ui->PasswdLineEdit->text();
    QString cpwd = ui->CheckPasswdLineEdit->text();

    if(pwd != cpwd)
    {
       QMessageBox::information(this,"infomation","confirmed passwd is not same as passwd!",QMessageBox::Ok);
       ui->PasswdLineEdit->setFocus();
       return;
    }

    ConnnectToServer();
    SendRegisterMessage();
}

void RegisterWidget::ConnnectToServer()
{
    M_TcpSocket->abort(); //取消原有连接
    M_TcpSocket->connectToHost("127.0.0.1",M_TcpPort);
    M_TcpSocket->waitForConnected();

    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;
    QString messageType = "CONNECT";  // message type
    QString connectType = "register";
    out << messageType << connectType;
    out.device()->seek(0);
    out << qint64(data.size() - sizeof(qint64));
    M_TcpSocket->write(data); // write message
}

void RegisterWidget::SendRegisterMessage()
{
    QString nickname = ui->UsrNickNameLineEdit->text();
    QString pwd = ui->PasswdLineEdit->text();

    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;

    QString messageType = "REGISTER";  // message type
    out << messageType << nickname << pwd;
    out.device()->seek(0);
    out << qint64(data.size() - sizeof(qint64));

    M_TcpSocket->write(data); // write message
}

void RegisterWidget::ReadMessage()
{
    qint64 TotalBytes = 0,bytesReceived = 0;
    QString uid;
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
    bool registed;
    in >> messageType;
    if(messageType != "REGISTER")
    {
#ifdef DEBUG
        qDebug()<<" wrong message type !";
#endif
        return;
    }
    bytesReceived += messageType.size();

    in >> registed;
    bytesReceived += sizeof(registed);

    if(registed)
    {
        in >> uid;
        QMessageBox::information(this,"infomation","register successfully!\nPlease remember your ID is " + uid,QMessageBox::Ok);
        this->close();
    }
    else
    {
        QMessageBox::warning(this,"warning","register failed!",QMessageBox::Ok);
        return;
    }
}

void RegisterWidget::DisplayError(QAbstractSocket::SocketError)
{
    QMessageBox::warning(this,"waring",M_TcpSocket->errorString(),QMessageBox::Ok);
    qDebug()<<M_TcpSocket->errorString();
    return;
}
