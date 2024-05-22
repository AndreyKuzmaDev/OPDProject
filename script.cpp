#include "script.h"
#include <QProcess>
#include <QDebug>
#include <QFile>

void Script::update_DB(const QStringList &params)
{
    if (QFile::exists("C://prog//temp//data_transfer.exe"))
    {
        QFile::remove("C://prog//temp//data_transfer.exe");
    }
    qDebug() << "Started";

    qDebug() << QFile::copy(":/scripts/data_transfer.exe", "C://prog//temp//data_transfer.exe");
    QString program("C://prog//temp//data_transfer.exe");

    qDebug() << QProcess::execute(program, params);

    qDebug() << "Finished";

    QFile::remove("C://prog//temp//data_transfer.exe");
    emit DB_updated();
}
