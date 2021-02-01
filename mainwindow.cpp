#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);

    QTimer *timerUsers, *timerChannels;
    timerUsers = new QTimer(this);
    timerChannels = new QTimer(this);

    connect(ui->ConnectButton, SIGNAL(clicked()),
            this, SLOT(connectServer()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(showError(QAbstractSocket::SocketError)));
    connect(ui->SendCommand, SIGNAL(clicked()),
            this, SLOT(sendCommand()));
    connect(ui->SendMessage, SIGNAL(clicked()),
            this, SLOT(sendMessage()));
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(readyRead()));
    connect(ui->setNickname, SIGNAL(clicked()),
            this, SLOT(setNick()));
    connect(ui->createChannelButton, SIGNAL(clicked()),
            this, SLOT(showChannel()));
    connect(socket, SIGNAL(connected()),
            this, SLOT(onConnection()));
    connect(ui->joinChannelButton, SIGNAL(clicked()),
            this, SLOT(joinChannel()));
    connect(ui->channelsList, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
            this, SLOT(joinChannel(QListWidgetItem *)));
    connect(timerUsers, SIGNAL(timeout()),
            this, SLOT(usersList()));
    connect(timerChannels, SIGNAL(timeout()),
            this, SLOT(channelsList()));
    connect(ui->DeleteChannel, SIGNAL(clicked()),
            this, SLOT(deleteChannel()));
    connect(ui->LeaveChannel, SIGNAL(clicked()),
            this, SLOT(leaveChannel()));

//    timerUsers->start(9000);
//    timerChannels->start(9500);

    /**
     * Obsługa dialogu
     */
    addChannel = new QDialog(this);
    channelDialogLayout = new QVBoxLayout(addChannel);
    channelName = new QLineEdit(addChannel);
    channelNameButton = new QPushButton("Create channel", addChannel);
    channelDialogLayout->addWidget(channelName);
    channelDialogLayout->addWidget(channelNameButton);
    addChannel->setLayout(channelDialogLayout);

    connect(channelNameButton, SIGNAL(clicked()),
            this, SLOT(createChannel()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectServer()
{
    ui->plainTextEdit->appendHtml(
                QString().append("<b>Connecting to ")
                .append(ui->ServerInput->text())
                .append(":")
                .append(ui->PortInput->text())
                .append("... </b>"));
    socket->connectToHost(ui->ServerInput->text(), ui->PortInput->text().toInt());
}

void MainWindow::showError(QAbstractSocket::SocketError error)
{
    ui->plainTextEdit->appendHtml(
                QString().append("<b style='color: red'>")
                .append(socket->errorString())
                .append("</b>"));
}

void MainWindow::sendCommand()
{
    socket->write(ui->CommandInput->text().toLocal8Bit());
    ui->CommandInput->clear();
}

void MainWindow::sendMessage()
{
    for(int i=0;i<ui->channelsElements->count();i++){
        ui->channelsElements->itemAt(i);
        if(static_cast<QRadioButton*>(ui->channelsElements->itemAt(i)->widget())->isChecked()){
            socket->write(
                        QString().append("SENDTOCHANNEL $")
                        .append(static_cast<QRadioButton*>(ui->channelsElements->itemAt(i)->widget())->text())
                        .append("$ ")
                        .append(ui->MessageInput->text()).toLocal8Bit()
                        .append("$")
                        );
            ui->MessageInput->clear();
        }
    }
}



void MainWindow::readyRead()
{
    QString response = socket->readAll();
    if(response.indexOf("USERS")==0){
        response.replace(0,6,"");
        QStringList users = response.split("\n");
        ui->usersList->clear();
        for(int i=0;i<users.size();i++) {
            ui->usersList->addItem(users.at(i));
        }
        return;
    } else if(response.indexOf("CHANNELS")==0){
        response.replace(0,9,"");
        QStringList channels = response.split("\n");
        ui->channelsList->clear();
        for(int i=0; i<channels.size(); i++){
            ui->channelsList->addItem(channels.at(i));
        }
        return;
    } else if(response.indexOf("JOINCHANNEL")==0){
        response.replace(0,12,"");
        QRadioButton *rb = new QRadioButton(response, this);
        ui->channelsElements->addWidget(rb);
        return;
    } else if(response.indexOf("CHANNELCREATED")==0){
        QRadioButton *rb = new QRadioButton(getFirstAttribute(response), this);
        ui->otherChannelsElements->addWidget(rb);
        return;
    }
    ui->plainTextEdit->appendPlainText(response);
}

void MainWindow::setNick()
{
    socket->write(
                QString().append("SETNICKNAME $").append(ui->Nickname->text()).append("$").toLocal8Bit()
                );
}

void MainWindow::usersList()
{
    socket->write(
                QString().append("LISTUSERS").toLocal8Bit()
                );
}

void MainWindow::channelsList()
{
    socket->write(
                QString().append("LISTCHANNELS").toLocal8Bit()
                );
}

void MainWindow::onConnection()
{
    ui->plainTextEdit->appendPlainText("done\n");
}

void MainWindow::joinChannel(QListWidgetItem *item)
{
    socket->write(
                QString().append("JOINCHANNEL $").append(item->text()).append("$").toLocal8Bit()
                );
}

void MainWindow::showChannel()
{
    addChannel->show();
}

void MainWindow::createChannel()
{
    socket->write(
                QString().append("CREATECHANNEL $").append(channelName->text()).append("$").toLocal8Bit()
                );
    addChannel->hide();
}

void MainWindow::deleteChannel()
{
    for(int i=0;i<ui->channelsElements->count();i++){
        ui->channelsElements->itemAt(i);
        if(static_cast<QRadioButton*>(ui->channelsElements->itemAt(i)->widget())->isChecked()){
            socket->write(
                        QString().append("REMOVECHANNEL $")
                        .append(static_cast<QRadioButton*>(ui->channelsElements->itemAt(i)->widget())->text())
                        .append("$ ").toLocal8Bit()
                        );
            ui->channelsElements->takeAt(i);
        }
    }
}

void MainWindow::leaveChannel()
{
    for(int i=0;i<ui->channelsElements->count();i++){
        ui->channelsElements->itemAt(i);
        if(static_cast<QRadioButton*>(ui->channelsElements->itemAt(i)->widget())->isChecked()){
            socket->write(
                        QString().append("LEAVECHANNEL $")
                        .append(static_cast<QRadioButton*>(ui->channelsElements->itemAt(i)->widget())->text())
                        .append("$ ").toLocal8Bit()
                        );
            ui->channelsElements->takeAt(i);
        }
    }
}

void MainWindow::joinChannel()
{
    for(int i=0; i < ui->otherChannelsElements->count(); i++){
        if(static_cast<QRadioButton*>(ui->otherChannelsElements->itemAt(i)->widget())->isChecked()){
            socket->write(
                        QString().append("JOINCHANNEL $")
                        .append(static_cast<QRadioButton*>(ui->otherChannelsElements->itemAt(i)->widget())->text())
                        .append("$")
                        .toLocal8Bit()
                        );
            ui->otherChannelsElements->takeAt(i);
            return;
        }
    }
}

QString MainWindow::getFirstAttribute(QString response)
{
    QStringList list = response.split("$");
    return list.at(1);

}
