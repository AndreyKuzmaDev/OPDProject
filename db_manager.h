#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>

#define CATHEGORY_EMPTY "Без фильтрации"

#define PLACEHOLDER_CONNECT_DB "Please connect the database..."
#define PLACEHOLDER_ENTER_INGREDIENTS "Enter avaliable ingredients..."

class DB_manager
{
private:
    QSqlDatabase db;
    QSqlQueryModel *model;
    QVector<QString> words;

    bool db_ready;

public:
    DB_manager();

    QSqlQueryModel* search(QString request, QString cathegory);

    bool openDB(QString path);

    bool createDB(QString path, QString name);

    QString get_name();
    QString get_err();
    QStringList get_cathegories();
    QString get_recipe_details(QString recipe_name);
};

#endif // DB_MANAGER_H
