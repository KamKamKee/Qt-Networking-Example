#include "netsocket.h"
#include <QApplicationStatic>

NetSocket::NetSocket() : QObject(nullptr) {
    tcpSocket = new QTcpSocket();
    Setup();
}

NetSocket::NetSocket(QTcpSocket *rawTcpSocket) : QObject(nullptr)
{
    tcpSocket = rawTcpSocket;
    Setup();
}

NetSocket::~NetSocket() {
    tcpSocket->deleteLater();
}

QTcpSocket *NetSocket::TcpSocket()
{
    return tcpSocket;
}

const QString NetSocket::LastErrorString()
{
    return tcpSocket->errorString();
}

bool NetSocket::ConnectToServer(QString hostAddress, int port)
{
    qInfo() << "Connecting to server...";

    // In case a loading screen should popup before the connection establishment (which could take a while...)
    QCoreApplication::processEvents();

    // Attempt connection begin
    if (hostAddress == "localhost")
    {
        tcpSocket->connectToHost(QHostAddress::LocalHost, port);
    }

    else
    {
        tcpSocket->connectToHost(hostAddress, port);
    }

    // Wait for response
    if (tcpSocket->waitForConnected(5000))
    {
        qInfo() << "Connected Successfully";
        return true;
    }

    else
    {
        qWarning() << "Connect Failed";
        return false;
    }
}

void NetSocket::DisconnectFromServer()
{
    qInfo() << "Disconnecting from Server...";

    // In case a loading screen should popup before the connection establishment (which could take a while...)
    QCoreApplication::processEvents();

    tcpSocket->disconnectFromHost();
}

bool NetSocket::IsConnected()
{
    return isConnected;
}

void NetSocket::readReady()
{
    NetMessage message = NetMessage::FromDataStream(tcpSocket->readAll());

    qInfo() << "\n<< Received message of purpose " << message.Type() << " with body: " << message.Msg();

    emit onMessageReceived(message);
}

void NetSocket::SendMessage(NetType messageType, QString messageBody)
{
    NetMessage message(messageType, messageBody);
    SendMessage(message);
}

void NetSocket::SendMessage(NetMessage message)
{
    if(tcpSocket)
    {
        if(tcpSocket->isOpen())
        {
            QDataStream socketStream(tcpSocket);
            socketStream.setVersion(QDataStream::Qt_6_9);

            QByteArray packagedMessage = message.ToPackage();
            socketStream << packagedMessage;

            qInfo() << "Sending message of purpose " << message.Type() << " with body: " << message.Msg();
            qDebug() << packagedMessage;
        }
        else
            qCritical() << "Socket doesn't seem to be opened";
    }
    else
        qCritical() << "Not connected";
}

void NetSocket::Setup()
{
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readReady()));
    connect(tcpSocket, &QTcpSocket::connected, this, [this](){
        isConnected = true;
        emit onConnected();
    });
    connect(tcpSocket, &QTcpSocket::disconnected, this, [this](){
        isConnected = false;
        emit onDisconnected();
    });
    connect(tcpSocket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error){emit onError(error);});
}
