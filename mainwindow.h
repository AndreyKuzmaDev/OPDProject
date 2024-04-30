#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QVector>
#include <string>
#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <iostream>
#include <locale.h>
#include <windows.h>
#include <QDebug>
#include <QFrame>
#include <QPoint>
#include <qthread.h>
#include <QComboBox>
#include "db_manager.h"


using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread updater_thread;
    QThread manager_thread;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void update_done();
    void search_done(QSqlQueryModel* res);
    void open_done(bool res, QString message, QStringList* cathegories);
    void create_done(bool res, QString message, QStringList* cathegories);
    void got_cathegories(QStringList* cathegories);
    void got_recipe_details(QString res);

private slots:
    void on_search_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_actionUpdateDB_triggered();

    void on_actionOpenDB_triggered();

    void on_actionCreateDB_triggered();

    void on_comboBox_2_activated(const QString &arg1);

signals:
    void do_update(const QStringList &params);
    void do_search(QString request, QString cathegory);
    void do_open(QString path);
    void do_create(QString path, QString name);
    void do_get_cathegories();
    void do_get_recipe_details(QString recipe_name);

private:
    Ui::MainWindow *ui;

    QSqlDatabase db;
    QSqlQueryModel *model;//basic model
    QVector<QString> words;//all words in search
    QString LowSearch;//Processed string for sqlQuery
    QString cathegory;//current category


    bool dbReady;

    void set_cathegories(QStringList* cathegories);
};

#endif // MAINWINDOW_H
