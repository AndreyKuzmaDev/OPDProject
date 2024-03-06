#ifndef SCRIPT_H
#define SCRIPT_H

#include <QThread>

class Script : public QThread
{
    Q_OBJECT

public slots:
    void updateDB(const QStringList &params);

signals:
    void DBUpdated();
};

#endif // SCRIPT_H
