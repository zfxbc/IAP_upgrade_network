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

    /* 分配空间,指定父对象 */
    tcpClientSocket = new QTcpSocket(this);  /* 创建socket变量 */

    SignalSlotInit();
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

    connect(&delayTimer,&QTimer::timeout,
            [=]()
            {
                delayTimer.stop();
                sendData();
            }
            );


}

char TcpClient::FileCrcCode(char *data, int len)
{
    int i =1;

    char ret = data[0];

    for(i = 1; i < len; i++)
    {
        ret ^= data[i];
    }

    return ret;

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
    ui->FilePathLineEdit->clear();

    unsigned char sendCmdBuf[7] = LM_REQ_UPGRADE_CMD;

    if(!tcpClientSocket->isOpen())
    {
        QMessageBox::information(this,"提示","网口未打开");
        ui->chooseFileBtn->setEnabled(true);
    }

    if(sizeof(sendCmdBuf) != \
            tcpClientSocket->write((char*)sendCmdBuf,sizeof(sendCmdBuf))) {
        ui->ComRecvtextEdit->append("发送IAP指令有误");
    } else {
        ui->ComRecvtextEdit->append("升级请求命令发送完成");
    }


}
/* 启动升级 */
void TcpClient::on_startUpgardeBtn_clicked()
{
    /* 先发送文件头信息 */
    QString head = QString("%1##%2").arg(filename).arg(fileSize);
    qDebug() << head.section("##",0,0);
     qDebug() << head.section("##",1,1);
    /* 发送头部信息 */
    qint64 len = tcpClientSocket->write(head.toUtf8());

    if(len > 0)
    {
       /* 发送文件内容，防止黏包，延时20ms */
        delayTimer.start(10);
    }else
    {
        qDebug() << "头部信息发送失败";
        file.close();
        ui->chooseFileBtn->setEnabled(true);
    }
}
/* 终止升级 */
void TcpClient::on_stopUpgardeBtn_clicked()
{

}
/* 选择IAP文件 */
void TcpClient::on_chooseFileBtn_clicked()
{
    /* 1. 获取文件路径 此处为了记录上次的打开路径 */
    QSettings setting("./Setting.ini", QSettings::IniFormat);
    QString lastPath = setting.value("LastFilePath").toString();
    QString FilePath = QFileDialog::getOpenFileName(this, \
                                                    tr("Open File"), \
                                                    lastPath, \
                                                    tr("Model File(*_V*.hex *_V*.bin)"));

    /* 2. 重新组合获取到的文件路径 */
    QRegExp rx1("([^/]+)\\..+");                        /* 正则表达式 */
    rx1.indexIn(FilePath);                              /* 获取总文件路径包括hex文件名 */
    QString  HexName     = rx1.cap(1);               /* 获取hex文件名不包括后缀 */

    int first            = FilePath.lastIndexOf("/");   /* 从当前路径最后一个字符开始查找'/' */
    QString ExcepHexName = FilePath.left(first);        /* 获取除了hex文件外的路径 */

    QString ToBin        = \
            ExcepHexName + '/' + HexName + ".bin";      /* 组合bin文件路径 */


    QString EndPath = FilePath;                         /* 最终显示的文件路径 */

    if(false == EndPath.isEmpty())
    {
        filename.clear();
        fileSize = 0;

        /* 获取文件信息 */
        QFileInfo info(EndPath);
        filename = info.fileName();
        fileSize = info.size();

        /* 打印文件信息 */
        ui->ComRecvtextEdit->append(filename);
//        ui->ComRecvtextEdit->append();

        sendSize = 0;


        file.setFileName(EndPath);
        ui->ComRecvtextEdit->append(EndPath);


        bool isOk = file.open(QIODevice::ReadOnly);
        if(false == isOk)
        {
            qDebug() << "只读方式打开失败";
        }
        ui->chooseFileBtn->setEnabled(false);

    }else
    {
        ui->ComRecvtextEdit->append("选择路径出错");
    }


//  QString filePath = QFileDialog::getOpenFileName(this,"open","../");

//    if(false == FilePath.isEmpty())
//    {
//        filename.clear();
//        fileSize = 0;

//        /* 获取文件信息 */
//        QFileInfo info(FilePath);
//        filename = info.fileName();
//        fileSize = info.size();

//        sendSize = 0;


//        file.setFileName(FilePath);

//        bool isOk = file.open(QIODevice::ReadOnly);
//        if(false == isOk)
//        {
//            qDebug() << "只读方式打开失败";
//        }
//        ui->chooseFileBtn->setEnabled(false);

//    }else
//    {
//        ui->ComRecvtextEdit->append("选择路径出错");
//    }

}
/* 清除 */
void TcpClient::on_clearBtn_clicked()
{
    ui->ComRecvtextEdit->clear();
    ui->progressBar->setValue(0);
}
/* 接收数据 */
void TcpClient::getData()
{
    QByteArray ReceiveDat = tcpClientSocket->readAll();

    quint16 ilen = 0;
    unsigned char ComRecvData[256];

    if(ReceiveDat.length() >= (int)sizeof(ComRecvData))
    {
        ilen = sizeof (ReceiveDat.length());
    }else {
        ilen = ReceiveDat.length();
   }

    memset(ComRecvData,0,sizeof(ComRecvData));
    memcpy(ComRecvData,ReceiveDat,ilen);

    /* 解析数据 */
    /* 解析数据帧头 */
    if(LM_COMM_FH != ComRecvData[0])
    {
        ui->ComRecvtextEdit->append("命令帧头错误");
        ReceiveDat.clear();
        return;
    }
    /* 解析数据帧尾 */
    if(LM_COMM_EOF != ComRecvData[ilen-1])
    {
        ui->ComRecvtextEdit->append("命令帧尾错误");
        ReceiveDat.clear();
        return;
    }

    /* 计算校验和 */
    int data_sum = 0;
    data_sum = FileCrcCode((char *)&ComRecvData[1],ilen-3);
    if(LM_COMM_ACK != ComRecvData[ilen - 2]) {
        ui->ComRecvtextEdit->append("校验和错误");
        ReceiveDat.clear();
        return;
    }

    /* 提取下位机数据中的有用数据 */


}

void TcpClient::sendData()
{
    qint64 ilen = 0;
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(fileSize/1024);
    ui->progressBar->setValue(0);

    do
    {
        /* 每次发送数据的大小 */
        char buf[1024] = {0};
        ilen = 0;

        /* 往文件中读数据 */
        ilen = file.read(buf,sizeof(buf));
        /* 发送多少读多少 */
        ilen = tcpClientSocket->write(buf,ilen);

        /* 发送的数据需要积累 */
        sendSize += ilen;
         ui->progressBar->setValue(sendSize/1024);
         qDebug() << "sendSize" << sendSize;

    }while(ilen > 0);

    if(fileSize == sendSize)
    {
        ui->ComRecvtextEdit->setText("文件发送完毕");
        file.close();

    }
}
