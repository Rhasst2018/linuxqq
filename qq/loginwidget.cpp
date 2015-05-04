#include "loginwidget.h"
#include "ui_loginwidget.h"

LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);

    /*设置窗口最大大小*/
    this->setMaximumSize(243,471);
    /*设置窗口最小大小*/
    this->setMinimumSize(243,471);
    /*设置窗口起始位置*/
    this->move(1100,50);
    /*设置窗口标题*/
    this->setWindowTitle("QQ for Linux");
    /*设置密码显示模式*/
    ui->PasswdLineEdit->setEchoMode(QLineEdit::Password);

    /*设置窗口动画*/
    movie = new QMovie("./images/loginqq.gif");
    ui->QQGifLabel->setMovie(movie);
    movie->start();

    /*设置TCP连接*/
    M_TcpPort = 6666;


    /*设置用户名编辑行为焦点*/
    ui->UsrIDLineEdit->setFocus();

    /*信号和槽*/
    connect(ui->RegisterButton,SIGNAL(clicked()),this,SLOT(RegisterButton_Click_Slot()));
    connect(ui->LoginButton,SIGNAL(clicked()),this,SLOT(LoginButton_Click_Slot()));
    connect(ui->PasswdLineEdit,SIGNAL(returnPressed()),this,SLOT(LoginButton_Click_Slot()));
    //connect(M_TcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(DisplayError(QAbstractSocket::SocketError)));

}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::RegisterButton_Click_Slot()
{
    RegisterWidget *register_widget = new RegisterWidget();
    register_widget->show();

}

void LoginWidget::LoginButton_Click_Slot()
{    
    if(ui->UsrIDLineEdit->text() == "" || ui->PasswdLineEdit->text() == "")
    {
        QMessageBox::information(this,"infomation","Username or passwd can't be empty!",QMessageBox::Ok);
        ui->UsrIDLineEdit->setFocus();
        return ;
    }

    ConnnectToServer();  // connect to server
    SendLoginMessage(); // send login infoamtion to server
}

void LoginWidget::ConnnectToServer()
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
    QString connectType = "mainWidget";
    QString uid = ui->UsrIDLineEdit->text();
    out << messageType << connectType << uid;
    out.device()->seek(0);
    out << qint64(data.size() - sizeof(qint64));
#ifdef DEBUG
    qDebug()<<"client connect bytes send:"<<(qint64)(data.size() - sizeof(qint64));
#endif
    M_TcpSocket->write(data); // write message
}

void LoginWidget::SendLoginMessage()
{
#ifdef DEBUG
    qDebug() <<"send login message";
#endif
    QString uid = ui->UsrIDLineEdit->text();
    QString pwd = ui->PasswdLineEdit->text();

    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;

    QString messageType = "LOGIN";  // message type
    out << messageType << uid << pwd;
    out.device()->seek(0);
    out << qint64(data.size() - sizeof(qint64));
#ifdef DEBUG
    qDebug()<<"client login bytes send:"<<(qint64)(data.size() - sizeof(qint64));
#endif

    M_TcpSocket->write(data); // write message

#ifdef DEBUG
    qDebug()<<"client login write over";
#endif

}

void LoginWidget::DisplayError(QAbstractSocket::SocketError)
{
    QMessageBox::warning(this,"waring",M_TcpSocket->errorString(),QMessageBox::Ok);
    qDebug()<<M_TcpSocket->errorString();
    return;
}

void LoginWidget::ReadMessage()
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
        }
        in >> TotalBytes;
        qDebug()<<"TotalBytes:"<<TotalBytes;
    }

    if(M_TcpSocket->bytesAvailable() < TotalBytes)
    {
#ifdef DEBUG
        qDebug()<<" the data is not full !";
#endif
        return ;
    }

    QString messageType;
    bool login;
    in >> messageType ;
#ifdef DEBUG
        qDebug()<<"login widget message type:"<<messageType;
#endif
    if(messageType != "LOGIN")
    {
#ifdef DEBUG
        qDebug()<<" wrong message type !";
#endif
        return;
    }
    bytesReceived += messageType.size();

    in >> login;
    bytesReceived += sizeof(login);

    if(login)
    {
#ifdef DEBUG
        qDebug() <<" login: "<<login;
#endif
        MainWidget *main_widget = new MainWidget(ui->UsrIDLineEdit->text(), M_TcpSocket);
        main_widget->show();
        disconnect(M_TcpSocket, SIGNAL(readyRead()), this, SLOT(ReadMessage()));
        this->hide();
    }
    else
    {
        QMessageBox::warning(this,"infomation","username or passwd is wrong!",QMessageBox::Ok);
        ui->UsrIDLineEdit->clear();
        ui->PasswdLineEdit->clear();
        ui->UsrIDLineEdit->setFocus();
    }
}
