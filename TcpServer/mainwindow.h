#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>


#define LM_COMM_FH                 0x7F         /*上下位机通信命令帧头*/
#define LM_COMM_EOF                0xF7         /*上下位机通信命令帧尾*/
#define LM_COMM_SIZE               0x04         /*上下位机通信命令长度*/
#define LM_COMM_ACK                0x00         /*上下位机通信命令ACK标志*/



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void mainwindowInit();
    void signalSlotInit();

    char IapFileCrcCode(char *data, int len);
private:
    Ui::MainWindow *ui;
    QTcpServer *tcpserver;
    QTcpSocket *tcpsocket;

    QFile file;
    QString filename;
    quint64 fileSize;
    quint64 getSize;



    bool isStart;
    QTimer delayTimer;


public slots:
    void getData();
};

#endif // MAINWINDOW_H
