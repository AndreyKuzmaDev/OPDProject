#include <QProcess>
#include <QInputDialog>
#include <QDebug>
#include <QDir>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "script.h"

#include <QStringList>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    dbReady = true;

    qDebug() << QCoreApplication::applicationDirPath();

    Script *updater = new Script;
    updater->moveToThread(&workerThread);

    connect(&workerThread, &QThread::finished, updater, &QObject::deleteLater);
    connect(this, &MainWindow::doUpdate, updater, &Script::updateDB);
    connect(updater, &Script::DBUpdated, this, &MainWindow::DBUpdateDone);

    workerThread.start();

    ui->setupUi(this);
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

    if (!dbReady)
    {
        qDebug() << "DB is currently updating, pls wait";
        return;
    }

    QString search;

    search = ui->lineEdit->text();

    search.insert(0,"'");
    search.append("'");


    LowSearch = search.toLower();


    bool makeUpper = true;

    for (int i = 0; i < LowSearch.length(); i++) {
        if (makeUpper && LowSearch[i].isLetter()) {
            LowSearch[i] = LowSearch[i].toUpper();
            makeUpper = false;
        } else if (LowSearch[i] == ' ') {
            makeUpper = true;
        }
    }

    for(int i = 0; i < LowSearch.length(); i++)
    {
        if(LowSearch[i]==',')
        {
            LowSearch.insert(i,"'");
            LowSearch.insert(i+3,"'");
            i += 2;
        }

    }

    //qDebug() << LowSearch;

    model = new QSqlQueryModel;//Ã±Ã®Ã§Ã¤Ã Ã«Ã¨ Ã¬Ã®Ã¤Ã¥Ã«Ã¼ Ã¤Ã«Ã¿ Ã®Ã²Ã®Ã¡Ã°Ã Ã¦Ã¥Ã­Ã¨Ã¿ Ã§Ã Ã±Ã¯Ã°Ã®Ã±Ã

    words.clear();
// /////////////////////////////////////////////////////////////////////////////////////////////
    const QString separators = " ,;:.\"!?'*\n";
    //QVector<QString> pap;
    int start = LowSearch.indexOf(QRegExp("[^" + separators + "]"));
    while (start != -1)
    {
        int end = LowSearch.indexOf(QRegExp("[" + separators + "]"), start + 1);
        if (end == -1)
            end = LowSearch.length();

            words.push_back(LowSearch.mid(start, end - start));

        start = LowSearch.indexOf(QRegExp("[^" + separators + "]"), end + 1);
    }
// ///////////////////////////////////////////////////////////////////////////////////////////////

    QSqlQuery qry;
     QString query;
    if((tmp.isEmpty())||(ui->comboBox_2->currentIndex() == 0))
    {
    query = "SELECT r.name, r.Category "
                    "FROM Recipes r "
                    "JOIN Composition c ON r.id = c.id_recipe "
                    "JOIN Ingredients i ON c.id_ingredient = i.id "
                    "WHERE i.name IN (" + LowSearch + ")"
                    "GROUP BY r.name "
                    "HAVING COUNT(DISTINCT c.id_ingredient) = " + QString::number((int)words.size());
    }

    else
    {
        query = "SELECT r.name, r.Category "
                        "FROM Recipes r "
                        "JOIN Composition c ON r.id = c.id_recipe "
                        "JOIN Ingredients i ON c.id_ingredient = i.id "
                        "WHERE i.name IN (" + LowSearch + ")"
                        "AND  r.Category = "+ tmp +" "
                        "GROUP BY r.name "
                        "HAVING COUNT(DISTINCT c.id_ingredient) = " + QString::number((int)words.size());
    }

    if (qry.exec(query))//exec() Ã¢Ã®Ã§Ã¢Ã°Ã Ã¹Ã Ã¥Ã² Ã¡Ã³Ã«Ã¥Ã¢Ã® Ã§Ã­Ã Ã·Ã¥Ã­Ã¨Ã¥, ÃªÃ®Ã²Ã®Ã°Ã®Ã¥ Ã³ÃªÃ Ã§Ã»Ã¢Ã Ã¥Ã², Ã³Ã±Ã¯Ã¥Ã¸Ã­Ã® Ã«Ã¨ Ã¢Ã»Ã¯Ã®Ã«Ã­Ã¥Ã­ Ã§Ã Ã¯Ã°Ã®Ã±.
    {
        model->setQuery(query);
        ui->tableView->setModel(model);
        model->setHeaderData(0,Qt::Horizontal,"Recipes", Qt::DisplayRole);//Ã¨Ã§Ã¬Ã¥Ã­Ã¨Ã«Ã¨ Ã­Ã Ã§Ã¢Ã Ã­Ã¨Ã¥ Ã±Ã²Ã®Ã«Ã¡Ã¶Ã

        // ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//Ã¢Ã»Ã°Ã Ã¢Ã­Ã¨Ã¢Ã Ã­Ã¨Ã¥ Ã¯Ã® Ã¸Ã¨Ã°Ã¨Ã­Ã¥ Ã¢Ã¨Ã¤Ã¦Ã¥Ã²Ã
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed); // Ã“Ã±Ã²Ã Ã­Ã®Ã¢ÃªÃ  Ã°Ã¥Ã¦Ã¨Ã¬Ã  Ã¨Ã§Ã¬Ã¥Ã­Ã¥Ã­Ã¨Ã¿ Ã°Ã Ã§Ã¬Ã¥Ã°Ã®Ã¢ Ã¢Ã°Ã³Ã·Ã­Ã³Ã¾
        int totalWidth = ui->tableView->width(); // ÃŽÃ¡Ã¹Ã Ã¿ Ã¸Ã¨Ã°Ã¨Ã­Ã  TableView
        int firstColumnWidth = totalWidth * 0.55;
        int secondColumnWidth = totalWidth * 0.45;
        ui->tableView->setColumnWidth(0, firstColumnWidth); // Ã¯Ã¥Ã°Ã¢Ã»Ã© Ã±Ã²Ã®Ã«Ã¡Ã¥Ã¶
        ui->tableView->setColumnWidth(1, secondColumnWidth); // Ã¢Ã²Ã®Ã°Ã®Ã© Ã±Ã²Ã®Ã«Ã¡Ã¥Ã¶

        ui->tableView-> setSelectionBehavior(QAbstractItemView::SelectRows);//Ã¢Ã»Ã¤Ã¥Ã«Ã¿Ã¥Ã²Ã±Ã¿ Ã¢Ã±Ã¿ Ã±Ã²Ã°Ã®ÃªÃ , Ã  Ã­Ã¥ ÃªÃ®Ã­ÃªÃ°Ã¥Ã²Ã­Ã Ã¿ Ã¿Ã·Ã¥Ã©ÃªÃ
        ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//Ã®Ã²ÃªÃ«Ã¾Ã·Ã Ã¥Ã¬ scroll Ã¢Ã«Ã¥Ã¢Ã®/Ã¢Ã¯Ã°Ã Ã¢Ã®
          // ui->tableView->setShowGrid(false); // cÃªÃ°Ã»Ã¢Ã Ã¥Ã² Ã±Ã¥Ã²ÃªÃ³(Ã‡Ã€Ã—Ã…ÃŒ? Ã  Ã¿ Ã­Ã¥ Ã§Ã­Ã Ã¾
    }
    else return;
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        QString first = index.sibling(index.row(), 0).data().toString(); // ÃÃ¥Ã°Ã¢Ã Ã¿ ÃªÃ®Ã«Ã®Ã­ÃªÃ
        //QString second = index.sibling(index.row(), 1).data().toString();

        first.insert(0,"'");
        first.append("'");

        QSqlQuery qr;
        QString quer = "SELECT i.Name "
                       "FROM Recipes r "
                       "JOIN Composition c ON r.id = c.id_recipe "
                       "JOIN Ingredients i ON c.id_ingredient = i.id "
                       "WHERE r.name IN (" + first + ")";

        QSqlQueryModel *model2 = new QSqlQueryModel;//Ã±Ã®Ã§Ã¤Ã Ã«Ã¨ Ã¬Ã®Ã¤Ã¥Ã«Ã¼ Ã¤Ã«Ã¿ Ã®Ã²Ã®Ã¡Ã°Ã Ã¦Ã¥Ã­Ã¨Ã¿ Ã§Ã Ã±Ã¯Ã°Ã®Ã±Ã
        ui->textBrowser->setFont(QFont("Verdana", 12));//e

        if (qr.exec(quer))
        {
            model2->setQuery(quer);
           // ui->tableView->setModel(model2);

            QString result = "\t\t      ingredients \n";
            ui->textBrowser->setText(result);
            result.clear();

            for (int row = 0; row < model2->rowCount(); ++row)
            {
                QModelIndex index = model2->index(row, 0);
                result = model2->data(index).toString();
                for(QString i : words)
                {
                    if(i == result)

                        result  = "<s>" + result +" </s>"; // teg s for perecherkivanie
                }
                ui->textBrowser->append(result);
            }

            //ui->tableView->setModel(model);
        }
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

    param << QInputDialog::getText(this, QString("Parameters"), QString("Enter parameters:"), QLineEdit::Normal,
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
    get_cathegories();

}

void MainWindow::on_actionOpenDB_triggered()
{
    QString path = QFileDialog::getOpenFileName(this);

    if (db.open())
        db.close();

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(db.open())
    {
        ui->statusBar->showMessage("db is open: " + db.databaseName());
        get_cathegories();
    }
    else
        ui->statusBar->showMessage("db have error: "+ db.lastError().databaseText());
}

void MainWindow::on_actionCreateDB_triggered()
{
    bool ok;

    QString path = QFileDialog::getExistingDirectory(this);
    QString name = QInputDialog::getText(this, QString("Name"), QString("Enter new database name:"), QLineEdit::Normal,
                                         "", &ok);

    qDebug() << path + "/" + name;
    if (!ok)
        return;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path + "/" + name);

    if(db.open())
    {
        ui->statusBar->showMessage("db is open: " + db.databaseName());

        QSqlQuery qr;
        QString quer1 = "CREATE TABLE Composition ( "
                                              "id	INTEGER, "
                                              "id_recipe	INTEGER, "
                                              "id_ingredient	INTEGER, "
                                              "number	REAL, "
                                              "PRIMARY KEY(id AUTOINCREMENT), "
                                              "FOREIGN KEY(id_recipe) REFERENCES Recipes(id), "
                                              "FOREIGN KEY(id_ingredient) REFERENCES Ingredients(id) "
                                          ")";
        QString quer2 = "CREATE TABLE Ingredients ("
                                              "id	INTEGER, "
                                              "Name	text, "
                                              "Unit	TEXT, "
                                              "PRIMARY KEY(id AUTOINCREMENT) "
                                          ")";
        QString quer3 = "CREATE TABLE Recipes ("
                    "id	INTEGER, "
                    "Name	TEXT, "
                    "Category	TEXT, "
                    "Description	TEXT, "
                    "Link	TEXT, "
                    "PRIMARY KEY(id AUTOINCREMENT)"
                ")";
        qDebug() << quer1;
        model = new QSqlQueryModel;
        if (qr.exec(quer1) && qr.exec(quer2) && qr.exec(quer3))
        {
            model->setQuery(quer3);
            model->setQuery(quer2);
            model->setQuery(quer1);
            //ui->tableView->setModel(model);
            qDebug() << "here";
        }
    }
    else
        ui->statusBar->showMessage("db have error: "+ db.lastError().databaseText());
}


void MainWindow::on_comboBox_2_activated(const QString &arg1)
{

     tmp = arg1;
//     qDebug() << NotFillter;
//     qDebug() << tmp;
    tmp.insert(0,"'");
    tmp.append("'");
    QSqlQuery qry;
    QString query;


    if(ui->comboBox_2->currentIndex() == 0)//not
    {
        query = "SELECT r.name, r.Category "
                        "FROM Recipes r "
                        "JOIN Composition c ON r.id = c.id_recipe "
                        "JOIN Ingredients i ON c.id_ingredient = i.id "
                        "WHERE i.name IN (" + LowSearch + ")"
                        "GROUP BY r.name "
                        "HAVING COUNT(DISTINCT c.id_ingredient) = " + QString::number((int)words.size());
    }

   else
    {
        query = "SELECT r.name, r.Category "
                        "FROM Recipes r "
                        "JOIN Composition c ON r.id = c.id_recipe "
                        "JOIN Ingredients i ON c.id_ingredient = i.id "
                        "WHERE i.name IN (" + LowSearch + ")"
                        "AND  r.Category = "+ tmp +" "
                        "GROUP BY r.name "
                        "HAVING COUNT(DISTINCT c.id_ingredient) = " + QString::number((int)words.size());
    }


   if(qry.exec(query))
   {
     model->setQuery(query);
     ui->tableView->setModel(model);
   }
}


void MainWindow::get_cathegories()
{
    QStringList cathegories;
    QString tmp;
    QSqlQuery qr;
    QString quer = "SELECT DISTINCT	Category  FROM Recipes";

    QSqlQueryModel *modeltmp = new QSqlQueryModel;

    if (qr.exec(quer))

    {
         modeltmp->setQuery(quer);
        for (int row = 0; row < modeltmp->rowCount(); ++row)
        {
            QModelIndex index = modeltmp->index(row, 0);
            tmp = modeltmp->data(index).toString();
            //qDebug() << tmp;
            cathegories << tmp;
        }
       //NotFillter = ui->comboBox_2->itemText(0);
       //qDebug() << NotFillter;
        ui->comboBox_2->addItems(cathegories);
    }
}
