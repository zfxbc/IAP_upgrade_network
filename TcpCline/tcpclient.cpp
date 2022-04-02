#include "tcpclient.h"
#include "ui_tcpclient.h"

TcpClient::TcpClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);

    /* 初始化 */
    mainwindowInit();

    /*  固定界面大小 */
    this->setMinimumSize(MAINWINDOW_W_SIZE,MAINWINDOW_H_SIZE);
    this->setMaximumSize(MAINWINDOW_W_SIZE,MAINWINDOW_H_SIZE);

    /* 初始化套接字 */
    tcpClientSocket = NULL;

    /* 分配空间,指定父对象  */
    tcpClientSocket = new QTcpSocket(this);  /* 创建socket变量 */
}

TcpClient::~TcpClient()
{
    delete ui;
}
/* 初始化 */
void TcpClient::mainwindowInit()
{
     connectFlag = false;   /* 设置连接时的状态 */
     ui->progressBar->setValue(0);
     isStart = true;


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

}
/* 信号槽 */
void TcpClient::SignalSlotInit()
{
    connect(tcpClientSocket,&QTcpSocket::connected,
            [=]()
            {
                ui->ComRecvtextEdit->setText("成功连接服务器");
            }
            );

    connect(tcpClientSocket,&QTcpSocket::readyRead,
            [=]()
            {
                getData();
            }
            );

}
/* 连接 */
void TcpClient::on_connectedBtn_clicked()
{
    if(connectFlag == false)
    {
        QString addrIp = ui->ServerIpEdit->text();
        quint16 port = ui->portEdit->text().toInt();
        tcpClientSocket->connectToHost(addrIp,port);
        ui->connectedBtn->setText("断开连接");
        ui->ComRecvtextEdit->setText("成功连接服务器");
    }else{
        if(tcpClientSocket->state() == QAbstractSocket::ConnectedState)
        {
            tcpClientSocket->disconnectFromHost();
            ui->connectedBtn->setText("请连接");
            ui->ComRecvtextEdit->setText("服务器连接失败");
        }
    }
}
/* 请求升级 */
void TcpClient::on_reqUpgardeBtn_clicked()
{

}
/* 启动升级 */
void TcpClient::on_startUpgardeBtn_clicked()
{

}
/* 终止升级 */
void TcpClient::on_stopUpgardeBtn_clicked()
{

}
/* 选择文件 */
void TcpClient::on_chooseFileBtn_clicked()
{

}
/* 清除 */
void TcpClient::on_clearBtn_clicked()
{
    ui->ComRecvtextEdit->clear();
}

void TcpClient::getData()
{
    QByteArray comRecvData = tcpClientSocket->readAll();

    if(isStart == true)
    {
        isStart = false;
        /* 解析头部文件信息，初始化 */
        filename = QString(comRecvData).section("##",0,0);
        fileSize = QString(comRecvData).section("##",1,1).toInt();
        sendSize = 0;

        /* 打开文件 */
        file.setFileName("../"+filename);
        bool isOk = file.open(QIODevice::WriteOnly|QIODevice::Append);
        if(isOk == false)
        {
            //                tcpSocket->disconnectFromHost();
            //                tcpSocket->close();
            QMessageBox::information(this,"提示：","打开文件错误");
            //                return ;
        }

        /* 进度条初始化 */
        ui->progressBar->setMinimum(0);
        ui->progressBar->setMaximum(fileSize/1024);
    }else
    {
        quint64 len = file.write(comRecvData);
        if(len > 0)
        {
            sendSize +=len;
        }

        /* 更新进度条 */
        ui->progressBar->setValue(sendSize/1024);
        if(sendSize == fileSize)
        {
            //传输文件完毕
            //                file.close();
            QMessageBox::information(this,"提示：","文件接收完毕");
            //                tcpSocket->disconnectFromHost();
            //                tcpSocket->close();
        }
    }
}
