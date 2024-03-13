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
#include <QThread>
#include <QComboBox>


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

    void on_comboBox_2_activated(const QString &arg1);

signals:
    void doUpdate(const QStringList &params);

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQueryModel *model;
    QVector<QString> words;
    QString LowSearch;
    QString tmp;
    QString NotFillter;

    bool dbReady;
};

#endif // MAINWINDOW_H
