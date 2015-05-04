#ifndef SQLCONNECTION_H
#define SQLCONNECTION_H

#include <QSqlDatabase>
#include <QSqlQuery>

#define DEBUG

#ifdef DEBUG
#include <QDebug>
#endif

static bool SqlConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("123");
    db.setDatabaseName("qq");

#ifdef DEBUG
    if(!db.open())
    {
        qDebug() <<"open database failed";
        return false;
    }
#endif

    else
    {
        return true;
    }

}

#endif // SQLCONNECTION_H
