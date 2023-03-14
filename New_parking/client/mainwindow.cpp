#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QBarCategoryAxis>
#include <QSqlDatabase>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QLayoutItem>
#include <QtGui/QIcon>
#include <QtSql>
#include <QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QsqlError>
#include <QSqlTableModel>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QVector> // QVector를 사용하기 위해 추가합니다.
#include <QtAlgorithms> // qSort 함수를 사용하기 위해 추가합니다.


using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->tableWidget->setColumnWidth(0, 305);
    ui->tableWidget->setColumnWidth(1, 85);
    ui->tableWidget->setColumnWidth(2, 85);
    ui->btn_enter->hide();
    ui->verticalLayout_4->addWidget(&view);
    ui->verticalLayout_2->addWidget(&view2);
    ui->verticalLayout_5->addWidget(&view3);
    ui->verticalLayout_6->addWidget(&view4);

    chartView = new QChartView();
    ui->verticalLayout_2p->addWidget(chartView);

    Towns[0].name = "개포";
    Towns[0].x = 4.1;
    Towns[0].y = 6.4;
    Towns[1].name = "논현";
    Towns[1].x = 2.8;
    Towns[1].y = 7.4;
    Towns[2].name = "대치";
    Towns[2].x = 4.1;
    Towns[2].y = 6.9;
    Towns[3].name = "도곡";
    Towns[3].x = 3.5;
    Towns[3].y = 6.65;
    Towns[4].name = "삼성";
    Towns[4].x = 3.8;
    Towns[4].y = 7.3;
    Towns[5].name = "세곡";
    Towns[5].x = 5.75;
    Towns[5].y = 6.08;
    Towns[6].name = "수서";
    Towns[6].x = 5.1;
    Towns[6].y = 6.55;
    Towns[7].name = "신사";
    Towns[7].x = 2.65;
    Towns[7].y = 7.60;
    Towns[8].name = "압구정";
    Towns[8].x = 2.6;
    Towns[8].y = 7.85;
    Towns[9].name = "역삼";
    Towns[9].x = 3.2;
    Towns[9].y = 6.95;
    Towns[10].name = "율현";
    Towns[10].x = 5.65;
    Towns[10].y = 6.25;
    Towns[11].name = "일원";
    Towns[11].x = 4.75;
    Towns[11].y = 6.6;
    Towns[12].name = "자곡";
    Towns[12].x = 5.1;
    Towns[12].y = 6.3;
    Towns[13].name = "청담";
    Towns[13].x = 3.5;
    Towns[13].y = 7.65;

    qDebug() << QSqlDatabase::drivers();

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("10.10.21.103");
        db.setDatabaseName("parking_station");
        db.setUserName("parking");
        db.setPassword("12345678");

        if (!db.open()) {
            qDebug() << "Database error occurred";
                      }
        else {
            qDebug() << "Database open Success";
        }


    TCPSocket = new QTcpSocket();
    //// TCPSocket->connectToHost(QHostAddress(HOST_ADDRESS),PORT_NUMBER); ////
    TCPSocket->connectToHost(QHostAddress("10.10.21.118"),7942);
    connect(TCPSocket, SIGNAL(readyRead()), this,SLOT(Read_Data_From_Socket()));
    TCPSocket->open(QIODevice::ReadWrite);
    if (TCPSocket->isOpen())
    {
        QMessageBox::information(this,"Qt with ketan","connected to the server.");
    }
    else
    {
        QMessageBox::information(this,"Qt with ketan","Not connected to the server.");
    }
    AllDataSize=0;

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::Read_Data_From_Socket()
{
    if(TCPSocket)
    {
        QByteArray received_data = TCPSocket->read(1024); // 소켓을 통해 전송된 데이터를 읽어옴
        qDebug() << "TCPSocket 읽음";
        if(AllDataSize==0)
        {
        AllDataSize = received_data.left(received_data.indexOf("|")).toInt(); // 데이터의 크기 계산
        qDebug() << "AllDataSize=0";
        }
        QByteArray json_data = received_data.right(received_data.size() - received_data.indexOf("|") - 1); // 데이터 추출
        qDebug() << json_data.size();
        qDebug() << "데이터 크기: " << AllDataSize;
        qDebug() << "반복문 돔";
        while (json_data.size() < AllDataSize) { // 데이터를 모두 읽어올 때까지 반복
            QByteArray temp_data = TCPSocket->read(1024);
            qDebug() << json_data.size();
            qDebug() << "데이터 크기: " << AllDataSize;
            qDebug() << "반복문 도는중";
            if (temp_data.isEmpty()) {
                qDebug() << "반복문 끝남";
                break;
            }
            json_data.append(temp_data);
        }
        qDebug() << json_data.size();
        qDebug() << "데이터 크기: " << AllDataSize;
        qDebug() << "반복문 끝남";
//        qDebug() << json_data;
        receivedAllData.append(json_data);
        if(receivedAllData.size() >= AllDataSize)
        {
            processReceivedData(receivedAllData); // 데이터 처리
            receivedAllData.clear();
            AllDataSize=0;
        }
    }
}

void MainWindow::processReceivedData(QByteArray receivedData)
{
    QJsonDocument doc = QJsonDocument::fromJson(receivedData);
    QJsonObject obj;
    obj = doc.object();
    QString command = obj.value("command").toString();
    QJsonArray subObject = obj.value("information").toArray();
    qDebug() << "There are " + QString::number(subObject.size()) + " items in subObject array";
    QString subList = "";
    //for(unsigned int i=0; i<subObject.size(); i++ )subList.append(subObject.at(i)["지역"].toString() + "\n");

    QString decodedData = "";
    decodedData.append(subList + "\n");


    if(command == "매물정보")
    {
        QString decodedData = obj.value("specificity").toString();
        qDebug() << decodedData << "dss";

        // 동정보 라벨 텍스트 변경
        ui->infolabel->setText(decodedData);

        // 테이블 위젯에 값 넣기
        int rowCount = subObject.size();

        ui->tableWidget->clearContents();
        ui->tableWidget->setRowCount(rowCount);

        for (int i = 0; i < rowCount; ++i)
        {
            QJsonObject item = subObject[i].toObject();

            QString address = item.value("address").toString();               // 주소
            QString area_size = item.value("area_size").toString();           // 면적
            QString actual_price = item.value("actual_price").toString();     // 매물가격

            QTableWidgetItem* itemAddress = new QTableWidgetItem(address);
            QTableWidgetItem* itemArea_size = new QTableWidgetItem(area_size);
            QTableWidgetItem* itemActual_price = new QTableWidgetItem(actual_price);

            ui->tableWidget->setItem(i, 0, itemAddress);
            ui->tableWidget->setItem(i, 1, itemArea_size);
            ui->tableWidget->setItem(i, 2, itemActual_price);
        }

        dong_location(obj);        

    }
    else if(command == "선택매물정보")
    {
        view_chart(obj);
        view_chart2(obj);
        Analysis_score(obj);
    }
    else if(command == "분석보기")
    {
        see_location(obj);
    }

    else if(command == "유동인구")
    {
        ShowScatterChart(obj);
    }
    else if(command == "평균공시지가")
    {
        ShowScatterChart2(obj);

    }
    else if(command == "주차장개수")
    {
        ShowScatterChart3(obj);
    }
}


void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    if (TCPSocket)
    {
        if (TCPSocket->isOpen())
        {
            //dong;
            QString dong = ui->comboBox->currentText();
            select_dong = QString(dong);
            qDebug() << dong;
            qDebug() << select_dong;

            QJsonObject All_message;
            QJsonObject information;
            QJsonArray info_array;
            All_message.insert("command","매물정보");
            information.insert("주소",dong);
            info_array.push_back(information);
            All_message.insert("information",info_array);
            QJsonDocument doc(All_message);
            QByteArray sendmessage = doc.toJson();
            TCPSocket->write(sendmessage);
            qDebug()<<"보낸 제이슨 데이터: "<< sendmessage;

        }
        else
        {
            QMessageBox::information(this,"Qt with ketan","Error : " + TCPSocket->errorString());
        }
    }
    else
    {
        QMessageBox::information(this,"Qt with ketan","Error : " + TCPSocket->errorString());
    }
}


void MainWindow::on_comboBox_2_currentTextChanged(const QString &arg1)
{
    if (TCPSocket)
    {
        if (TCPSocket->isOpen())
        {
            //dong;
            QString command = ui->comboBox_2->currentText();
//            qDebug() << dong;

            QJsonObject All_message;
            QJsonObject information;
            QJsonArray info_array;
            All_message.insert("command",command);
            QJsonDocument doc(All_message);
            QByteArray sendmessage = doc.toJson();
            TCPSocket->write(sendmessage);
            qDebug()<<"보낸 제이슨 데이터: "<< sendmessage;
        }
        else
        {
            QMessageBox::information(this,"Qt with ketan","Error : " + TCPSocket->errorString());
        }
    }
    else
    {
        QMessageBox::information(this,"Qt with ketan","Error : " + TCPSocket->errorString());
    }
}

void MainWindow::ShowScatterChart(QJsonObject obj) // 유동인구
{
    QJsonArray subobject = obj.value("information").toArray();

    QScatterSeries *Good = new QScatterSeries();

    Good->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    Good->setBrush(Qt::green);
    Good->setMarkerSize(40);
    QScatterSeries *Normal = new QScatterSeries();

    Normal->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    Normal->setBrush(Qt::yellow);
    Normal->setMarkerSize(30);
    QScatterSeries *Bad = new QScatterSeries();

    Bad->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    Bad->setBrush(Qt::red);
    Bad->setMarkerSize(20);

    QVector<double> town_doubles;

    for(int i=0;i<14;i++){
        Towns[i].town_string = subobject.at(0).toObject().value(Towns[i].name).toString();
        Towns[i].town_double = Towns[i].town_string.toDouble();
        town_doubles.append(Towns[i].town_double);
        qDebug() << Towns[i].town_double;
        if(Towns[i].town_double<100000){
            Bad->append(Towns[i].x,Towns[i].y);
        }
        else if(100000<=Towns[i].town_double && Towns[i].town_double<=300000){
            Normal->append(Towns[i].x,Towns[i].y);
        }
        else{
            Good->append(Towns[i].x,Towns[i].y);
        }
        qDebug()<< Towns[i].name << Towns[i].x<<Towns[i].y;
        qDebug()<< Towns[i].name;
    }
        sort(town_doubles.begin(), town_doubles.end());
        qDebug()<< town_doubles[13];
        QScatterSeries *series1 = new QScatterSeries();
        *series1 << QPointF(1.2, 6.0) << QPointF(7, 8.2);
        series1->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series1->setMarkerSize(10);


        //Create a chart object and format
        QChart *chart = new QChart();
        chart->legend()->hide();
        chart->addSeries(series1);
        chart->addSeries(Good);
        chart->addSeries(Normal);
        chart->addSeries(Bad);
//        chart->createDefaultAxes();
        // y축 범위 설정

        QValueAxis *axisY = new QValueAxis;
        axisY->setRange(6.0, 8.2);
        chart->addAxis(axisY, Qt::AlignLeft);
        series1->attachAxis(axisY);
        Good->attachAxis(axisY);
        Normal->attachAxis(axisY);
        Bad->attachAxis(axisY);
        QValueAxis *axisX = new QValueAxis;
        axisX->setRange(1.2, 7.0);
        chart->addAxis(axisX, Qt::AlignBottom);
        series1->attachAxis(axisX);
        Good->attachAxis(axisX);
        Normal->attachAxis(axisX);
        Bad->attachAxis(axisX);
        axisY -> setGridLineVisible(false);
        axisX -> setGridLineVisible(false);
       QImage skyplot("/Users/Kiot/PycharmProjects/gangnam/지도3.png");

    //Add the chart to the view widget
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing, true);
    chartView->setFixedSize(800, 600);

    //Grab the size of the plot and view areas
    int width = static_cast<int>(chart->plotArea().width());
    int height = static_cast<int>(chart->plotArea().height());
    int ViewW = static_cast<int>(chartView->width());
    int ViewH = static_cast<int>(chartView->height());

    //scale the image to fit plot area
    skyplot = skyplot.scaled(QSize(ViewW, ViewH));

    QImage translated(ViewW, ViewH, QImage::Format_ARGB32);
    translated.fill(Qt::white);
    QPainter painter(&translated);
    QPointF TopLeft = chart->plotArea().topLeft();
    painter.drawImage(TopLeft, skyplot);

    //Display image in background
    chart->setPlotAreaBackgroundBrush(translated);
    chart->setPlotAreaBackgroundVisible(true);
    for(int i=0;i<14;i++){
        if (Towns[i].town_double == town_doubles[13])
        {
            ui->label1->setText(Towns[i].name + "동" + " " +QString::number(town_doubles[13]) + "명");
        }
        else if (Towns[i].town_double == town_doubles[12])
        {
            ui->label2->setText(Towns[i].name + "동"  + " " + QString::number(town_doubles[12]) + "명");
        }
        else if (Towns[i].town_double == town_doubles[11])
        {
            ui->label3->setText(Towns[i].name + "동"  + " " + QString::number(town_doubles[11]) + "명");
        }
        else if (Towns[i].town_double == town_doubles[10])
        {
            ui->label4->setText(Towns[i].name + "동"  + " " + QString::number(town_doubles[10]) + "명");
        }
        else if (Towns[i].town_double == town_doubles[9])
        {
            ui->label5->setText(Towns[i].name + "동"  + " " + QString::number(town_doubles[9]) + "명");
        }

    chartView->show();
}
}


void MainWindow::ShowScatterChart2(QJsonObject obj) // 평균공시지가
{
    QJsonArray subobject = obj.value("information").toArray();

    QScatterSeries *Good = new QScatterSeries();

    Good->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    Good->setBrush(Qt::green);
    Good->setMarkerSize(40);
    QScatterSeries *Normal = new QScatterSeries();

    Normal->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    Normal->setBrush(Qt::yellow);
    Normal->setMarkerSize(30);
    QScatterSeries *Bad = new QScatterSeries();

    Bad->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    Bad->setBrush(Qt::red);
    Bad->setMarkerSize(20);

    QVector<double> town_doubles;

    for(int i=0;i<14;i++){
        Towns[i].town_string = subobject.at(0).toObject().value(Towns[i].name).toString();
        Towns[i].town_double = Towns[i].town_string.toDouble();
        town_doubles.append(Towns[i].town_double);
        qDebug() << Towns[i].town_double;
        if(Towns[i].town_double<500){
            Good->append(Towns[i].x,Towns[i].y);
        }
        else if(500<=Towns[i].town_double && Towns[i].town_double<=1500){
            Normal->append(Towns[i].x,Towns[i].y);
        }
        else{
            Bad->append(Towns[i].x,Towns[i].y);
        }
        qDebug()<< Towns[i].name << Towns[i].x<<Towns[i].y;
    }
        sort(town_doubles.begin(), town_doubles.end());
        qDebug()<< town_doubles[13];
        QScatterSeries *series1 = new QScatterSeries();
        *series1 << QPointF(1.2, 6.0) << QPointF(7, 8.2);
        series1->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series1->setMarkerSize(10);


        //Create a chart object and format
        QChart *chart = new QChart();
        chart->legend()->hide();
        chart->addSeries(series1);
        chart->addSeries(Good);
        chart->addSeries(Normal);
        chart->addSeries(Bad);
//        chart->createDefaultAxes();
        // y축 범위 설정

        QValueAxis *axisY = new QValueAxis;
        axisY->setRange(6.0, 8.2);
        chart->addAxis(axisY, Qt::AlignLeft);
        series1->attachAxis(axisY);
        Good->attachAxis(axisY);
        Normal->attachAxis(axisY);
        Bad->attachAxis(axisY);
        QValueAxis *axisX = new QValueAxis;
        axisX->setRange(1.2, 7.0);
        chart->addAxis(axisX, Qt::AlignBottom);
        series1->attachAxis(axisX);
        Good->attachAxis(axisX);
        Normal->attachAxis(axisX);
        Bad->attachAxis(axisX);
        axisY -> setGridLineVisible(false);
        axisX -> setGridLineVisible(false);
       QImage skyplot("/Users/Kiot/PycharmProjects/gangnam/지도3.png");

    //Add the chart to the view widget
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing, true);
    chartView->setFixedSize(800, 600);

    //Grab the size of the plot and view areas
    int width = static_cast<int>(chart->plotArea().width());
    int height = static_cast<int>(chart->plotArea().height());
    int ViewW = static_cast<int>(chartView->width());
    int ViewH = static_cast<int>(chartView->height());

    //scale the image to fit plot area
    skyplot = skyplot.scaled(QSize(ViewW, ViewH));

    QImage translated(ViewW, ViewH, QImage::Format_ARGB32);
    translated.fill(Qt::white);
    QPainter painter(&translated);
    QPointF TopLeft = chart->plotArea().topLeft();
    painter.drawImage(TopLeft, skyplot);

    //Display image in background
    chart->setPlotAreaBackgroundBrush(translated);
    chart->setPlotAreaBackgroundVisible(true);
    for(int i=0;i<14;i++){
           if (Towns[i].town_double == town_doubles[13])
           {
               ui->label1->setText(Towns[i].name + "동" + " " +QString::number(town_doubles[13]) + "만원");
           }
           else if (Towns[i].town_double == town_doubles[12])
           {
               ui->label2->setText(Towns[i].name + "동"  + " " + QString::number(town_doubles[12]) + "만원");
           }
           else if (Towns[i].town_double == town_doubles[11])
           {
               ui->label3->setText(Towns[i].name + "동"  + " " + QString::number(town_doubles[11]) + "만원");
           }
           else if (Towns[i].town_double == town_doubles[10])
           {
               ui->label4->setText(Towns[i].name + "동"  + " " + QString::number(town_doubles[10]) + "만원");
           }
           else if (Towns[i].town_double == town_doubles[9])
           {
               ui->label5->setText(Towns[i].name + "동"  + " " + QString::number(town_doubles[9]) + "만원");
           }

}

    chartView->show();
}

void MainWindow::ShowScatterChart3(QJsonObject obj) // 주차장개수
{
    QJsonArray subobject = obj.value("information").toArray();

    QScatterSeries *Good = new QScatterSeries();

    Good->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    Good->setBrush(Qt::green);
    Good->setMarkerSize(40);
    QScatterSeries *Normal = new QScatterSeries();

    Normal->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    Normal->setBrush(Qt::yellow);
    Normal->setMarkerSize(30);
    QScatterSeries *Bad = new QScatterSeries();

    Bad->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    Bad->setBrush(Qt::red);
    Bad->setMarkerSize(20);

    QVector<double> town_doubles;

    for(int i=0;i<14;i++){
        Towns[i].town_string = subobject.at(0).toObject().value(Towns[i].name).toString();
        Towns[i].town_double = Towns[i].town_string.toDouble();
        town_doubles.append(Towns[i].town_double);
        qDebug() << Towns[i].town_double;
        if(Towns[i].town_double<100){
            Good->append(Towns[i].x,Towns[i].y);
        }
        else if(100<=Towns[i].town_double && Towns[i].town_double<=200){
            Normal->append(Towns[i].x,Towns[i].y);
        }
        else{
            Bad->append(Towns[i].x,Towns[i].y);
        }
        qDebug()<< Towns[i].name << Towns[i].x<<Towns[i].y;
    }
        sort(town_doubles.begin(), town_doubles.end());
        qDebug()<< town_doubles[13];
        QScatterSeries *series1 = new QScatterSeries();
        *series1 << QPointF(1.2, 6.0) << QPointF(7, 8.2);
        series1->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        series1->setMarkerSize(10);


        //Create a chart object and format
        QChart *chart = new QChart();
        chart->legend()->hide();
        chart->addSeries(series1);
        chart->addSeries(Good);
        chart->addSeries(Normal);
        chart->addSeries(Bad);
//        chart->createDefaultAxes();
        // y축 범위 설정

        QValueAxis *axisY = new QValueAxis;
        axisY->setRange(6.0, 8.2);
        chart->addAxis(axisY, Qt::AlignLeft);
        series1->attachAxis(axisY);
        Good->attachAxis(axisY);
        Normal->attachAxis(axisY);
        Bad->attachAxis(axisY);
        QValueAxis *axisX = new QValueAxis;
        axisX->setRange(1.2, 7.0);
        chart->addAxis(axisX, Qt::AlignBottom);
        series1->attachAxis(axisX);
        Good->attachAxis(axisX);
        Normal->attachAxis(axisX);
        Bad->attachAxis(axisX);
        axisY -> setGridLineVisible(false);
        axisX -> setGridLineVisible(false);
       QImage skyplot("/Users/Kiot/PycharmProjects/gangnam/지도3.png");

    //Add the chart to the view widget
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing, true);
    chartView->setFixedSize(800, 600);

    //Grab the size of the plot and view areas
    int width = static_cast<int>(chart->plotArea().width());
    int height = static_cast<int>(chart->plotArea().height());
    int ViewW = static_cast<int>(chartView->width());
    int ViewH = static_cast<int>(chartView->height());

    //scale the image to fit plot area
    skyplot = skyplot.scaled(QSize(ViewW, ViewH));

    QImage translated(ViewW, ViewH, QImage::Format_ARGB32);
    translated.fill(Qt::white);
    QPainter painter(&translated);
    QPointF TopLeft = chart->plotArea().topLeft();
    painter.drawImage(TopLeft, skyplot);

    //Display image in background
    chart->setPlotAreaBackgroundBrush(translated);
    chart->setPlotAreaBackgroundVisible(true);
    for(int i=0;i<14;i++){
           if (Towns[i].town_double == town_doubles[13])
           {
               ui->label1->setText(Towns[i].name + "동" + " " +QString::number(town_doubles[13]) + "개");
           }
           else if (Towns[i].town_double == town_doubles[12])
           {
               ui->label2->setText(Towns[i].name + "동"  + " " + QString::number(town_doubles[12]) + "개");
           }
           else if (Towns[i].town_double == town_doubles[11])
           {
               ui->label3->setText(Towns[i].name + "동"  + " " + QString::number(town_doubles[11]) + "개");
           }
           else if (Towns[i].town_double == town_doubles[10])
           {
               ui->label4->setText(Towns[i].name + "동"  + " " + QString::number(town_doubles[10]) + "개");
           }
           else if (Towns[i].town_double == town_doubles[9])
           {
               ui->label5->setText(Towns[i].name + "동"  + " " + QString::number(town_doubles[9]) + "개");
           }

}
    chartView->show();
}

void MainWindow::on_search_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_btn_enter_clicked()
{   
    ui->stackedWidget->setCurrentIndex(3);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->label_4->clear();
    ui->label_7->clear();
    ui->label_10->clear();
    ui->label_red->setStyleSheet("border-radius: 15px; background-color:rgb(101,103,107);");
    ui->label_yellow->setStyleSheet("border-radius: 15px; background-color:rgb(101,103,107);");
    ui->label_green->setStyleSheet("border-radius: 15px; background-color:rgb(101,103,107);");

    //그래프 초기화
    for (int i = 0; i < ui->verticalLayout->count(); ++i)
    {
        QWidget *widget = ui->verticalLayout->itemAt(i)->widget();
        widget->close();
    }
    QLayoutItem *erase;
    while ((erase = ui->verticalLayout->takeAt(0)) != 0) {
        delete erase->widget();
        delete erase;
    }

    //그래프 초기화
    for (int i = 0; i < ui->verticalLayout_3->count(); ++i)
    {
        QWidget *widget = ui->verticalLayout_3->itemAt(i)->widget();
        widget->close();
    }
    QLayoutItem *erase1;
    while ((erase1 = ui->verticalLayout_3->takeAt(0)) != 0) {
        delete erase1->widget();
        delete erase1;
    }

    if (TCPSocket)
    {
        if (TCPSocket->isOpen())
        {
            QJsonObject All_message;
            QJsonObject information;
            QJsonArray info_array;
            All_message.insert("command","선택매물정보");
            information.insert("주소",row);
            info_array.push_back(information);
            All_message.insert("information",info_array);
            QJsonDocument doc(All_message);
            QByteArray sendmessage = doc.toJson();
            TCPSocket->write(sendmessage);
            qDebug()<<"보낸 제이슨 데이터: "<< sendmessage;
        }
        else
        {
            QMessageBox::information(this,"Qt with ketan","Error : " + TCPSocket->errorString());
        }
    }
    else
    {
        QMessageBox::information(this,"Qt with ketan","Error : " + TCPSocket->errorString());
    }
}


void MainWindow::on_btn_check_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(1);

    if (TCPSocket->isOpen())
    {
        QJsonObject All_message;
        QJsonObject information;
        QJsonArray info_array;
        All_message.insert("command","분석보기");
        All_message.insert("주소",row);
        info_array.push_back(information);
        All_message.insert("information",info_array);
        QJsonDocument doc(All_message);
        QByteArray sendmessage = doc.toJson();
        TCPSocket->write(sendmessage);
        qDebug()<<"보낸 제이슨 데이터: "<< sendmessage;
    }
}


void MainWindow::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    row = ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text();
    qDebug() << row;
    ui->btn_enter->show();
}

void MainWindow::see_location(QJsonObject obj)
{
    QString lat = obj.value("item_lat").toString();
    QString lon = obj.value("item_lon").toString();
    QJsonArray CCTV_information = obj.value("CCTV_information").toArray();
    QJsonArray Parkinglot_information = obj.value("Parkinglot_information").toArray();

    //CCTV 좌표 Array 만들기
    int arraySize = CCTV_information.size();
    QJsonArray CCTV_positions_array;
    for (int i = 0; i < arraySize; ++i) {
        QJsonObject item = CCTV_information[i].toObject();
        QJsonObject latlngobj;

        QString lat = item.value("lat").toString();
        QString lon = item.value("lon").toString();
        QString positionstring = QString("new kakao.maps.LatLng(%1, %2)").arg(lat).arg(lon);
        latlngobj.insert("latlng",positionstring);
        CCTV_positions_array.push_back(latlngobj);
    }

    //주차장 좌표 Array 만들기
    arraySize = Parkinglot_information.size();
    QJsonArray Parkinglot_positions_array;
    for (int i = 0; i < arraySize; ++i) {
        QJsonObject item = Parkinglot_information[i].toObject();
        QJsonObject latlngobj;

        QString lat = item.value("lat").toString();
        QString lon = item.value("lon").toString();
        QString positionstring = QString("new kakao.maps.LatLng(%1, %2)").arg(lat).arg(lon);
        latlngobj.insert("latlng",positionstring);
        Parkinglot_positions_array.push_back(latlngobj);
    }


    // CCTV html문서 생성
    QJsonDocument doc(CCTV_positions_array);
    QString positions_String = doc.toJson();
    qDebug()<<positions_String;
    positions_String.replace("\n", "");
    // 문자열 내에 있는 모든 역슬래시를 제거합니다.
    QRegularExpression slash_re("\\\\");
    positions_String = positions_String.replace(slash_re, "");
    // 문자열 내에 있는 모든 큰따옴표를 제거합니다.
    QRegularExpression quote_re("\"");
    positions_String = positions_String.replace(quote_re, "");
    QString filename = "C:/Qt/MyFile.html";
    write(filename,positions_String,lat,lon);
    view.setUrl(QUrl("http://localhost/Qt/MyFile.html"));

    // 주차장 html문서 생성
    QJsonDocument doc2(Parkinglot_positions_array);
    positions_String = doc2.toJson();
    positions_String.replace("\n", "");
    // 문자열 내에 있는 모든 역슬래시를 제거합니다.
    positions_String = positions_String.replace(slash_re, "");
    // 문자열 내에 있는 모든 큰따옴표를 제거합니다.
    positions_String = positions_String.replace(quote_re, "");
    QString filename2 = "C:/Qt/MyFile1.html";
    write(filename2,positions_String,lat,lon);
    view2.setUrl(QUrl("http://localhost/Qt/MyFile1.html"));

    // 기본 위치 문서 생성
    positions_String = "[]";
    QString filename3 = "C:/Qt/MyFile2.html";
    write(filename3,positions_String,lat,lon);
    view3.setUrl(QUrl("http://localhost/Qt/MyFile2.html"));

}

void MainWindow::dong_location(QJsonObject obj)
{
    QString lat = obj.value("lat").toString();
    QString lon = obj.value("lon").toString();
    QJsonArray information = obj.value("information").toArray();
    qDebug() << lat << lon;

    // 선택한 동의 매물 좌표 Array 만들기
    int arraySize = information.size();
    QJsonArray positions_array;
    for (int i = 0; i < arraySize; ++i) {
        QJsonObject item = information[i].toObject();
        QJsonObject latlngobj;

        QString lat = item.value("lat").toString();
        QString lon = item.value("lon").toString();
        QString positionstring = QString("new kakao.maps.LatLng(%1, %2)").arg(lat).arg(lon);
        latlngobj.insert("latlng",positionstring);
        positions_array.push_back(latlngobj);
    }

    // html 문서 생성
    QJsonDocument doc(positions_array);
    QString positions_String = doc.toJson();
    qDebug()<<positions_String;
    positions_String.replace("\n", "");
    // 문자열 내에 있는 모든 역슬래시를 제거합니다.
    QRegularExpression slash_re("\\\\");
    positions_String = positions_String.replace(slash_re, "");
    // 문자열 내에 있는 모든 큰따옴표를 제거합니다.
    QRegularExpression quote_re("\"");
    positions_String = positions_String.replace(quote_re, "");
    QString filename = "C:/Qt/MyFile3.html";
    write(filename,positions_String,lat,lon);
    view4.setUrl(QUrl("http://localhost/Qt/MyFile3.html"));
}


QString MainWindow::make_html(QString positions_string,QString lat,QString lon)
{
    QString html;
    QString html_1 =QString(
        R"(<!DOCTYPE html>
            <html>
            <head>
                <meta charset="utf-8">
                <title>여러개 마커 표시하기</title>

            </head>
            <body>
            <div id="map" style="width:100%;height:500px;"></div>

            <script type="text/javascript" src="//dapi.kakao.com/v2/maps/sdk.js?appkey=26f5edd2b1147034ef182010252c662a"></script>
            <script>
            var mapContainer = document.getElementById('map'), // 지도를 표시할 div
                mapOption = {
                    center: new kakao.maps.LatLng(%1, %2), // 지도의 중심좌표
                    level: 3 // 지도의 확대 레벨
                };)").arg(lat).arg(lon);

    // 맵을 만들어준다
    QString html_2 = "var map = new kakao.maps.Map(mapContainer, mapOption);" ;

    // 파란색 마커(마커 한개 표시)
    QString html_3 = QString(R"( // 마커가 표시될 위치입니다
            var markerPosition  = new kakao.maps.LatLng(%1, %2);
            var marker = new kakao.maps.Marker({
                position: markerPosition
            });
            marker.setMap(map);)").arg(lat).arg(lon);


    // 마커를 표시할 위치와 title 객체 배열입니다
    QString html_4 = "var positions = "; // 뒤에 제이슨어레이형식의 포지션들 추가해야함.

    // 마커 이미지의 이미지 주소입니다
    QString html_5 = R"(var imageSrc = "https://t1.daumcdn.net/localimg/localimages/07/mapapidoc/markerStar.png";

            for (var i = 0; i < positions.length; i ++) {

                // 마커 이미지의 이미지 크기 입니다
                var imageSize = new kakao.maps.Size(24, 35);

                // 마커 이미지를 생성합니다
                var markerImage = new kakao.maps.MarkerImage(imageSrc, imageSize);

                // 마커를 생성합니다
                var marker = new kakao.maps.Marker({
                    map: map, // 마커를 표시할 지도
                    position: positions[i].latlng, // 마커를 표시할 위치
                    title : positions[i].title, // 마커의 타이틀, 마커에 마우스를 올리면 타이틀이 표시됩니다
                    image : markerImage // 마커 이미지
                });
            }
            </script>
            </body>
            </html>)";

    html.append(html_1);
    html.append(html_2);
    html.append(html_3);
    html.append(html_4);
    html.append(positions_string);
    html.append(";");
    html.append(html_5);
    qDebug()<< html;
    return html;
}

void MainWindow::write(QString filename,QString positions_string,QString lat,QString lon)
{
    QFile file(filename);
    // Trying to open in WriteOnly and Text mode
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug() << " Could not open file for writing";
        return;
    }

    // To write text, we use operator<<(),
    // which is overloaded to take
    // a QTextStream on the left
    // and data types (including QString) on the right

    QTextStream out(&file);
    out << make_html(positions_string, lat, lon);
    file.flush();
    file.close();
}



void MainWindow::view_chart(QJsonObject obj)
{
    QJsonArray mainObject = obj.value("count_info").toArray();
    qDebug() << mainObject;
    int Restaurant = mainObject.at(0).toObject().value("Restaturant").toString().toInt();
    int Cafe = mainObject.at(0).toObject().value("Cafe").toString().toInt();
    int Cultural_facilities = mainObject.at(0).toObject().value("Cultural_facilities").toString().toInt();
    int Tour_place = mainObject.at(0).toObject().value("Tour_place").toString().toInt();
    int Parking_lot = mainObject.at(0).toObject().value("Parking_lot").toString().toInt();
    int Hospital = mainObject.at(0).toObject().value("Hospital").toString().toInt();
    int Convenience_store = mainObject.at(0).toObject().value("Convenience_store").toString().toInt();
    int Bank = mainObject.at(0).toObject().value("Bank").toString().toInt();

    QPieSeries* set0 = new QPieSeries();
    set0->setName("음식점");
    QPieSlice* slice0 = new QPieSlice("음식점", Restaurant);
    set0->append(slice0);

    QPieSeries* set1 = new QPieSeries();
    set1->setName("카페");
    QPieSlice* slice1 = new QPieSlice("카페", Cafe);
    set1->append(slice1);

    QPieSeries* set2 = new QPieSeries();
    set2->setName("문화시설");
    QPieSlice* slice2 = new QPieSlice("문화시설", Cultural_facilities);
    set2->append(slice2);

    QPieSeries* set3 = new QPieSeries();
    set3->setName("관광");
    QPieSlice* slice3 = new QPieSlice("관광", Tour_place);
    set3->append(slice3);

    QPieSeries* set4 = new QPieSeries();
    set4->setName("주차장");
    QPieSlice* slice4 = new QPieSlice("주차장", Parking_lot);
    set4->append(slice4);

    QPieSeries* set5 = new QPieSeries();
    set5->setName("병원");
    QPieSlice* slice5 = new QPieSlice("병원", Hospital);
    set5->append(slice5);

    QPieSeries* set6 = new QPieSeries();
    set6->setName("편의점");
    QPieSlice* slice6 = new QPieSlice("편의점", Convenience_store);
    set6->append(slice6);

    QPieSeries* set7 = new QPieSeries();
    set7->setName("은행");
    QPieSlice* slice7 = new QPieSlice("은행", Bank);
    set7->append(slice7);

    QWidget *chartContainer = new QWidget(ui->centralwidget);
    QVBoxLayout *layout = new QVBoxLayout(chartContainer);

    QPieSeries *series = new QPieSeries;
    series->append("음식점", Restaurant);
    series->append("카페", Cafe);
    series->append("문화시설", Cultural_facilities);
    series->append("관광", Tour_place);
    series->append("주차장", Parking_lot);
    series->append("병원", Hospital);
    series->append("편의점", Convenience_store);
    series->append("은행", Bank);

//    series->setLabelsVisible();
//    series->setLabelsPosition(QPieSlice::LabelOutside);
//    for(auto slice : series->slices())
//        slice->setLabel(QString("%1\n %2").arg(slice->label()).arg(slice->value()));

    QChart *chart = new QChart();
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->addSeries(series);
    chart->setTitle("주변 상점");
    QChartView *chartView = new QChartView(chart);
    layout->addWidget(chartView);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->verticalLayout->addWidget(chartView);

}

void MainWindow::view_chart2(QJsonObject obj)
{
    QJsonArray subobject = obj.value("information").toArray();
    QString data = subobject.at(0).toObject().value("월수익").toString();
    double profit = data.toDouble();
    QString data2 = subobject.at(0).toObject().value("대출이자").toString();
    double interest = data2.toDouble();
    QString data3 = subobject.at(0).toObject().value("원리금").toString();
    double principal = data3.toDouble();

    QPieSeries* set0 = new QPieSeries();
    set0->setName("월수익");
    QPieSlice* slice0 = new QPieSlice("월수익", profit);
    set0->append(slice0);

    QPieSeries* set1 = new QPieSeries();
    set1->setName("대출이자");
    QPieSlice* slice1 = new QPieSlice("대출이자", interest);
    set1->append(slice1);

    QPieSeries* set2 = new QPieSeries();
    set2->setName("원리금");
    QPieSlice* slice2 = new QPieSlice("원리금", principal);
    set2->append(slice2);

    QWidget *chartContainer = new QWidget(ui->centralwidget);
    QVBoxLayout *layout = new QVBoxLayout(chartContainer);

    QPieSeries *series = new QPieSeries;
    series->append("월수익", profit);
    series->append("대출이자", interest);
    series->append("원리금", principal);

    series->setLabelsVisible();
    series->setLabelsPosition(QPieSlice::LabelOutside);
    for(auto slice : series->slices())
        slice->setLabel(QString("%1\n %2%").arg(slice->label()).arg(100*slice->percentage(), 0, 'f', 1));

    QChart *chart = new QChart();
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->addSeries(series);
    chart->legend()->setVisible(false);
    QChartView *chartView = new QChartView(chart);
    layout->addWidget(chartView);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->verticalLayout_3->addWidget(chartView);

}

void MainWindow::Analysis_score(QJsonObject obj)
{
    QJsonArray subobject = obj.value("information").toArray();
    qDebug() << subobject;
    double score1 = subobject.at(0).toObject().value("상권점수").toString().toDouble();
    QString score2 = subobject.at(0).toObject().value("주차대수").toString();
    QString score3 = subobject.at(0).toObject().value("월수익").toString();
    QString score4 = subobject.at(0).toObject().value("수익률").toString();
    double rate = subobject.at(0).toObject().value("수익률").toString().toDouble();
    qDebug() << score1 << score2 << score3 << score4 << "42424242";

    ui->lcdNumber->display(score1);
    ui->label_4->setText(score2);   //주차대수
    ui->label_7->setText(score3);  //월수익
    ui->label_10->setText(score4); // 수익률

    if(-1 < rate && rate < 3)
    {
        ui->label_red->setStyleSheet("border-radius: 15px; background-color:red;");
    }
    else if(3 < rate && rate < 5)
    {
        ui->label_yellow->setStyleSheet("border-radius: 15px; background-color:yellow;");
    }
    else if(5 < rate)
    {
        ui->label_green->setStyleSheet("border-radius: 15px; background-color:green;");
    }

}

void MainWindow::on_btn_back_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_start_btn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_btn_back_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_btn_back_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

