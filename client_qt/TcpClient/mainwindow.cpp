#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTcpSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->lineEdit,
            &QLineEdit::returnPressed,
            ui->sendButton,
            &QPushButton::click);

    socket = new QTcpSocket(this);

    connect(socket,
            &QTcpSocket::connected,
            this,
            [=]()
            {
                ui->textEdit->append(
                    "连接成功");

                name = "Tom";

                QString login =
                    "LOGIN:" + name + "\n";

                socket->write(
                    login.toUtf8());
            });

    connect(socket, &QTcpSocket::errorOccurred, this, [=](QAbstractSocket::SocketError) {
        ui->textEdit->append("连接失败: " + socket->errorString());
    });

    connect(socket,
            &QTcpSocket::readyRead,
            this,
            [=]()
            {
                QByteArray data = socket->readAll();

                ui->textEdit->append(
                    "收到：" + QString(data));
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    socket->connectToHost(
        "127.0.0.1",
        8888);

    ui->textEdit->append("正在连接服务器...");
}


void MainWindow::on_sendButton_clicked()
{
    QString msg = ui->lineEdit->text();

    QString sendMsg =
        name+":" +msg + "\n";

    if(socket->state() ==
        QAbstractSocket::ConnectedState)
    {
        socket->write(sendMsg.toUtf8());
    }
    else
    {
        ui->textEdit->append(
            "未连接服务器");
    }

    ui->textEdit->append(
        "我：" + msg);

    ui->lineEdit->clear();
}