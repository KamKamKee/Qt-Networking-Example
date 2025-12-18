#include "clientuser.h"



ClientUser::ClientUser(QTcpSocket *inputSocket)
{
    socket = inputSocket;
    id = inputSocket->socketDescriptor();
}

QTcpSocket *ClientUser::Socket()
{
    return socket;
}

int ClientUser::ID()
{
    return id;
}
