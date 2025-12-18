#include "netserver.h"

NetServer::NetServer() : QObject(nullptr) {
    tcpServer = new QTcpServer();

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

NetServer::~NetServer()
{
    tcpServer->close();
    tcpServer->deleteLater();

    for (QTcpSocket* socket : std::as_const(clients))
    {
        socket->close();
        socket->deleteLater();
    }

    clients.clear();
}

QTcpServer *NetServer::TcpServer()
{
    return tcpServer;
}

const QList<QTcpSocket *> NetServer::Clients()
{
    return std::as_const(clients);
}

bool NetServer::IsOpened()
{
    return isOpened;
}

bool NetServer::OpenServer(int port)
{
    if(tcpServer->listen(QHostAddress::Any, port))
    {
        connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
        qInfo() << "Server started and is listening...";
        isOpened = true;
        return true;
    }
    else
    {
        qCritical() << "Server failed to start: " << tcpServer->errorString();
        return false;
    }
}

void NetServer::CloseServer()
{
    for (QTcpSocket* socket : std::as_const(clients))
    {
        socket->close();
        socket->deleteLater();
    }

    clients.clear();
    tcpServer->close();
    isOpened = false;
}



void NetServer::SendMessage(int receiverDescriptor, NetType messageType, QString messageBody)
{
    SendMessage(receiverDescriptor, NetMessage(messageType, messageBody));
}

void NetServer::SendMessage(int receiverDescriptor, NetMessage message)
{
    for (QTcpSocket* client : std::as_const(clients))
    {
        if (client->socketDescriptor() != receiverDescriptor)
            continue;

        SendMessage(client, message);
    }
}

void NetServer::SendMessage(QTcpSocket *receiver, NetType messageType, QString messageBody)
{
    SendMessage(receiver, NetMessage(messageType, messageBody));
}

void NetServer::SendMessage(QTcpSocket *receiver, NetMessage message)
{
    if(receiver)
    {
        if(receiver->isOpen())
        {
            QDataStream socketStream(receiver);
            socketStream.setVersion(QDataStream::Qt_6_9);

            socketStream << message.ToPackage();

            qInfo() << "Sending " << receiver->socketDescriptor() << " message of purpose " << message.Type() << " with body: " << message.Msg();

        }
        else
            qCritical() << "Socket " << receiver->socketDescriptor() << " doesn't seem to be opened";
    }
    else
        qCritical() << "Socket " << receiver->socketDescriptor() << " is not connected";
}

void NetServer::BroadcastMessage(NetType messageType, QString messageBody)
{
    BroadcastMessage(NetMessage(messageType, messageBody));
}

void NetServer::BroadcastMessage(NetMessage message)
{
    for (QTcpSocket* client : std::as_const(clients))
    {
        SendMessage(client, message);
    }
}

void NetServer::newConnection()
{
    while (tcpServer->hasPendingConnections())
    {
        QTcpSocket *newSocket = tcpServer->nextPendingConnection();
        clients.append(newSocket);

        connect(newSocket, &QTcpSocket::readyRead, this, [newSocket, this]() {readyRead(newSocket);});
        connect(newSocket, &QTcpSocket::disconnected, this, [newSocket, this]() {disconnected(newSocket);});
        connect(newSocket, &QTcpSocket::errorOccurred, this, [newSocket, this](QAbstractSocket::SocketError error) {emit onError(newSocket, error);});

        emit onClientConnected(newSocket);
    }
}

void NetServer::disconnected(QTcpSocket* senderSocket)
{
    clients.removeOne(senderSocket);

    senderSocket->close();
    senderSocket->deleteLater();

    emit onClientDisconnected(senderSocket);
}

void NetServer::readyRead(QTcpSocket* senderSocket)
{
    NetMessage message = NetMessage::FromDataStream(senderSocket->readAll());

    emit onMessageReceived(senderSocket, message);
}
