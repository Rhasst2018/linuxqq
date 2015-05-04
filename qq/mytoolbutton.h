#ifndef MYTOOLBOXBUTTON_H
#define MYTOOLBOXBUTTON_H

#include <QToolButton>
#include <QMouseEvent>

class MyToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit MyToolButton(QString fid, QString nickname, QWidget *parent = 0);

protected:
    virtual void mousePressEvent(QMouseEvent *event);

private:
    QString M_fid;
    QString M_nickname;

signals:
    void MyToolButtonClicked(QString, QString);

public:
    QString getFid();

};

#endif // MYTOOLBOXBUTTON_H
