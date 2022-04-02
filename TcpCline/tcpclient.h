#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QFile>
#include <QMessageBox>

#define MAINWINDOW_H_SIZE   600
#define MAINWINDOW_W_SIZE   800

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


private:
    Ui::TcpClient *ui;

    QTcpSocket *tcpClientSocket;
    QString getLocalIp;     /* 获取本机IP */
    bool connectFlag;
    QFile file;
    QString filename;
    quint64 fileSize;
    quint64 sendSize;

    bool isStart;



private slots:
    void on_connectedBtn_clicked();
    void on_reqUpgardeBtn_clicked();
    void on_startUpgardeBtn_clicked();
    void on_stopUpgardeBtn_clicked();
    void on_chooseFileBtn_clicked();
    void on_clearBtn_clicked();

    void getData();

};

#endif // TCPCLIENT_H
