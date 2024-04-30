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

    QPixmap backgroundImage(":/new/prefix1/Frame 6 (1).png");
    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(backgroundImage));
    this->setPalette(palette);

    ui->lineEdit->setPlaceholderText("Please connect the database...");

    ui->comboBox_2->setView(new QListView());

    ui->tableView->verticalHeader()->hide();
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

    ui->statusBar->clearMessage();

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
