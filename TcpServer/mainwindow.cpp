#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* 使用qss设置界面样式*/
   this->setStyleSheet("QLineEdit,QPushButton,QComboBox,QProgressBar:on{"
                       "border: 2px groove gray;"
                       "border-radius: 9px;"
                       "padding: 3px 4px;"
                       "border-color: rgb(12,45,68);}"

                       "QGroupBox{"
                       "border: 2px groove gray;"
                       "border-radius: 9px;"
                       "padding: 3px 4px;"
                       "border-color:rgb(12,152,102);}"

                       "QPushButton:hover{"
                       "background-color: rgb(255, 160, 6);"
                       "color:black;}"

                       "QLineEdit,QPushButton,QComboBox,QProgressBar:off{"
                       "border: 2px groove gray;"
                       "border-radius: 9px;"
                      " padding: 3px 4px;"
                       "background-color: rgb(223, 223, 223);}"

                       "QProgressBar:{border: 2px groove gray;"
                       "border-radius: 9px;"
                       "padding: 3px 4px;"
                       "border-color: rgb(12,45,68);}"
                       );

    this->setWindowTitle("服务器：222");

    tcpserver = new QTcpServer(this);
    tcpserver->listen(QHostAddress::Any,222);

    connect(tcpserver,&QTcpServer::newConnection,
            [=]()
            {
                tcpsocket = tcpserver->nextPendingConnection();

                QString ip = tcpsocket->peerAddress().toString();
                quint16 port = tcpsocket->peerPort();

                QString temp = QString("[%1:%2]:连接成功").arg(ip).arg(port);
                ui->textEdit->setText(temp);

                connect(tcpsocket,&QTcpSocket::readyRead,
                        [=]()
                        {
                            QByteArray array = tcpsocket->readAll();
                            ui->textEdit->setText(array);
                        }
                        );

            }
            );
}
MainWindow::~MainWindow()
{
    delete ui;
}
