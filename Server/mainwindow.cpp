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

    // Server Socket Connections
    connect(&server, SIGNAL(onMessageReceived(QTcpSocket*,NetMessage)), this, SLOT(onMessageReceived(QTcpSocket*,NetMessage)));
    connect(&server, SIGNAL(onClientConnected(QTcpSocket*)), this, SLOT(onClientConnected(QTcpSocket*)));
    connect(&server, SIGNAL(onClientDisconnected(QTcpSocket*)), this, SLOT(onClientDisconnected(QTcpSocket*)));
    connect(&server, SIGNAL(onError(QTcpSocket*,QAbstractSocket::SocketError)), this, SLOT(onError(QTcpSocket*,QAbstractSocket::SocketError)));

    // UI Connections
    connect(ui->openServerButton, SIGNAL(clicked(bool)), this, SLOT(openServer()));
    connect(ui->clientComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(clientComboBoxChanged()));
    connect(ui->messageEdit, SIGNAL(textChanged(QString)), this, SLOT(messageEditChanged(QString)));
    connect(ui->sendButton, SIGNAL(clicked(bool)), this, SLOT(sendMessage()));
    connect(ui->clearMessageViewButton, SIGNAL(clicked(bool)), this, SLOT(clearMessageView()));

    UpdateClientComboBox();
}

MainWindow::~MainWindow()
{
    for (ClientUser *user : std::as_const(clientUsers))
    {
        delete user;
    }
    clientUsers.clear();

    delete ui;
}

ClientUser *MainWindow::GetUserFromSocket(QTcpSocket* socket)
{
    for (ClientUser *user : std::as_const(clientUsers))
    {
        if (user->Socket() == socket)
        {
            return user;
        }
    }

    return nullptr;
}

QString MainWindow::GetUserIdentification(ClientUser *user)
{
    if (user->username.isEmpty())
    {
        return QString::number(user->ID());
    }

    else
    {
        return user->username + " [" + QString::number(user->ID()) + "]";
    }
}

void MainWindow::AddMessageToView(const QString &message)
{
    messageList.append(message);
    messageModel->setStringList(messageList);
    ui->messageView->scrollToBottom();
}

void MainWindow::clearMessageView()
{
    messageList.clear();
    messageModel->setStringList(messageList);
}

void MainWindow::UpdateClientComboBox()
{
    ui->clientComboBox->clear();
    ui->clientComboBox->addItem("Everyone");

    for (ClientUser *user : std::as_const(clientUsers))
    {
        ui->clientComboBox->addItem(GetUserIdentification(user));
    }

    ui->clientComboBox->setCurrentIndex(0);
}

void MainWindow::onMessageReceived(QTcpSocket *sender, NetMessage message)
{
    qInfo() << "Received: " << message.ToPackage();
    ClientUser *user = GetUserFromSocket(sender);

    // Client is not registered
    if (user == nullptr)
    {
        if (message.Type() == NetType::RegisterClient)
        {
            ClientUser *user = new ClientUser(sender);
            user->username = message.Msg();

            clientUsers.append(user);

            AddMessageToView(">> " + GetUserIdentification(user) + " joined");
            server.BroadcastMessage(NetType::ChatMessage, ">> " + GetUserIdentification(user) + " joined");

            UpdateClientComboBox();
        }

        return;
    }

    if (message.Type() == NetType::ChatMessage)
    {
        QString userIdentification = GetUserIdentification(user);;
        AddMessageToView(userIdentification + ": " + message.Msg());
        server.BroadcastMessage(NetType::ChatMessage, userIdentification + ": " + message.Msg());
    }
}

void MainWindow::onClientConnected(QTcpSocket *socket)
{

}

void MainWindow::onClientDisconnected(QTcpSocket *socket)
{
    ClientUser *disconnectedUser = GetUserFromSocket(socket);

    // Client wasn't registered
    if (disconnectedUser == nullptr)
    {
        return;
    }

    AddMessageToView(">> " + GetUserIdentification(disconnectedUser) + " left");
    server.BroadcastMessage(NetType::ChatMessage, ">> " + GetUserIdentification(disconnectedUser) + " left");

    for (int i = 0; i < clientUsers.count(); i++)
    {
        if (disconnectedUser == clientUsers[i])
        {
            delete clientUsers[i];
            clientUsers.removeAt(i);
        }
    }
}

void MainWindow::onError(QTcpSocket *sender, QAbstractSocket::SocketError error)
{
    //AddMessageToView("Error Occurred: " + sender->errorString());
}

void MainWindow::openServer()
{
    if (!server.IsOpened())
    {
        server.OpenServer(ui->portEdit->text().toInt());
        ui->openedLabel->setText("Opened");
        ui->openServerButton->setText("Close Server");
        ui->portEdit->setEnabled(false);

        ui->messageEdit->setEnabled(true);
        ui->clientComboBox->setEnabled(true);
    }

    else
    {
        server.CloseServer();
        ui->openedLabel->setText("Closed");
        ui->openServerButton->setText("Open Server");
        ui->portEdit->setEnabled(true);

        ui->messageEdit->setEnabled(false);
        ui->clientComboBox->setEnabled(false);
    }
}

void MainWindow::clientComboBoxChanged()
{
    ui->messageEdit->setPlaceholderText("Message to " + ui->clientComboBox->currentText());
}


void MainWindow::messageEditChanged(const QString &text)
{
    ui->sendButton->setEnabled(text.length() != 0);
}


void MainWindow::sendMessage()
{
    QString messageContent;

    if (ui->clientComboBox->currentIndex() == 0)
    {
        messageContent = "[Server]: " + ui->messageEdit->text();
        server.BroadcastMessage(NetType::ChatMessage, messageContent);
    }

    else if (ui->clientComboBox->currentIndex() != -1)
    {
        ClientUser *user = clientUsers[ui->clientComboBox->currentIndex() - 1];

        messageContent = "[Server->" + GetUserIdentification(user) + "]: " + ui->messageEdit->text();
        server.SendMessage(user->Socket(), NetType::ChatMessage, messageContent);
    }

    AddMessageToView(messageContent);
    ui->messageEdit->clear();
}

