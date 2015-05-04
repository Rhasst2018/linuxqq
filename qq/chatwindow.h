#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#define DEBUG

#include <QMainWindow>
#include <QColor>
#include <QColorDialog>
#include <QTextCharFormat>
#include <QKeyEvent>
#include <QtNetwork>
#include <QTextStream>
#include <QMessageBox>
#include <QScrollBar>

#ifdef DEBUG
#include <QDebug>
#endif

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChatWindow(QString fid, QString uid, QString nickname, QString friend_nickname, QWidget *parent = 0);
    ~ChatWindow();

private:
    Ui::ChatWindow *ui;
    QString M_fid;
    QString M_uid;
    QColor M_color;
    QTcpSocket *M_TcpSocket;
    QString M_nickname;
    QString M_friendnickname;


private:
    void SendMessage();
    QString GetMessage();
    void ShowMessageInTextBrowser(QString nickname, QString msg, bool self);
    void SendFileMessage(QString filename);
    void SendReceiveFileMessage(QString filename);
    void SendRefusedFileMessage(QString filename);
    void ConnnectToServer();

protected:
    bool eventFilter(QObject *target, QEvent *event);
    void closeEvent(QCloseEvent *);

private slots:
    void TextBoldBtnClicked(bool checked);
    void TextItalicToolBtnClicked(bool checked);
    void TextUnderlineBtnClicked(bool checked);
    void TextColorBtnClicked();
    void SendFileBtnClicked();
    void ClearMesgToolBtnClicked();
    void SendBtnClicked();
    void CloseBtnClicked();
    void FontComboBox_currentFontChanged(QFont f);
    void FontsizeComboBox_currentIndexChanged(QString );

    void ReceiveMessage(); //接收消息
    void DisplayError(QAbstractSocket::SocketError); //显示错误

signals:
    void chatClose(QString);
};

#endif // CHATWINDOW_H
