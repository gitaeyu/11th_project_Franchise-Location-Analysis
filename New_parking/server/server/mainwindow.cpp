#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    // QTcpServer 객체 생성
    TCP_Server = new QTcpServer();
    // QTcpSever객체를 모든 ip주소에서 7942 포트로 listen
    if (TCP_Server->listen(QHostAddress::Any,7942))
    {
        qDebug()<<"연결성공";
        // QTcpServer에서 클라이언트가 연결될때마다 newConnection()함수호출
        connect(TCP_Server,SIGNAL(newConnection()),this,SLOT(newConnection()));
        QDateTime currentDateTime = QDateTime::currentDateTime();
        qDebug()<<"server open:"+currentDateTime.toString("yyyy-mm-dd")+" "+currentDateTime.toString("hh:mm:ss");
    }
    else
    {
        qDebug() << "연결실패";
    }
    // 데이터베이스 연결
    db = QSqlDatabase::addDatabase("QMYSQL");

}

MainWindow::~MainWindow()
{
    delete ui;
}

// 새로운 클라이언트 연결
void MainWindow::newConnection()
{
    while(TCP_Server->hasPendingConnections())
    {
        Add_New_Client_Connection(TCP_Server->nextPendingConnection());
    }
}

// 클라이언트가 보내준 데이터 가져오기
void MainWindow::Read_Data_From_Socket()
{
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(sender());
        QByteArray Message_From_Client = socket->readAll();

        QJsonDocument doc = QJsonDocument::fromJson(Message_From_Client);
        qDebug() << doc;
        QJsonObject obj;
        obj = doc.object();
        QString command = obj.value("command").toString();
        QJsonArray subObject = obj.value("information").toArray();
        qDebug() << "명령" << command;
        qDebug() << subObject;
        qDebug() << "There are " + QString::number(subObject.size()) + " items in subObject array";

    if (command == "유동인구")
    {
        qDebug()<<"유동인구 보내주기";
        floting_population(socket);
    }
    else if (command == "평균공시지가")
    {
        qDebug()<<"공시지가 보내주기";
        land_price(socket);
    }
    else if (command == "주차장개수")
    {
        qDebug()<<"전체주차장 보내주기";
        parking_station(socket);
    }
    else if (command == "매물정보")
    {
        qDebug()<<"동매물 보내주기";
        const QJsonObject jsonObj = subObject.at(0).toObject();
        QString select_by_dong = jsonObj.value("주소").toString();
        land_sale(socket,select_by_dong);

    }
    else if (command == "선택매물정보")
    {
        qDebug()<<"매물점수정보 보내주기";
        const QJsonObject jsonObj = subObject.at(0).toObject();
        QString selected_address = jsonObj.value("주소").toString();
        search_score(socket,selected_address);
    }
    else if (command == "분석보기")
    {
        QString address = obj.value("주소").toString() ;
        qDebug()<<"선택매물정보 보내주기";
        sendlocation(socket,address);
    }
}

void MainWindow::Add_New_Client_Connection(QTcpSocket *socket)
{
    Client_Connection_List.append(socket);
    connect(socket,SIGNAL(readyRead()),this,SLOT(Read_Data_From_Socket()));
    ui->comboBox_Client_List->addItem(QString::number(socket->socketDescriptor()));
    QString Client = "Client:" + QString::number(socket->socketDescriptor())+"Connected with the server.";
    ui->textEdit_Client_Messages->append(Client);
}

// 유동인구
void MainWindow::floting_population(QTcpSocket *socket)
{
    db.setHostName("10.10.21.103");
    db.setDatabaseName("parking_station");
    db.setUserName("parking");
    db.setPassword("12345678");
    QJsonArray information;
    QJsonObject object;

    if (db.open())
    {
        QSqlQuery qry;
        qry.prepare("SELECT * FROM parking_station.floating_population where sort = '전체 유동인구'");
        if(qry.exec())
        {
            qDebug() << "Database open Success (연결성공)";
            while(qry.next())
            {
                object.insert("압구정",qry.value(1).toString());
                object.insert("신사",qry.value(2).toString());
                object.insert("청담",qry.value(3).toString());
                object.insert("논현",qry.value(4).toString());
                object.insert("삼성",qry.value(5).toString());
                object.insert("역삼",qry.value(6).toString());
                object.insert("도곡",qry.value(7).toString());
                object.insert("대치",qry.value(8).toString());
                object.insert("개포",qry.value(9).toString());
                object.insert("일원",qry.value(10).toString());
                object.insert("수서",qry.value(11).toString());
                object.insert("세곡",qry.value(12).toString());
                object.insert("자곡",qry.value(13).toString());
                object.insert("율현",qry.value(14).toString());
                information.push_back(object);
            }
        }
        else
        {
            qDebug() << "Database error occurred (연결실패)";
        }
        db.close();
    }
    QJsonObject mainObject;
    mainObject.insert("information", information);
    mainObject.insert("command", "유동인구");
    QJsonDocument personDocument(mainObject);
    QString personJson = personDocument.toJson();
    qDebug()<<personJson;
    send_json_object(personDocument,socket);
}

// 공시지가
void MainWindow::land_price(QTcpSocket *socket)
{
    db.setHostName("10.10.21.103");
    db.setDatabaseName("parking_station");
    db.setUserName("parking");
    db.setPassword("12345678");
    QJsonArray information;
    QJsonObject object;

    if (db.open())
    {
        QSqlQuery qry;
        QString qurly =("CALL call_avg_by_dong()");
        if(qry.exec(qurly))
        {
            qDebug() << "Database open Success (연결성공)";
            while(qry.next())
            {
                object.insert("청담",qry.value(0).toString());
                object.insert("대치",qry.value(1).toString());
                object.insert("도곡",qry.value(2).toString());
                object.insert("개포",qry.value(3).toString());
                object.insert("일원",qry.value(4).toString());
                object.insert("자곡",qry.value(5).toString());
                object.insert("논현",qry.value(6).toString());
                object.insert("삼성",qry.value(7).toString());
                object.insert("세곡",qry.value(8).toString());
                object.insert("신사",qry.value(9).toString());
                object.insert("역삼",qry.value(10).toString());
                object.insert("율현",qry.value(11).toString());
                object.insert("압구정",qry.value(12).toString());
                object.insert("수서","104");
                information.push_back(object);
            }
        }
        else
        {
            qDebug() << "Database error occurred (연결실패)";
        }
        db.close();
    }
    QJsonObject mainObject;
    mainObject.insert("information", information);
    mainObject.insert("command", "평균공시지가");
    QJsonDocument personDocument(mainObject);
    QString personJson = personDocument.toJson();
    qDebug()<<personJson;
    send_json_object(personDocument,socket);
}


// 주차장 전체개수
void MainWindow::parking_station(QTcpSocket *socket)
{
    db.setHostName("10.10.21.103");
    db.setDatabaseName("parking_station");
    db.setUserName("parking");
    db.setPassword("12345678");
    QJsonArray information;
    QJsonObject object;

    if (db.open())
    {
        QSqlQuery qry;
        QString qurly =("CALL call_pariking_lot_num()");
        if(qry.exec(qurly))
        {
            qDebug() << "Database open Success (연결성공)";
            while(qry.next())
            {
                object.insert("개포",qry.value(0).toString());
                object.insert("논현",qry.value(1).toString());
                object.insert("대치",qry.value(2).toString());
                object.insert("도곡",qry.value(3).toString());
                object.insert("삼성",qry.value(4).toString());
                object.insert("세곡",qry.value(5).toString());
                object.insert("수서",qry.value(6).toString());
                object.insert("신사",qry.value(7).toString());
                object.insert("압구정",qry.value(8).toString());
                object.insert("역삼",qry.value(9).toString());
                object.insert("율현",qry.value(10).toString());
                object.insert("일원",qry.value(11).toString());
                object.insert("자곡",qry.value(12).toString());
                object.insert("청담",qry.value(13).toString());
                information.push_back(object);
            }
        }
        else
        {
            qDebug() << "Database error occurred (연결실패)";
        }
        db.close();
    }
    QJsonObject mainObject;
    mainObject.insert("information", information);
    mainObject.insert("command", "주차장개수");
    QJsonDocument personDocument(mainObject);
    QString personJson = personDocument.toJson();
    qDebug()<<personJson;
    send_json_object(personDocument,socket);
}

// 선택된 동의 매물정보 보내주기
void MainWindow::land_sale(QTcpSocket *socket,QString selected_address)
{
    db.setHostName("10.10.21.103");
    db.setDatabaseName("parking_station");
    db.setUserName("parking");
    db.setPassword("12345678");
    QJsonObject mainObject;
    QJsonArray information;
    QJsonObject object;


    if (db.open())
    {
        QSqlQuery qry;
        QString qurly =QString("SELECT a.address_name,\
                               a.actual_price,b.area_size,c.specificity,lat,lon\
                               from parking_station.land_for_sale_final AS a\
                               inner join parking_station.land_price_result AS b\
                               inner Join parking_station.information AS c\
                               On a.address_name = b.address_name and a.administrative_dong = c.town where a.address_name like '%%1%'").arg(selected_address);
        qry.prepare(qurly);
        if(qry.exec())
        {
            qDebug() << "Database open Success (연결성공)";
            while(qry.next())
            {
                object.insert("address",qry.value(0).toString());
                object.insert("area_size",qry.value(2).toString());
                object.insert("actual_price",qry.value(1).toString());
                object.insert("lat",qry.value(4).toString());
                object.insert("lon",qry.value(5).toString());
                information.push_back(object);
                mainObject.insert("specificity",qry.value(3).toString());
                mainObject.insert("lat",qry.value(4).toString());
                mainObject.insert("lon",qry.value(5).toString());
            }

        }
        else
        {
            qDebug() << "Database error occurred (연결실패)";
        }
        db.close();
    }

    mainObject.insert("information", information);
    mainObject.insert("command","매물정보");
    QJsonDocument personDocument(mainObject);
    QString personJson = personDocument.toJson();
    qDebug()<<personJson;
    send_json_object(personDocument,socket);
}



// 선택매물에 대한 점수와 수익률
void MainWindow::search_score(QTcpSocket *socket, QString selected_address)
{
    db.setHostName("10.10.21.103");
    db.setDatabaseName("parking_station");
    db.setUserName("parking");
    db.setPassword("12345678");
    QJsonArray information;
    QJsonArray count_info;
    QJsonObject object;
    qDebug()<<"주소확인하기!!!"<<selected_address;
    if (db.open())
    {
        QSqlQuery qry;
        QString qurly = QString("select b.*,((Restaurant*0.4+Cafe*0.3+Cultural_facilities*0.2+Tour_place*0.1)-(Parking_lot*0.4 + Hospital*0.3 +  Convenience_store *0.2 + Bank * 0.1))as 상권점수\
                                from count_check_sale as a left outer join land_price_result as b on a.address_name = b.address_name where a.address_name = '%1'").arg(selected_address);
            qry.prepare(qurly);
            if(qry.exec())
            {
                qDebug() << "Database open Success (연결성공)";
                qry.next();
                    qDebug()<<"반복문돌았냐";
                    bool ok;
                    double Utilization_rate;
                    if (qry.value(7).toInt(&ok) < 50 && ok)
                    {
                       Utilization_rate = 0.5;
                    }
                    else if (qry.value(7).toInt(&ok) < 100 && ok)
                    {
                       Utilization_rate = 0.6;
                    }
                    else if (qry.value(7).toInt(&ok) < 180 && ok)
                    {
                       Utilization_rate = 0.7;
                    }
                    else if (qry.value(7).toInt(&ok) < 280 && ok)
                    {
                       Utilization_rate = 0.8;
                    }

                    QString administrative_dong = qry.value(1).toString();
                    QString qurly2 = QString("select avg(default_price) from park_price where default_time = 30 and administrative_dong = '%1'").arg(administrative_dong);
                    qry.exec(qurly2);
                    qry.next();
                    int price = qry.value(0).toInt(&ok);
                    qDebug()<<"금액확인"<<price;
                    QSqlQuery qry2;
                    qurly2 =QString("select ((Restaurant*0.4+Cafe*0.3+Cultural_facilities*0.2+Tour_place*0.1)-(Parking_lot*0.4 + Hospital*0.3 +  Convenience_store *0.2 + Bank * 0.1))as 상권점수,\
                                    round((actual_price * 100000000)) as 땅값,\
                                    round((((actual_price * 100000000)/2)*0.07)/12) as 대출이자,\
                                    round(((actual_price * 100000000)/2)/60) as 원리금,\
                                    round(area_size/12.5)*5 as 주차대수,\
                                    round(area_size/12.5)*5*%3*30*48*%1 as 수익,\
                                    round(area_size/12.5)*5*%3*30*48*%1 - ((actual_price * 100000000)*0.5*0.0225)  as 월수익,\
                                    round(((round(area_size/12.5)*5*%3*30*48*%1)- ((actual_price * 100000000)*0.5*0.0225))/((actual_price * 100000000)*0.5)*100,2) as 월수익률\
                                    from count_check_sale as a  join land_price_result as b  join land_for_sale_final as c on a.address_name = b.address_name and a.address_name = c.address_name where a.address_name = '%2'").arg(Utilization_rate).arg(selected_address).arg(price);
                    qry2.prepare(qurly2);
                    if (qry2.exec())
                    {
                        qDebug()<<"점수나와라";
                        qDebug() << "Database open Success (연결성공)";
                        while(qry2.next())
                        {
                            object.insert("상권점수",qry2.value(0).toString());
                            object.insert("대출이자",qry2.value(2).toString());
                            object.insert("원리금",qry2.value(3).toString());
                            object.insert("주차대수",qry2.value(4).toString());
                            object.insert("월수익",qry2.value(6).toString());
                            object.insert("수익률",qry2.value(7).toString());
                            qDebug()<<qry2.value(5).toDouble()<<qry2.value(4).toDouble();
                            information.push_back(object);
                        }
                    }
                    QSqlQuery qry3;
                    QString query_str = QString("SELECT a.address_name, a.Mart,\
                                                a.Convenience_store, a.Day_care_center,\
                                                a.School, a.Academy,\
                                                a.Parking_lot, a.Gas_station,\
                                                a.Subway, a.Bank, a.Cultural_facilities,\
                                                a.Broker, a.public_institution, a.Tour_place,\
                                                a.Accommodation, a.Restaurant, a.Cafe,\
                                                a.Hospital, a.CCTV,\
                                                b.actual_price,\
                                                c.official_land_price\
                                                from parking_station.count_check_sale AS a\
                                                inner Join parking_station.land_for_sale_final AS b\
                                                inner Join parking_station.land_price_result AS c\
                                                On a.address_name = b.address_name and a.address_name = c.address_name where a.address_name like '%%1%'")
                                                .arg(selected_address);
                    qDebug() << selected_address;
                    qry3.prepare(query_str);
                    if(qry3.exec())
                    {
                        qDebug() << "Database open Success (연결성공)";
                        while(qry3.next())
                        {
                            object.insert("name",qry3.value(0).toString());
                            object.insert("mart",qry3.value(1).toString());
                            object.insert("Convenience_store",qry3.value(2).toString());
                            object.insert("Day_care_center",qry3.value(3).toString());
                            object.insert("School",qry3.value(4).toString());
                            object.insert("Academy",qry3.value(5).toString());
                            object.insert("Parking_lot",qry3.value(6).toString());
                            object.insert("Gas_station",qry3.value(7).toString());
                            object.insert("Bank",qry3.value(8).toString());
                            object.insert("Cultural_facilities",qry3.value(9).toString());
                            object.insert("Broker",qry3.value(10).toString());
                            object.insert("public_institution",qry3.value(11).toString());
                            object.insert("Tour_place",qry3.value(12).toString());
                            object.insert("Accommodation",qry3.value(13).toString());
                            object.insert("Restaurant",qry3.value(14).toString());
                            object.insert("Cafe",qry3.value(15).toString());
                            object.insert("Hospital",qry3.value(16).toString());
                            object.insert("CCTV",qry3.value(17).toString());
                            object.insert("official_land_price",qry3.value(18).toString());
                            object.insert("area_size",qry3.value(19).toString());
                            count_info.push_back(object);
                        }
                    }
                    else
                    {
                        qDebug() << "Database error occurred (연결실패)";
                    }
                }

            else
            {
                qDebug() << "Database error occurred (연결실패)";
            }
        db.close();
    }
    QJsonObject mainObject;
    mainObject.insert("information", information);
    mainObject.insert("count_info", count_info);
    mainObject.insert("command","선택매물정보");
    QJsonDocument personDocument(mainObject);
    QString personJson = personDocument.toJson();
    qDebug()<<personJson;
    send_json_object(personDocument,socket);
}

// 분석보기
void MainWindow::sendlocation(QTcpSocket *socket,QString selected_address)
{
    db.setHostName("10.10.21.103");
    db.setDatabaseName("parking_station");
    db.setUserName("parking");
    db.setPassword("12345678");
    QJsonObject mainObject;
    QJsonArray CCTV_information;
    QJsonArray Parkinglot_information;
    QJsonObject object;

    if (db.open())
    {
        QSqlQuery qry;
        QString query2 = QString("SELECT *FROM (\
                                 SELECT ( 6371 * acos( cos( radians( b.lat ) ) * cos( radians(a.lat) ) * cos( radians( a.lon ) - radians(b.lon) ) + \
                                 sin( radians(b.lat) ) * sin( radians(a.lat) ) ) ) AS distance,a.lat,a.lon\
                                 FROM cctv as a join land_for_sale_final as b where b.address_name = '%1') as DATA\
                                 WHERE DATA.distance < 0.5").arg(selected_address);
        qry.prepare(query2);
        if(qry.exec())
        {
            qDebug() << "Database open Success (연결성공)";
            while(qry.next())
            {
                object.insert("lat",qry.value(1).toString());
                object.insert("lon",qry.value(2).toString());
                CCTV_information.push_back(object);
            }
        }
        query2 = QString("SELECT *FROM (\
                                 SELECT ( 6371 * acos( cos( radians( b.lat ) ) * cos( radians(a.lat) ) * cos( radians( a.lon ) - radians(b.lon) ) + \
                                 sin( radians(b.lat) ) * sin( radians(a.lat) ) ) ) AS distance,a.lat,a.lon\
                                 FROM public_parking_space as a join land_for_sale_final as b where b.address_name = '%1') as DATA\
                                 WHERE DATA.distance < 0.5").arg(selected_address);

        qry.prepare(query2);
        if(qry.exec())
        {
            qDebug() << "Database open Success (연결성공)" << "주차장 쿼리";
            while(qry.next())
            {
                object.insert("lat",qry.value(1).toString());
                object.insert("lon",qry.value(2).toString());
                Parkinglot_information.push_back(object);
            }
        }
        else{
            qDebug() << "주차장 쿼리 왜 안되냐 ? ";
        }
        query2 = QString("SELECT lat,lon FROM land_for_sale_final where address_name = '%1'").arg(selected_address);
        qry.prepare(query2);
        if(qry.exec())
        {
            qDebug() << "Database open Success (연결성공)";
            while(qry.next())
            {
                mainObject.insert("item_lat",qry.value(0).toString());
                mainObject.insert("item_lon",qry.value(1).toString());
            }
        }
        db.close();
    }

    mainObject.insert("CCTV_information", CCTV_information);
    mainObject.insert("Parkinglot_information", Parkinglot_information);
    mainObject.insert("command","분석보기");
    QJsonDocument personDocument(mainObject);
    send_json_object(personDocument,socket);
}

// 클라이언트에게 json변환 데이터 보내주기
void MainWindow::send_json_object(QJsonDocument data,QTcpSocket *socket)
{
    int dataSize = data.toJson().size();
    qDebug() << dataSize;
    QByteArray json_bytes = data.toJson();
    QByteArray send_message = QByteArray::number(dataSize) + "|" + json_bytes;
    qDebug() << send_message;
    int dataSize2;
    QByteArray dataSize2_bytes = send_message.left(send_message.indexOf("|"));
    dataSize2 = dataSize2_bytes.toInt();

    socket->write(send_message);
    qDebug()<<dataSize;

}



