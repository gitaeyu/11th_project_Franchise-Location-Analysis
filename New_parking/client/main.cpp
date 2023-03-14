#include "mainwindow.h"
#include <QApplication>
#include <QtSql>
#include <QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QsqlError>
#include <QSqlTableModel>
#include <QSqlDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
