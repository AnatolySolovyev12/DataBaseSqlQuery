#include <QtCore/QCoreApplication>
#include <qfile.h>
#include <QSqlDatabase>
#include <qsqlerror.h>
#include <QSqlQuery>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTextStream out(stdout);
    QTextStream in(stdin);

    qDebug() << "clear - clear LOG file. exit - close application.\n";

    QSqlDatabase mw_db = QSqlDatabase::addDatabase("QODBC"); // указываем какой использовать драйвер для подключения к БД и имя подключения. Если имя не задано то пор умолчанию подключаемся к этой базе. Вроде так.

    mw_db.setHostName("10.86.142.47");
    mw_db.setDatabaseName("DBTESTZ");
    mw_db.setUserName("solexp");
    mw_db.setPassword("RootToor#");

    if (!mw_db.open()) // открываем БД. Если не открывает то вернёт false
    {
        QString any;
        QString any2;

        any = mw_db.lastError().databaseText();
        any2 = mw_db.lastError().driverText();

        qDebug() << "Cannot open database: " << mw_db.lastError();

        qDebug() << "LOG file was created in the root directory of the program.";
       
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
    QString str_t;

    do {

        str_t = in.readLine(); // записываем данные из потока цельной строкой в переменную.

        query.exec(str_t); // Отправляем запрос на количество записей

        while (query.next())
        {
            str_t = query.value(0).toString();

            qDebug() << str_t;

            QFile file("LOG.txt");
            file.open(QIODevice::WriteOnly | QIODevice::Append);
            QTextStream out(&file); // поток записываемых данных направляем в файл

            // Для записи данных в файл используем оператор <<
            out << str_t << Qt::endl;

            file.close();
        }

        if (str_t == "exit")
        {
            mw_db.removeDatabase("DBTESTZ");
            return 1;
        }

        if (str_t == "clear")
        {
            QFile file("LOG.txt");
            file.open(QIODevice::WriteOnly | QIODevice::Truncate);
            file.close();
        }

    } while (true);

    return a.exec();
}
