#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("服务器：222");

    mainwindowInit();


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
                            getData();
                        }
                        );
            }
            );

}
void MainWindow::mainwindowInit()
{
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

    ui->progressBar->setValue(0);
    isStart = true;
}

void MainWindow::signalSlotInit()
{

}
char MainWindow::IapFileCrcCode(char *data, int len)
{
    int  i = 1;

    char ret = data[0];

    for (i = 1; i < len; i++) {
        ret ^= data[i];
    }

    return ret;
}
void MainWindow::getData()
{
    QByteArray ReceiveDat = tcpsocket->readAll();
    qDebug() <<" comRecvData "<< ReceiveDat;


    if(isStart == true)
    {
        isStart = false;
        /* 解析头部文件信息，初始化 */
        filename = QString(ReceiveDat).section("##",0,0);
        qDebug() << filename <<93;
        fileSize = QString(ReceiveDat).section("##",1,1).toInt();
        qDebug() << fileSize <<93;
        getSize = 0;

        /* 打开文件 */
        file.setFileName("../"+filename);
        bool isOk = file.open(QIODevice::WriteOnly);
        if(isOk == false)
        {
            //                tcpSocket->disconnectFromHost();
            //                tcpSocket->close();
            QMessageBox::information(this,"提示：","打开文件错误");
            //                return ;
        }

        QString str = QString("接收文件：[%1:%2kb]").arg(filename).arg(fileSize);
        ui->textEdit->setText(str);
       // QMessageBox::information(this,"文件信息",str);
        /* 进度条初始化 */
        ui->progressBar->setMinimum(0);
        ui->progressBar->setMaximum(fileSize/1024);
        ui->progressBar->setValue(0);
    }else
    {
        quint64 len = file.write(ReceiveDat);
        if(len > 0)
        {
            getSize +=len;
        }

        /* 更新进度条 */
        ui->progressBar->setValue(getSize/1024);
        if(getSize == fileSize)
        {
            //传输文件完毕
            //                file.close();
            QMessageBox::information(this,"提示：","文件接收完毕");
            file.close();
            //                tcpSocket->disconnectFromHost();
            //                tcpSocket->close();
        }
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

