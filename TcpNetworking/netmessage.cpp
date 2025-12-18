#include "netmessage.h"
#include <QDebug>

NetMessage::NetMessage() {}

NetMessage::NetMessage(NetType msgType, QString msgBody)
{
    FillData(msgType, msgBody);
}

NetMessage NetMessage::FromDataStream(QByteArray input)
{
    QStringList dataInput = QString(input).split(Preamble() + splitter);
    QString packageData;

    // Check for packages in data stream by preamble
    for (const QString &data : std::as_const(dataInput))
    {
        if (data.startsWith("L:"))
        {
            packageData = data;
            break;
        }
    }

    if (packageData.isNull())
        throw "Package data could not be read - no message-preamble found";

    // Split package and interpret data
    dataInput = packageData.split("|");

    int msgLen = dataInput[0].remove("L:").toInt();
    dataInput.removeFirst();

    NetType msgType = NetType(dataInput[0].remove("T:").toInt());
    dataInput.removeFirst();

    QString msgBody = dataInput.join(splitter);
    msgBody = msgBody.mid(0, msgLen);

    // Create and return instance
    return NetMessage(msgType, msgBody);
}

int NetMessage::Len() {
    return len;
}

NetType NetMessage::Type()
{
    return type;
}

QString NetMessage::Msg()
{
    return msg;
}

QString NetMessage::Preamble()
{
    return "<" + QString(char(1)) + QString(char(1)) + "<";
}

void NetMessage::FillData(NetType msgType, QString msgBody)
{
    type = msgType;
    msg = msgBody;
    len = msgBody.length();
}

QByteArray NetMessage::ToPackage()
{
    QString package = "";

    package.append(Preamble() + splitter);
    package.append("L:" + QString::number(len) + splitter);
    package.append("T:" + QString::number(type) + splitter);
    package.append(msg);

    return package.toUtf8();
}
