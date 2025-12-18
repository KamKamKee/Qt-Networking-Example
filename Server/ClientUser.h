#ifndef CLIENTUSER_H
#define CLIENTUSER_H

#include <QTcpSocket>

struct ClientUser
{
public:
    QString username;

    ClientUser(QTcpSocket* inputSocket);
    QTcpSocket* Socket();
    int ID();

private:
    QTcpSocket *socket;
    int id;
};

#endif // CLIENTUSER_H
