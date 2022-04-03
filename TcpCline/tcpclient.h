#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>
#include <QRegExp>
#include <QTimer>

#define MAINWINDOW_H_SIZE   600
#define MAINWINDOW_W_SIZE   800

#define LM_COMM_FH                 0x7F         /*上下位机通信命令帧头*/
#define LM_COMM_EOF                0xF7         /*上下位机通信命令帧尾*/
#define LM_COMM_SIZE               0x04         /*上下位机通信命令长度*/
#define LM_COMM_ACK                0x00         /*上下位机通信命令ACK标志*/
#define LM_REQ_UPGRADE_CMD          {0x7F,0x04,0xAA,0x03,0x01,0xB2,0xF7} /* 请求升级命令 */
#define LM_FINISH_UPGRADE_CMD       {0x7F,0x04,0xAA,0x06,0x01,0xB5,0xF7} /* 升级终止命令 */

namespace Ui {
class TcpClient;
}

class TcpClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

    void mainwindowInit();
    void SignalSlotInit();
    char FileCrcCode(char *data, int len);   /* 计算校验和 */


private:
    Ui::TcpClient *ui;

    QTcpSocket *tcpClientSocket;
    QString getLocalIp;     /* 获取本机IP */
    bool connectFlag;
    QFile file;
    QString filename;
    quint64 fileSize;
    quint64 sendSize;

    char *ver;
    QString bin_Ver;

    QTimer delayTimer;



private slots:
    void on_connectedBtn_clicked();
    void on_reqUpgardeBtn_clicked();
    void on_startUpgardeBtn_clicked();
    void on_stopUpgardeBtn_clicked();
    void on_chooseFileBtn_clicked();
    void on_clearBtn_clicked();

    void getData();
    void sendData();

};

#endif // TCPCLIENT_H
