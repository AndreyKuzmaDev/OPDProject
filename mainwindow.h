#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <vector>
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
#include <QThread>


using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread workerThread;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void DBUpdateDone();

private slots:
    void on_search_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_actionUpdateDB_triggered();

    void on_actionOpenDB_triggered();

    void on_actionCreateDB_triggered();

signals:
    void doUpdate(const QStringList &params);

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQueryModel *model;


    bool dbReady;
};

#endif // MAINWINDOW_H
