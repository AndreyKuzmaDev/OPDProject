#include <QProcess>
#include <QInputDialog>
#include <QDebug>
#include <QDir>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "script.h"
#include <iomanip>

#include <QListView>
#include <QStringList>
#include <QUrl>
#include <QAbstractItemView>

#include <QPixmap>
#include <QPalette>

#define DEFAULT_DB_NAME "recipes.db"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    cathegory = CATHEGORY_EMPTY;

    dbReady = true;

    // qDebug() << QCoreApplication::applicationDirPath();

    Script *updater = new Script;
    updater->moveToThread(&workerThread);

    connect(&workerThread, &QThread::finished, updater, &QObject::deleteLater);
    connect(this, &MainWindow::doUpdate, updater, &Script::updateDB);
    connect(updater, &Script::DBUpdated, this, &MainWindow::DBUpdateDone);

    workerThread.start();

    ui->setupUi(this);
    // ui->statusBar->hide();

    QPixmap backgroundImage(":/new/prefix1/Frame 3 1.png");
    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(backgroundImage));
    this->setPalette(palette);

    ui->lineEdit->setPlaceholderText("Please connect the database...");

    // this->setStyleSheet("background-color: rgb(130, 149, 150);");
    ui->menuBar->setStyleSheet("background-color:rgb(198, 227, 245);font:  14px \"Arial\" ;");
    ui->search->setStyleSheet("QPushButton {"
                                  "background-color: rgb(198, 227, 245);"
                                                        "border-style: outset;"
                                                        "border-width: 2px;"
                                                        "border-radius: 7px;"
                                                        "border-color: #7f7f7f;"
                                                        "font: bold 14px;"
                                                        "min-width: 10em;"
                                                        "padding: 5px ;"
                                                        "}"
                                                        "QPushButton:pressed {"
                                                        "background-color: rgb(170, 200, 255);"
                                                        "border-style:inset;"
                                                         "border-width: 4px;"
                                                        "}");

    ui->comboBox_2->setStyleSheet("background-color: rgb(247, 247, 242);color: rgb(0, 0, 0);  border: 1px solid gray; border-radius: 7px;border: 2px solid #7f7f7f;padding: 1px 18px 1px 3px;");
    ui->comboBox_2->setView(new QListView());

    ui->lineEdit->setStyleSheet("background-color: rgb(247, 247, 242);"
                                 "border: 2px solid #7f7f7f;"
                                 "border-radius: 7px;"
                                 "padding: 5px;"
                                 "selection-background-color: rgb(198, 227, 245);"
                                 "font:  14px \"Arial\" ;");

    ui->textBrowser->setStyleSheet("background-color: rgb(247, 247, 242);border-radius: 7px;border: 2px solid #7f7f7f;");

    // ui->tableView->setStyleSheet("background-color: rgb(247, 247, 242);");
    ui->tableView->verticalHeader()->hide();

    ui->tableView->setStyleSheet("QTableView { background-color: rgb(247, 247, 242);"
                                      "             color: black;border-radius: 7px;border: 2px solid #7f7f7f;"
                                      "             selection-background-color:  rgb(198, 227, 245);"
                                    "   font:  14px \"Arial\" }"
                                    "QHeaderView::section { background-color: lightblue; color: black; border: 1px solid gray;}");

}
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
    workerThread.exit();
    delete model;
    delete ui;
}

void MainWindow::on_search_clicked()
{

    setlocale(LC_ALL, "RUS");

    LowSearch = ui->lineEdit->text();
    ui->tableView->setModel(manager.search(LowSearch, cathegory));

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    int totalWidth = ui->tableView->width();
    int firstColumnWidth = totalWidth * 0.55;
    int secondColumnWidth = totalWidth * 0.45;
    ui->tableView->setColumnWidth(0, firstColumnWidth);
    ui->tableView->setColumnWidth(1, secondColumnWidth);

    ui->tableView-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{

    ui->textBrowser->clear();

    if (index.isValid())
    {
        QString recipe_name = index.sibling(index.row(), 0).data().toString(); // recipeName
        // QString second = index.sibling(index.row(), 1).data().toString(); // categoryName

        QString details = manager.get_recipe_details(recipe_name);
        ui->textBrowser->insertHtml(details);
    }
}



void MainWindow::on_actionUpdateDB_triggered()
{
    bool ok;
    QStringList param;

    if (db.open())
        param << db.databaseName();
    else
        return;

    param << QInputDialog::getText(this, QString("Parameters:"), QString("Enter parameters:"), QLineEdit::Normal,
                                   "", &ok).split(' ');

    if (ok && !param[0].isEmpty())
    {
        dbReady = false;
        emit doUpdate(param);
    }
}

void MainWindow::DBUpdateDone()
{
    dbReady = true;
    set_cathegories();
}

void MainWindow::on_actionOpenDB_triggered()
{
    QString path = QFileDialog::getOpenFileName(this);
    if(path.isEmpty())
        return;

    if (db.open())
        db.close();

    bool res = manager.openDB(path);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(res)//ask Lev Olegovich
    {
        ui->statusBar->showMessage("db is open: " + manager.get_name());
        set_cathegories();
        ui->lineEdit->setPlaceholderText("Enter available ingredients...");
    }
    else
        ui->statusBar->showMessage("db have error: "+ manager.get_err());
}

void MainWindow::on_actionCreateDB_triggered()
{
    bool ok;

    QString path = QFileDialog::getExistingDirectory(this);
    if(path.isEmpty()) return;
    QString name = QInputDialog::getText(this, QString("Name"), QString("Enter new database name:"), QLineEdit::Normal,
                                         "", &ok);
    if(name.isEmpty()) name = DEFAULT_DB_NAME;
    qDebug() << path + "/" + name;
    if (!ok)
        return;

    bool res = manager.createDB(path, name);

    if(res)
    {
        ui->lineEdit->setPlaceholderText("Enter available ingredients...");
        ui->statusBar->showMessage("db is open: " + name);
    }
    else
        ui->statusBar->showMessage("db have error: "+ db.lastError().databaseText());
}


void MainWindow::on_comboBox_2_activated(const QString &arg1)
{
    cathegory = arg1;
}


void MainWindow::set_cathegories()//methods for combobox
{

    QStringList cathegories = manager.get_cathegories();
    ui->comboBox_2->addItems(cathegories);
}
