#ifndef NETSERVER_H
#define NETSERVER_H

#include "TcpNetworking_global.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "netmessage.h"


class TCPNETWORKING_EXPORT NetServer : public QObject
{
    Q_OBJECT
public:
    // Constructor
    NetServer();
    ~NetServer();

    // Getter Setter
    QTcpServer* TcpServer();
    const QList<QTcpSocket*> Clients();
    bool IsOpened();

    // Functions
    bool OpenServer(int port);
    void CloseServer();
    void SendMessage(int receiverDescriptor, NetType messageType, QString messageBody);
    void SendMessage(int receiverDescriptor, NetMessage message);
    void SendMessage(QTcpSocket* receiver, NetType messageType, QString messageBody);
    void SendMessage(QTcpSocket* receiver, NetMessage message);
    void BroadcastMessage(NetType messageType, QString messageBody);
    void BroadcastMessage(NetMessage message);

private:
    QTcpServer *tcpServer;
    QList<QTcpSocket*> clients;
    bool isOpened;

private slots:
    void newConnection();
    void disconnected(QTcpSocket* senderSocket);
    void readyRead(QTcpSocket* senderSocket);

signals:
    void onClientConnected(QTcpSocket*);
    void onClientDisconnected(QTcpSocket*);
    void onMessageReceived(QTcpSocket*, NetMessage);
    void onError(QTcpSocket*, QAbstractSocket::SocketError);
};

#endif // NETSERVER_H
