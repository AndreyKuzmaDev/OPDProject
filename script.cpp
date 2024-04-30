#include "script.h"
#include <QProcess>
#include <QDebug>

void Script::update_DB(const QStringList &params)
{
    qDebug() << "Started";

    QString program("C:\\prog\\project\\temp\\scripts\\data_transfer.exe");
    qDebug() << QProcess::execute(program, params);

    qDebug() << "Finished";

    emit DB_updated();
}
