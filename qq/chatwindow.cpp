#include "chatwindow.h"
#include "ui_chatwindow.h"

#include <QFile>
#include <QFileDialog>

ChatWindow::ChatWindow(QString fid, QString uid, QString nickname, QString friend_nickname ,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChatWindow)
{
    ui->setupUi(this);

    /*初始化FID UID*/
    M_fid = fid;
    M_uid = uid;

    M_nickname = nickname;
    M_friendnickname = friend_nickname;

    M_TcpSocket = new QTcpSocket();
    ConnnectToServer();   /*聊天窗口建立自己的连接，以免与主窗口的消息混合*/

    /*设置窗口标题*/
    this->setWindowTitle("Chat With " + M_friendnickname);


    /*设置获取焦点的策略*/
    ui->SendMesgTextEdit->setFocusPolicy(Qt::StrongFocus);
    ui->MesgTextBrowser->setFocusPolicy(Qt::NoFocus);

    ui->SendMesgTextEdit->setFocus();
    ui->SendMesgTextEdit->installEventFilter(this);


    /*信号和槽*/
    connect(ui->TextBoldToolButton,SIGNAL(clicked(bool)),this,SLOT(TextBoldBtnClicked(bool)));
    connect(ui->TextItalicToolButton,SIGNAL(clicked(bool)),this,SLOT(TextItalicToolBtnClicked(bool)));
    connect(ui->TextUnderlineToolButton,SIGNAL(clicked(bool)),this,SLOT(TextUnderlineBtnClicked(bool)));
    connect(ui->TextColorToolButton,SIGNAL(clicked()),this,SLOT(TextColorBtnClicked()));
    connect(ui->SendFileToolButton,SIGNAL(clicked()),this,SLOT(SendFileBtnClicked()));
    connect(ui->ClearMsgToolButton,SIGNAL(clicked()),this,SLOT(ClearMesgToolBtnClicked()));
    connect(ui->SendButton,SIGNAL(clicked()),this,SLOT(SendBtnClicked()));
    connect(ui->CloseButton,SIGNAL(clicked()),this,SLOT(CloseBtnClicked()));

    connect(ui->FontComboBox,SIGNAL(currentFontChanged(QFont)),this,SLOT(FontComboBox_currentFontChanged(QFont)));
    connect(ui->FontSizeComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(FontsizeComboBox_currentIndexChanged(QString)));

    /*关联服务端发来数据的槽*/
    connect(M_TcpSocket,SIGNAL(readyRead()),this,SLOT(ReceiveMessage()));
    //connect(M_TcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(DisplayError(QAbstractSocket::SocketError)));
    connect(M_TcpSocket, SIGNAL(disconnected()), M_TcpSocket, SLOT(deleteLater()));
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::ConnnectToServer()
{
    M_TcpSocket->abort(); //取消原有连接
    M_TcpSocket->connectToHost("127.0.0.1",6666);
    M_TcpSocket->waitForConnected();

    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;
    QString messageType = "CONNECT";  // message type
    QString connectType = "chat";
    out << messageType << connectType << M_uid << M_fid;
    out.device()->seek(0);
    out << qint64(data.size() - sizeof(qint64));
    M_TcpSocket->write(data); // write message
}

bool ChatWindow::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->SendMesgTextEdit)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *k = static_cast<QKeyEvent *>(event);
            if(k->key() == Qt::Key_Return)
            {
                SendBtnClicked();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(target, event);
}



QString ChatWindow::GetMessage()
{
    QString msg = ui->SendMesgTextEdit->toHtml();

    ui->SendMesgTextEdit->clear();
    ui->SendMesgTextEdit->setFocus();
    return msg;
}

void ChatWindow::SendReceiveFileMessage(QString filename)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;
    QString messageType = "CHAT";
    out << messageType;
    out << M_fid << M_uid;

    QString msg = "receive file:" + filename + " successfully";
#ifdef DEBUG
    qDebug()<<"chat tcpclient msg:"<<msg;
#endif
    out << msg;
    out.device()->seek(0);
    out << qint64( data.size() - sizeof(qint64));
#ifdef DEBUG
    qDebug()<<"block size:"<<(qint64( data.size() - sizeof(qint64)));
#endif
    M_TcpSocket->write(data);

    /*将发送的消息显示到聊天窗口*/
    ShowMessageInTextBrowser(M_nickname,msg,true);
}

void ChatWindow::SendRefusedFileMessage(QString filename)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;
    QString messageType = "CHAT";
    out << messageType;
    out << M_fid << M_uid;

    QString msg = "refused receive file:" + filename ;
#ifdef DEBUG
    qDebug()<<"chat tcpclient msg:"<<msg;
#endif
    out << msg;
    out.device()->seek(0);
    out << qint64( data.size() - sizeof(qint64));
#ifdef DEBUG
    qDebug()<<"block size:"<<(qint64( data.size() - sizeof(qint64)));
#endif
    M_TcpSocket->write(data);

    /*将发送的消息显示到聊天窗口*/
    ShowMessageInTextBrowser(M_nickname,msg,true);
}

void ChatWindow::SendMessage()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;
    QString messageType = "CHAT";
    out << messageType;
#ifdef DEBUG
    qDebug()<<"chat tcpclient message type:"<<messageType;
#endif
    out << M_fid << M_uid;
#ifdef DEBUG
    qDebug()<<"chat tcpclient fid:"<<M_fid;
#endif
    if(ui->SendMesgTextEdit->toPlainText() == "")
    {
        QMessageBox::warning(this,"warning",tr("the message send can't be empty!"),QMessageBox::Ok);
        return;
    }

    QString msg = GetMessage();
#ifdef DEBUG
    qDebug()<<"chat tcpclient msg:"<<msg;
#endif
    out << msg;
    out.device()->seek(0);
    out << qint64( data.size() - sizeof(qint64));
#ifdef DEBUG
    qDebug()<<"block size:"<<(qint64( data.size() - sizeof(qint64)));
#endif
    M_TcpSocket->write(data);

    /*将发送的消息显示到聊天窗口*/
    ShowMessageInTextBrowser(M_nickname,msg,true);

}


void ChatWindow::TextBoldBtnClicked(bool checked)
{
    if(checked)
        ui->SendMesgTextEdit->setFontWeight(QFont::Bold);
    else
        ui->SendMesgTextEdit->setFontWeight(QFont::Normal);
    ui->SendMesgTextEdit->setFocus();
}

void ChatWindow::TextItalicToolBtnClicked(bool checked)
{
    ui->SendMesgTextEdit->setFontItalic(checked);
    ui->SendMesgTextEdit->setFocus();
}

void ChatWindow::TextUnderlineBtnClicked(bool checked)
{
    ui->SendMesgTextEdit->setFontUnderline(checked);
    ui->SendMesgTextEdit->setFocus();
}

void ChatWindow::TextColorBtnClicked()
{
    M_color = QColorDialog::getColor(M_color,this);
    if(M_color.isValid())
    {
        ui->SendMesgTextEdit->setTextColor(M_color);
        ui->SendMesgTextEdit->setFocus();
    }
}

void ChatWindow::SendFileBtnClicked()
{
    qint64 FileSize;
    QString fileName = QFileDialog::getOpenFileName(this);
    if(fileName.isEmpty())
        return;
    QFile *localFile = new QFile(fileName);
    if(!localFile->open((QFile::ReadOnly))){//以只读方式打开
        QMessageBox::warning(this,tr("information"),tr("can not read file %1:\n%2").arg(fileName).arg(localFile->errorString()));
        return;
    }
    FileSize = localFile->size();
    QByteArray data;//缓存一次发送的数据
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    QString messageType = "SENDFILE";
    QString currentFile = fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);
    out << qint64(0);
    out << messageType << M_fid << M_uid << currentFile << FileSize;
    qDebug()<<"messageType size:"<<(qint64)messageType.size();
    qDebug()<<"fid size:"<<(qint64)M_fid.size();
    qDebug()<<"uid size:"<<(qint64)M_uid.size();
    qDebug()<<"currentfile size:"<<(qint64)currentFile.size();
    qDebug()<<"filesize size:"<<sizeof(qint64);
    qDebug()<<currentFile<<" "<<FileSize;
    QByteArray fileBlock;
//    fileBlock = localFile->read(FileSize);
    fileBlock = localFile->readAll();
    out << fileBlock;
    qDebug()<<"read file size:"<<fileBlock.size();
    fileBlock.resize(0);
    out.device()->seek(0);
    out << qint64(data.size() - sizeof(qint64));
    qDebug()<<"send file message size:"<<(qint64( data.size() - sizeof(qint64)));
    M_TcpSocket->write(data);
    localFile->close();
    SendFileMessage(currentFile);


}

void ChatWindow::SendFileMessage(QString filename)
{ 
    QString msg = "send file:" + filename;
    /*将发送的消息显示到聊天窗口*/
    ShowMessageInTextBrowser(M_nickname,msg,true);
}

void ChatWindow::ClearMesgToolBtnClicked()
{
    ui->MesgTextBrowser->clear();
}

void ChatWindow::SendBtnClicked()
{
    SendMessage();
}

void ChatWindow::CloseBtnClicked()
{
    this->close();

}

void ChatWindow::FontComboBox_currentFontChanged(QFont f)
{
    ui->SendMesgTextEdit->setCurrentFont(f);
    ui->SendMesgTextEdit->setFocus();
}

void ChatWindow::FontsizeComboBox_currentIndexChanged(QString size)
{
    ui->SendMesgTextEdit->setFontPointSize(size.toDouble());
    ui->SendMesgTextEdit->setFocus();
}

void ChatWindow::ReceiveMessage()
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
            return ;
        }
        in >> TotalBytes;
    }

    QString messageType;
    in >> messageType;
    bytesReceived += (qint64)sizeof(messageType);
    if( messageType == "CHAT")
    {
        QString id, nickname, msg;
        in >> id >> nickname >> msg;
        bytesReceived += (qint64)id.size();
        bytesReceived += (qint64)nickname.size();
        bytesReceived += (qint64)msg.size();
        if(bytesReceived < TotalBytes )
        {
            QByteArray BLOCK;
            BLOCK = M_TcpSocket->readAll();
            BLOCK.resize(0);
        }
#ifdef DEBUG
        qDebug() <<"tcp client id:"<< id;
        qDebug() <<"tcp client nickname:"<< nickname;
        qDebug() <<"tcp client msg:"<< msg;
#endif

        /*将接收的消息显示到聊天窗口*/
        ShowMessageInTextBrowser(nickname, msg, false);
    }
    else if(messageType == "CHATCLOSED")
    {
        int btn = QMessageBox::information(this,tr("infomation"),
                                           tr("%1 closed chat!") .arg(M_friendnickname),
                                           QMessageBox::Ok);
        if(btn == QMessageBox::Ok)
        {
            this->close();
        }
    }
    else if( messageType == "SENDFILE")
    {
        QString fid, uid, currentFile;
        qint64 fileSize;
        in >> fid >> uid >> currentFile >> fileSize ;
        bytesReceived += (qint64)sizeof(fid);
        bytesReceived += (qint64)sizeof(uid);
        bytesReceived += (qint64)sizeof(currentFile);
        bytesReceived += (qint64)sizeof(qint64);
        bytesReceived += fileSize;
        if(bytesReceived < TotalBytes )
        {
            qDebug()<<"byteReceived:"<<bytesReceived;
            qDebug()<<"TotalBytes:"<<TotalBytes;
            qDebug()<<"chat send file receive file not full";
        }
        int btn = QMessageBox::information(this,tr("receive file"),
                                 tr("file send from:%1:%2,receive or refused?")
                                 .arg(M_friendnickname).arg(currentFile),
                                 QMessageBox::Yes,QMessageBox::No);
        if(btn == QMessageBox::Yes)
        {
            QString savename = QFileDialog::getSaveFileName(0,tr("save file"),currentFile, tr("Files (*.txt *.c *.jpg *.*)"));
            QFile *localFile = new QFile(savename);
            if(!localFile->open(QFile::WriteOnly)){
            QMessageBox::warning(this,tr("information"),tr("can not read file %1:\n%2.").arg(currentFile).arg(localFile->errorString()));
            return;
            }
//            localFile->write(M_TcpSocket->read(fileSize));
            localFile->write(M_TcpSocket->readAll());
            localFile->close();
            SendReceiveFileMessage(currentFile);
        }
        else
        {
            SendRefusedFileMessage(currentFile);
        }
    }
    else
    {
        qDebug()<<"wrong message type:"<<messageType;
        return;
    }


}

void ChatWindow::ShowMessageInTextBrowser(QString nickname, QString msg, bool self)
{
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    if(self)
    {
        ui->MesgTextBrowser->setTextColor(Qt::blue);
    }
    else
    {
        ui->MesgTextBrowser->setTextColor(Qt::green);
    }
    ui->MesgTextBrowser->setCurrentFont(QFont("Times New Roman",12));
    ui->MesgTextBrowser->append("[ " +nickname +" ] "+ time);
    ui->MesgTextBrowser->append(msg);
}

void ChatWindow::DisplayError(QAbstractSocket::SocketError)
{
    QMessageBox::warning(this,"waring",M_TcpSocket->errorString(),QMessageBox::Ok);
    qDebug()<<M_TcpSocket->error();
    return;
}

void ChatWindow::closeEvent(QCloseEvent *)
{
    emit chatClose(M_fid);

    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (qint64)0;
    QString messageType = "CLOSECHAT";  // message type
    out << messageType << M_uid << M_fid;
    out.device()->seek(0);
    out << qint64(data.size() - sizeof(qint64));

    M_TcpSocket->write(data); // write message
    M_TcpSocket->waitForBytesWritten(3000);
    M_TcpSocket->disconnectFromHost();
#ifdef DEBUG
    qDebug() << "send chat close message";
#endif
}
