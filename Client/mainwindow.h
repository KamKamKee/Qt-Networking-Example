#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>

#include "netsocket.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    NetSocket socket;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStringListModel *messageModel;
    QStringList messageList;

    void AddMessageToView(const QString &message);

private slots:
    // Socket
    void onMessageReceived(NetMessage message);
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);

    // UI
    void connectToServer();
    void messageEditChanged(const QString &text);
    void sendMessage();
    void clearMessageView();
};
#endif // MAINWINDOW_H
