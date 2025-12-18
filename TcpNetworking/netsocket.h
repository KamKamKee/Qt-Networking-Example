#ifndef NETSOCKET_H
#define NETSOCKET_H

#include "TcpNetworking_global.h"

#include <QObject>
#include <QTcpSocket>

#include "netmessage.h"


class TCPNETWORKING_EXPORT NetSocket : public QObject
{
    Q_OBJECT
public:
    // Constructor
    NetSocket();
    NetSocket(QTcpSocket* rawTcpSocket);
    ~NetSocket();

    // Getter Setter
    QTcpSocket* TcpSocket();
    const QString LastErrorString();
    bool IsConnected();

    // Functions
    bool ConnectToServer(QString hostAddress, int port);
    void DisconnectFromServer();
    void SendMessage(NetType messageType, QString messageBody);
    void SendMessage(NetMessage message);

private:
    QTcpSocket *tcpSocket;
    bool isConnected = false;
    void Setup();

private slots:
    void readReady();

signals:
    void onMessageReceived(NetMessage);
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError);
};

#endif // NETSOCKET_H
