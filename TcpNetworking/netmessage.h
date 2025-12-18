#ifndef NETMESSAGE_H
#define NETMESSAGE_H

#include "TcpNetworking_global.h"

#include <QString>

#include "nettype.h"

struct TCPNETWORKING_EXPORT NetMessage
{
public:
    static const char splitter = '|';

    // Constructor
    NetMessage();
    NetMessage(NetType msgType, QString msgBody);
    static NetMessage FromDataStream(QByteArray input);

    // Getter Setter
    int Len();
    NetType Type();
    QString Msg();
    static QString Preamble();

    // Functions
    void FillData(NetType msgType, QString msgBody);
    QByteArray ToPackage();

private:
    int len;
    NetType type;
    QString msg;
};

#endif // NETMESSAGE_H
