#ifndef SCRIPT_H
#define SCRIPT_H

#include <QThread>

class Script : public QThread
{
    Q_OBJECT

public slots:
    void update_DB(const QStringList &params);

signals:
    void DB_updated();
};

#endif // SCRIPT_H
