#ifndef ADDFRIENDWIDGET_H
#define ADDFRIENDWIDGET_H

#define DEBUG

#include <QWidget>
#include <QMessageBox>
#include <QtNetwork>

#ifdef DEBUG
#include <QDebug>
#endif

namespace Ui {
class AddFriendWidget;
}

class AddFriendWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AddFriendWidget(QString uid, QWidget *parent = 0);
    ~AddFriendWidget();

private:
    Ui::AddFriendWidget *ui;
    QString M_uid;
    QTcpSocket *M_TcpSocket;

private:
    void SendAddFriendMessage();
    void ConnnectToServer();

private slots:
    void SubmitBtnClicked();
    void ReadMessage();

signals:
    void addedFriendsSignal();
};

#endif // ADDFRIENDWIDGET_H
