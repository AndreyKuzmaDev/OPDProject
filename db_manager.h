#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include <QThread>

#define CATHEGORY_EMPTY "Без фильтрации"

#define PLACEHOLDER_CONNECT_DB "Please connect the database..."
#define PLACEHOLDER_ENTER_INGREDIENTS "Enter avaliable ingredients..."

class DB_manager : public QThread
{
    Q_OBJECT

private:
    QSqlDatabase db;
    QSqlQueryModel *model;
    QVector<QString> words;
    QStringList cathegories;

    bool db_ready;

public slots:
    void search(QString request, QString cathegory);
    void open_DB(QString path);
    void create_DB(QString path, QString name);
    void get_cathegories_slot();
    void get_recipe_details(QString recipe_name);

signals:
    void search_done(QSqlQueryModel* res);
    void DB_opened(bool res, QString message, QStringList* cathegories);
    void DB_created(bool res, QString message, QStringList* cathegories);
    void got_cathegories(QStringList* cathegories);
    void got_recipe_details(QString res);

public:
    DB_manager();
    QString get_name();
    QString get_err();
    void get_cathegories();
};

#endif // DB_MANAGER_H
