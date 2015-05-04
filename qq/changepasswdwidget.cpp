#include "changepasswdwidget.h"
#include "ui_changepasswdwidget.h"

ChangePasswdWidget::ChangePasswdWidget(QString uid, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChangePasswdWidget)
{
    ui->setupUi(this);
    this->setWindowTitle("Change Passwd");

    /*设置密码显示模式*/
    ui->OldPasswdLineEdit->setEchoMode(QLineEdit::Password);
    ui->NewPasswdLineEdit->setEchoMode(QLineEdit::Password);
    ui->CheckPasswdLineEdit->setEchoMode(QLineEdit::Password);
    this->move(500,250);

    /*设置TCP连接*/
    M_TcpPort = 6666;
    M_uid = uid;

    connect(ui->CheckPasswdLineEdit, SIGNAL(returnPressed()),this,SLOT(on_SubmitButton_clicked()));

}

ChangePasswdWidget::~ChangePasswdWidget()
{
    delete ui;
}

void ChangePasswdWidget::on_SubmitButton_clicked()
{
    QString old_pwd = ui->OldPasswdLineEdit->text();
    QString new_pwd = ui->NewPasswdLineEdit->text();
    QString new_cpwd = ui->CheckPasswdLineEdit->text();
    if(old_pwd.isEmpty() || new_pwd.isEmpty() || new_cpwd.isEmpty())
    {
        QMessageBox::information(this,tr("information"),tr("please input all information!"),QMessageBox::Ok);
        return;
    }
    if(new_pwd != new_cpwd)
    {
        QMessageBox::information(this,tr("information"),tr("confirmed new passwd is not same as new passwd!"),QMessageBox::Ok);
        return;
    }

    ConnnectToServer();
    SendChangePasswdMessage();
}

void ChangePasswdWidget::ConnnectToServer()
{
    M_TcpSocket = new QTcpSocket();
    connect(M_TcpSocket, SIGNAL(readyRead()), this, SLOT(ReadMessage()));
    connect(M_TcpSocket, SIGNAL(disconnected()), M_TcpSocket, SLOT(deleteLater()));

    M_TcpSocket->abort(); //取消原有连接
    M_TcpSocket->connectToHost("127.0.0.1",M_TcpPort);
    M_TcpSocket->waitForConnected();

    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;
    QString messageType = "CONNECT";  // message type
    QString connectType = "changePasswd";
    out << messageType << connectType << M_uid;
    out.device()->seek(0);
    out << qint64(data.size() - sizeof(qint64));
    M_TcpSocket->write(data); // write message
}

void ChangePasswdWidget::SendChangePasswdMessage()
{
    QString old_pwd = ui->OldPasswdLineEdit->text();
    QString new_pwd = ui->NewPasswdLineEdit->text();

    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;

    QString messageType = "CHANGEPASSWD";  // message type
    out << messageType << M_uid << old_pwd << new_pwd;
    out.device()->seek(0);
    out << qint64(data.size() - sizeof(qint64));

    M_TcpSocket->write(data); // write message
}

void ChangePasswdWidget::ReadMessage()
{
    qint64 TotalBytes = 0,bytesReceived = 0;
    //QByteArray inBlock;
    QDataStream in(M_TcpSocket);
    in.setVersion(QDataStream::Qt_4_8); //设置数据流版本，与客户端数据流版本保持一致

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
    }

    if(M_TcpSocket->bytesAvailable() < TotalBytes)
    {
#ifdef DEBUG
        qDebug()<<" the data is not full !";
#endif
        return ;
    }

    QString messageType;
    bool modified;
    in >> messageType ;
    if(messageType != "CHANGEPASSWD")
    {
#ifdef DEBUG
        qDebug()<<" wrong message type !";
#endif
        return ;
    }
    bytesReceived += messageType.size();

    in >> modified;
    bytesReceived += sizeof(modified);

    if(modified)
    {
        QMessageBox::information(this,tr("infomation"),tr("update passwd successfully!"),QMessageBox::Ok);
        this->close();
    }
    else
    {
        QMessageBox::information(this,tr("infomation"),tr("update passwd failed!"),QMessageBox::Ok);
        ui->OldPasswdLineEdit->text().clear();
        ui->NewPasswdLineEdit->text().clear();
        ui->CheckPasswdLineEdit->text().clear();
        ui->OldPasswdLineEdit->setFocus();
    }
}
