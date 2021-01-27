#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDebug>
#include <QTimer>
#include <QtNetwork/QTcpSocket>
#include <QDialog>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QDialog *addChannel;
    QVBoxLayout *channelDialogLayout;
    QLineEdit *channelName;
    QPushButton *channelNameButton;

public slots:
    void connectServer();
    void showError(QAbstractSocket::SocketError error);
    void sendCommand();
    void sendMessage();
    void readyRead();
    void setNick();
    void usersList();
    void channelsList();
    void onConnection();
    void joinChannel(QListWidgetItem *item);
    void showChannel();
    void createChannel();
};
#endif // MAINWINDOW_H
