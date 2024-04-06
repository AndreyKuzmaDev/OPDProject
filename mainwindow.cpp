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
#include<QAbstractItemView>

#include <QPixmap>
#include <QPalette>


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

      ui->statusBar->hide();

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

    if (!dbReady)
    {
        qDebug() << "DB is currently updating, pls wait";
        return;
    }



    LowSearch = ui->lineEdit->text();
    LowSearch.remove(QChar(' '), Qt::CaseInsensitive);//Voda,Voda

    LowSearch.insert(0,"'");
    LowSearch.append("'");//'Voda,Voda'

  LowSearch = LowSearch.toLower();
LowSearch[1] = LowSearch[1].toUpper();

  for (int i = 0; i < LowSearch.length(); i++) {
      if (LowSearch[i] == ',') {
          LowSearch[i+1] = LowSearch[i+1].toUpper();
      }
  }

  for(int i = 0; i < LowSearch.length(); i++)
  {
      if(LowSearch[i]==',')
      {
          LowSearch.insert(i,"'");
          LowSearch.insert(i+2,"'");
          i += 1;
      }
  }

    //--------------edit search

    model = new QSqlQueryModel;

    words.clear();//words is liststring for counting
// /////////////////////////////////////////////////////////////////////////////////////////////
    const QString separators = " ,;:.\"!?'*\n";
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

    if((tmp.isEmpty())||(ui->comboBox_2->currentIndex() == 0))//if not fillter or fillter = bez filltracii
    {
    query = "SELECT r.name, r.Category "
                    "FROM Recipes r "
                    "JOIN Composition c ON r.id = c.id_recipe "
                    "JOIN Ingredients i ON c.id_ingredient = i.id "
                    "WHERE i.name IN (" + LowSearch + ")"
                    "GROUP BY r.name "
                    "HAVING COUNT(DISTINCT c.id_ingredient) = " + QString::number((int)words.size());
    }

    else// else query with selected category
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

    if (qry.exec(query))//exec() - bool funct : 1 if  query work,  else 0
    {
        model->setQuery(query);
        ui->tableView->setModel(model);
        model->setHeaderData(0,Qt::Horizontal,"Recipes", Qt::DisplayRole);

        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        int totalWidth = ui->tableView->width();
        int firstColumnWidth = totalWidth * 0.55;
        int secondColumnWidth = totalWidth * 0.45;
        ui->tableView->setColumnWidth(0, firstColumnWidth);
        ui->tableView->setColumnWidth(1, secondColumnWidth);

        ui->tableView-> setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    }
    else return;
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{

    ui->textBrowser->clear();

    if (index.isValid()) {
        QString first = index.sibling(index.row(), 0).data().toString(); // recipeName
        //QString second = index.sibling(index.row(), 1).data().toString();// categoryName

        first.insert(0,"'");
        first.append("'");

        QSqlQuery qr;
        QString quer = "SELECT i.Name,c.number,i.Unit "    // all ingredients for current recipe
                       "FROM Recipes r "
                       "JOIN Composition c ON r.id = c.id_recipe "
                       "JOIN Ingredients i ON c.id_ingredient = i.id "
                       "WHERE r.name IN (" + first + ")";
        QString second = first;

        QSqlQueryModel *model2 = new QSqlQueryModel;// new model for a simultaneosly process
        //ui->textBrowser->setFont(QFont("Verdana", 12));//


        if (qr.exec(quer))
        {
            model2->setQuery(quer);

            QString result = "<center>ingredients<\center>";


            ui->textBrowser->insertHtml("<div style='text-align: left;'>" + result + "</div>");
            ui->textBrowser->insertPlainText("\n");


            QString tmp = " ";
             int maxlength = 0;

             for (int row = 0; row < model2->rowCount(); ++row)
             {
                 QModelIndex index = model2->index(row, 0);
                  result = model2->data(index).toString();
                  if(result.length() > maxlength) maxlength = result.length();
             }
             //qDebug() << maxlength;

            for (int row = 0; row < model2->rowCount(); ++row)
            {

                    result.clear();
                    tmp.clear();
                    first.clear();

                    QModelIndex index = model2->index(row, 0);
                     result = model2->data(index).toString();


                for(int column = 1; column < model2->columnCount(); ++column)
                {
                  QModelIndex index = model2->index(row, column);
                     tmp+= model2->data(index).toString();
                          tmp += " ";
                }


                int tempLength = maxlength - result.length();

                for(QString i : words)
                {
                    if(i == result)

                        result  = "<b>" + result +"</b>"; // teg <s> for crossing out
                }


                for(int i = 0; i <= tempLength; ++i)
                   first += QString(" ");

           //qDebug() <<result + tmp << tempLength;

            //  ui->textBrowser->;
              ui->textBrowser->insertHtml(result);
              ui->textBrowser->insertPlainText(first+"    " + tmp + "\n");
            }
        }

        quer = "SELECT r.Link "
               "FROM Recipes r "
               "WHERE r.name IN (" + second + ")";

        if (qr.exec(quer))
        {
            first.clear();
           model2->setQuery(quer);
            QModelIndex index = model2->index(0, 0);
             first = model2->data(index).toString();
             ui->textBrowser->append("\n");
           //ui->textBrowser->insertHtml(first);//если последний, то не будет выделения ссылки

             QUrl url(first);
             ui->textBrowser->setOpenExternalLinks(true);
             ui->textBrowser->insertHtml("<a href=\"" + url.toString() + "\" >this recipe</a>");
        }


        delete model2;
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
    get_cathegories();

}

void MainWindow::on_actionOpenDB_triggered()
{
    QString path = QFileDialog::getOpenFileName(this);
    if(path.isEmpty())
        return;

    if (db.open())
        db.close();

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(db.open())//ask Lev Olegovich
    {
      //  ui->statusBar->showMessage("db is open: " + db.databaseName());
        get_cathegories();
        ui->lineEdit->setPlaceholderText("Enter available ingredients...");
    }
//    else
//        //ui->statusBar->showMessage("db have error: "+ db.lastError().databaseText());
}

void MainWindow::on_actionCreateDB_triggered()
{
    bool ok;

    QString path = QFileDialog::getExistingDirectory(this);
    if(path.isEmpty()) return;
    QString name = QInputDialog::getText(this, QString("Name"), QString("Enter new database name:"), QLineEdit::Normal,
                                         "", &ok);
    if(name.isEmpty()) name = "recipes.db";
    qDebug() << path + "/" + name;
    if (!ok)
        return;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path + "/" + name);

    if(db.open())
    {
         ui->lineEdit->setPlaceholderText("Enter available ingredients...");
      //  ui->statusBar->showMessage("db is open: " + db.databaseName());

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
//    else
//        ui->statusBar->showMessage("db have error: "+ db.lastError().databaseText());
}


void MainWindow::on_comboBox_2_activated(const QString &arg1)
{
   if(!LowSearch.isEmpty())
   {
     tmp = arg1;
    tmp.insert(0,"'");
    tmp.append("'");
    QSqlQuery qry;
    QString query;


    if(ui->comboBox_2->currentIndex() == 0)
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
   else return ;
}


void MainWindow::get_cathegories()//methods for combobox
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
            cathegories << tmp;
        }

        ui->comboBox_2->addItems(cathegories);
      }
        delete modeltmp;
            return;
}
