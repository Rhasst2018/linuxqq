#include "mytoolbutton.h"


MyToolButton::MyToolButton(QString fid, QString nickname, QWidget *parent) :
    QToolButton(parent)
{
    M_fid = fid;
    M_nickname = nickname;
}

QString MyToolButton::getFid()
{
    return M_fid;
}

void MyToolButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit MyToolButtonClicked(M_fid, M_nickname);
    }
    QToolButton::mousePressEvent(event);
}
