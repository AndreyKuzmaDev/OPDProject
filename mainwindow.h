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



using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT



public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_search_clicked();



    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
     QSqlDatabase db;

};

#endif // MAINWINDOW_H
