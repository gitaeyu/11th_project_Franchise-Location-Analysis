#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QByteArray>
#include <QString>

#include <iostream>
#include <cstdlib>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDebug>

// json변환
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void newConnection();
    void Read_Data_From_Socket();
    void land_sale(QTcpSocket *socket,QString selected_address);
    void land_price(QTcpSocket *socket);
    void parking_station(QTcpSocket *socket);
    void floting_population(QTcpSocket *socket);
    void send_json_object(QJsonDocument data,QTcpSocket *socket);
    void search_score (QTcpSocket *socket,QString selected_address);
    void sendlocation(QTcpSocket *socket,QString selected_address);


private:
    void Add_New_Client_Connection(QTcpSocket *socket);
private:
    QSqlDatabase db;
    Ui::MainWindow *ui;
    QTcpServer *TCP_Server;
    QList<QTcpSocket*> Client_Connection_List;
    QString selected_address;
};
#endif
