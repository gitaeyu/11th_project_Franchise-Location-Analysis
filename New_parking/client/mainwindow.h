#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>
#include <QDataStream>
#include <QString>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QsqlError>
#include <QDebug>
#include <QSqlTableModel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <stdio.h>
#include <iostream>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QValueAxis>
#include <QLayoutItem>
#include <QtWebEngineWidgets>
#include <QVector>
#include <QtAlgorithms>
#include <string>
#include <string.h>


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
    void Read_Data_From_Socket();
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_btn_enter_clicked();
    void on_btn_check_clicked();
    void processReceivedData(QByteArray receivedData);
    void on_tableWidget_itemClicked(QTableWidgetItem *item);
    void on_btn_back_clicked();
    void on_comboBox_2_currentTextChanged(const QString &arg1);
    void on_search_btn_clicked();

    void on_start_btn_clicked();

    void on_btn_back_2_clicked();

    void on_btn_back_3_clicked();

private :
    void view_chart(QJsonObject obj);
    void view_chart2(QJsonObject obj);
    void Analysis_score(QJsonObject obj);
    void see_location(QJsonObject obj);
    void dong_location(QJsonObject obj);
    QString make_html(QString positions_string,QString lat,QString lon);
    void write(QString filename,QString positions_string,QString lat,QString lon);
    void ShowScatterChart(QJsonObject obj);
    void ShowScatterChart2(QJsonObject obj);
    void ShowScatterChart3(QJsonObject obj);

private:
    Ui::MainWindow *ui;
    QTcpSocket *TCPSocket;
    QByteArray receivedAllData; // 수신한 데이터를 저장할 변수
    int AllDataSize;
    QString select_dong;
    QString row;
    QWebEngineView view;
    QWebEngineView view2;
    QWebEngineView view3;
    QWebEngineView view4;
    QChartView* chartView;
    // 구조체의 선언
    struct Town {
        QString name;
        QString town_string; //IsolationControl.isol_btn 이런식으로 사용함
        double town_double;
        double x;
        double y;
    };
    //구조체 14개를 담기 위한 배열 선언
    Town Towns[14];

};
#endif // MAINWINDOW_H

