#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Message View
    messageModel = new QStringListModel(this);
    messageModel->setStringList(messageList);
    ui->messageView->setModel(messageModel);

    // Socket connections
    connect(&socket, SIGNAL(onMessageReceived(NetMessage)), this, SLOT(onMessageReceived(NetMessage)));
    connect(&socket, SIGNAL(onConnected()), this, SLOT(onConnected()));
    connect(&socket, SIGNAL(onDisconnected()), this, SLOT(onDisconnected()));
    connect(&socket, SIGNAL(onError(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    // UI Connections
    connect(ui->connectButton, SIGNAL(clicked(bool)), this, SLOT(connectToServer()));
    connect(ui->messageEdit, SIGNAL(textChanged(QString)), this, SLOT(messageEditChanged(QString)));
    connect(ui->sendButton, SIGNAL(clicked(bool)), this, SLOT(sendMessage()));
    connect(ui->clearMessageViewButton, SIGNAL(clicked(bool)), this, SLOT(clearMessageView()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::AddMessageToView(const QString &message)
{
    messageList.append(message);
    messageModel->setStringList(messageList);
    ui->messageView->scrollToBottom();
}

void MainWindow::connectToServer()
{
    if (!socket.IsConnected())
    {
        ui->connectedLabel->setText("Connecting...");
        bool success = socket.ConnectToServer(ui->addressEdit->text(), ui->portEdit->text().toInt());

        if (!success)
        {
            onDisconnected();
        }

        else
        {
            socket.SendMessage(NetType::RegisterClient, ui->usernameEdit->text());
        }
    }

    else
    {
        ui->connectedLabel->setText("Disconnecting...");
        socket.DisconnectFromServer();
    }
}

void MainWindow::messageEditChanged(const QString &text)
{
    ui->sendButton->setEnabled(text.length() != 0);
}

void MainWindow::sendMessage()
{
    socket.SendMessage(NetType::ChatMessage, ui->messageEdit->text());
    ui->messageEdit->setText("");
}

void MainWindow::clearMessageView()
{
    messageList.clear();
    messageModel->setStringList(messageList);
}

void MainWindow::onMessageReceived(NetMessage message)
{
    if (message.Type() == NetType::ChatMessage)
    {
        AddMessageToView(message.Msg());
    }
}

void MainWindow::onConnected()
{
    AddMessageToView("Connected");
    ui->connectedLabel->setText("Connected");
    ui->connectButton->setText("Disconnect");
    ui->addressEdit->setEnabled(false);
    ui->portEdit->setEnabled(false);
    ui->usernameEdit->setEnabled(false);
    ui->messageEdit->setEnabled(true);
}

void MainWindow::onDisconnected()
{
    AddMessageToView("Disconnected");
    ui->connectedLabel->setText("Disconnected");
    ui->connectButton->setText("Connect");
    ui->addressEdit->setEnabled(true);
    ui->portEdit->setEnabled(true);
    ui->usernameEdit->setEnabled(true);
    ui->messageEdit->setEnabled(false);
}

void MainWindow::onError(QAbstractSocket::SocketError error)
{
    AddMessageToView("Error Occured: " + socket.LastErrorString());
}
