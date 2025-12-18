#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>

#include "clientuser.h"
#include "netserver.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    NetServer server;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QList<ClientUser*> clientUsers;
    QStringListModel *messageModel;
    QStringList messageList;

    ClientUser* GetUserFromSocket(QTcpSocket* socket);
    QString GetUserIdentification(ClientUser* user);
    void AddMessageToView(const QString &message);

    void UpdateClientComboBox();

private slots:
    // Server Socket
    void onMessageReceived(QTcpSocket* sender, NetMessage message);
    void onClientConnected(QTcpSocket* socket);
    void onClientDisconnected(QTcpSocket* socket);
    void onError(QTcpSocket* sender, QAbstractSocket::SocketError error);

    // UI
    void openServer();
    void clientComboBoxChanged();
    void messageEditChanged(const QString &text);
    void sendMessage();
    void clearMessageView();

};
#endif // MAINWINDOW_H
