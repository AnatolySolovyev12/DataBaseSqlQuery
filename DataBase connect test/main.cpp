#include <QtCore/QCoreApplication>
#include <qfile.h>
#include <QSqlDatabase>
#include <qsqlerror.h>
#include <QSqlQuery>
#include <QTime>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTextStream out(stdout);
    QTextStream in(stdin);

    qDebug() << "clear - clear LOG file. exit - close application.\n";

    qDebug() << "LOG file was created in the root directory of the program.\n";

    QSqlDatabase mw_db = QSqlDatabase::addDatabase("QODBC"); // Для раблоты ODBC в Windows необходимо задвать пользовательский DNS в администрировании системы. Иначен не будет работать.

    mw_db.setHostName("10.86.142.47"); // хост где лежит БД
    mw_db.setDatabaseName("DBTESTZ"); // указываем имя пользовательского DNS который был создан в системе ранее.
    mw_db.setUserName("solexp");
    mw_db.setPassword("RootToor#");

    if (!mw_db.open()) // открываем БД. Если не открывает то вернёт false
    {
        QString any;
        QString any2;

        any = mw_db.lastError().databaseText();
        any2 = mw_db.lastError().driverText();

        qDebug() << "Cannot open database: " << mw_db.lastError();
       
        QFile file("LOG.txt");
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream out(&file); // поток записываемых данных направляем в файл

        // Для записи данных в файл используем оператор <<
        out << any << Qt::endl;
        out << any2 << Qt::endl;

        file.close();

    }
    else
        qDebug() << "DataBase is CONNECT.";

    QSqlQuery query;
    QString number;
    QString queryString;
    QString result;

    do {

        QDate curDate = QDate::currentDate();
        curDate = curDate.addDays(-1);
        QTime curTime = QTime::currentTime();
        QString timeInQuery = curDate.toString("yyyy-MM-dd");

        number = "'" + in.readLine() + "'"; // записываем данные из потока цельной строкой в переменную.

        if (number == "''")
            continue;

        qDebug() << number;

        if (number == "'exit'")
        {
            mw_db.removeDatabase("DBTESTZ");
            return 1;
        }

        if (number == "'clear'")
        {
            QFile file("LOG.txt");
            file.open(QIODevice::WriteOnly | QIODevice::Truncate);
            file.close();
            continue;
        }

        queryString = "select IDOBJECT_PARENT from dbo.PROPERTIES where PROPERTY_VALUE = " + number;

        query.exec(queryString); // Отправляем запрос на количество записей

        query.next();

        int iD =  query.value(0).toInt() - 1;

        queryString = "select VALUE_METERING from dbo.METERINGS where  IDOBJECT = '" + number.setNum(iD) + "' AND IDTYPE_OBJECT = '1201001' AND IDOBJECT_AGGREGATE = '1' AND TIME_END = '" + timeInQuery +" 19:00:00.0000000' AND VALUE_METERING != '0'";

        query.exec(queryString);

        query.next();

        result += "Day = " + query.value(0).toString();

        queryString = "select VALUE_METERING from dbo.METERINGS where  IDOBJECT = '" + number.setNum(iD) + "' AND IDTYPE_OBJECT = '1202001' AND IDOBJECT_AGGREGATE = '1' AND TIME_END = '" + timeInQuery + " 19:00:00.0000000' AND VALUE_METERING != '0'";

        query.exec(queryString);

        query.next();

        result += "   Night = " + query.value(0).toString();

        qDebug() << result;

        QFile file("LOG.txt");
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream out(&file); // поток записываемых данных направляем в файл

        // Для записи данных в файл используем оператор <<
        out << result << Qt::endl;

        file.close();

        result = "";

    } while (true);
    
    return a.exec();
}
