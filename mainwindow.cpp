#include <QProcess>
#include <QInputDialog>
#include <QDebug>
#include <QDir>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "script.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    dbReady = true;

    Script *updater = new Script;
    updater->moveToThread(&workerThread);

    connect(&workerThread, &QThread::finished, updater, &QObject::deleteLater);
    connect(this, &MainWindow::doUpdate, updater, &Script::updateDB);
    connect(updater, &Script::DBUpdated, this, &MainWindow::DBUpdateDone);
    workerThread.start();

    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/prog/project/temp/scripts/delicious.db");

// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(db.open())
    {
        //ui->statusBar->showMessage("db is open: " + db.databaseName());//ç� ïèñü î óä� ÷íîì ïîäêëþ÷åíèè ê á� çå ä� ííûõ

              // model = new QSqlTableModel(this,db);// âûâîä á� çû ä� ííûõ í�  ýêð� í
              //  ui->tableView->setModel(model);
    }
    else
        ui->statusBar->showMessage("db have error: "+ db.lastError().databaseText());
}     //   ÎÁÐÀÁÎÒÊÀ ÑÎÁÛÒÈÉ ÓÄÀ×ÍÎÃÎ È ÍÅÓÄÀ×ÍÎÃÎ ÏÎÄÊËÞ×ÅÍÈß Ê ÁÄ
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
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
       //ÊÀÂÛ×ÊÈ Â ÍÀ×ÀËÈ È ÊÎÍÖÅ ÇÀÏÐÎÑÀ

         QString LowSearch = search.toLower();//ïîèñêîâ� ÿ ñòðîê�  â íèæíåì ðåãèñòðå
         // òåïåðü íóæíî ïðîñò� âèòü ç� ãë� âíûå áóêâû â ñëîâ� õ))

          bool makeUpper = true;

              for (int i = 0; i < LowSearch.length(); i++) {
                  if (makeUpper && LowSearch[i].isLetter()) {
                      LowSearch[i] = LowSearch[i].toUpper();
                      makeUpper = false;
                  } else if (LowSearch[i] == ' ') {
                      makeUpper = true;
                  }
              }// ÏÅÐÂÛÅ ÁÓÊÂÛ ÑÒÀÍÎÂßÒÑß ÇÀÃËÀÂÍÛÌÈ

              for(int i = 0; i < LowSearch.length(); i++)//Ïðîññò� âëÿåì êîâû÷êè â íóæíûõ ìåñò� õ
              {
                  if(LowSearch[i]==',')
                  {
                     LowSearch.insert(i,"'");
                     LowSearch.insert(i+3,"'");
                     i += 2;
                  }

              }//ÊÀÂÛ×ÊÈ ÌÅÆÄÓ ÑËÎÂ Â ÇÀÏÐÎÑÅ

             //qDebug() << LowSearch;

        model = new QSqlQueryModel;//ñîçä� ëè ìîäåëü äëÿ îòîáð� æåíèÿ ç� ñïðîñ� 

// /////////////////////////////////////////////////////////////////////////////////////////////
        const string separators{ " ,;:.\"!?'*\n" };
        vector <string> words; // âåêòîð äëÿ õð� íåíèÿ ñëîâ
        size_t start { LowSearch.toStdString().find_first_not_of(separators) };
        while (start != string::npos)
        {
            size_t end = LowSearch.toStdString().find_first_of(separators, start + 1);
            if (end == string::npos)
                end = search.toStdString().length();
            words.push_back(LowSearch.toStdString().substr(start, end - start));
            start = LowSearch.toStdString().find_first_not_of(separators, end + 1);
        }//ÏÎÄÑ×ÅÒ ÑËÎÂ Â ÑÒÐÎÊÅ
// ///////////////////////////////////////////////////////////////////////////////////////////////

        QSqlQuery qry;
        QString query = "SELECT r.name, r.Category "
                        "FROM Recipes r "
                        "JOIN Composition c ON r.id = c.id_recipe "
                         "JOIN Ingredients i ON c.id_ingredient = i.id "
                        "WHERE i.name IN (" + LowSearch + ")"
                        "GROUP BY r.name "
                        "HAVING COUNT(DISTINCT c.id_ingredient) = " + QString::number((int)words.size());

        if (qry.exec(query))//exec() âîçâð� ù� åò áóëåâî çí� ÷åíèå, êîòîðîå óê� çûâ� åò, óñïåøíî ëè âûïîëíåí ç� ïðîñ.
        {
            model->setQuery(query);
            ui->tableView->setModel(model);

           model->setHeaderData(0,Qt::Horizontal,"Recipes", Qt::DisplayRole);//èçìåíèëè í� çâ� íèå ñòîëáö� 


          // ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//âûð� âíèâ� íèå ïî øèðèíå âèäæåò� 
           ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed); // Óñò� íîâê�  ðåæèì�  èçìåíåíèÿ ð� çìåðîâ âðó÷íóþ
           int totalWidth = ui->tableView->width(); // Îáù� ÿ øèðèí�  TableView
           int firstColumnWidth = totalWidth * 0.55;
           int secondColumnWidth = totalWidth * 0.45;
           ui->tableView->setColumnWidth(0, firstColumnWidth); // ïåðâûé ñòîëáåö
           ui->tableView->setColumnWidth(1, secondColumnWidth); // âòîðîé ñòîëáåö

          ui->tableView-> setSelectionBehavior(QAbstractItemView::SelectRows);//âûäåëÿåòñÿ âñÿ ñòðîê� , �  íå êîíêðåòí� ÿ ÿ÷åéê� 
          ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//îòêëþ÷� åì scroll âëåâî/âïð� âî
          // ui->tableView->setShowGrid(false); // cêðûâ� åò ñåòêó(ÇÀ×ÅÌ? �  ÿ íå çí� þ
        }
        else return;
    }


void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
           QString first = index.sibling(index.row(), 0).data().toString(); // Ïåðâ� ÿ êîëîíê� 
           QString second = index.sibling(index.row(), 1).data().toString();

           first.insert(0,"'");
           first.append("'");

           QSqlQuery qr;
           QString quer = "SELECT i.Name "
                           "FROM Recipes r "
                           "JOIN Composition c ON r.id = c.id_recipe "
                           "JOIN Ingredients i ON c.id_ingredient = i.id "
                           "WHERE r.name IN (" + first + ")";


          QSqlQueryModel *model2 = new QSqlQueryModel;//ñîçä� ëè ìîäåëü äëÿ îòîáð� æåíèÿ ç� ñïðîñ� 
          ui->textBrowser->setFont(QFont("Verdana", 12));//e

           if (qr.exec(quer))
           {
             model2->setQuery(quer);
             ui->tableView->setModel(model2);

               QString result = "List ingredients:\n";
               ui->textBrowser->setText(result);
                result.clear();

                for (int row = 0; row < model2->rowCount(); ++row)
                {
                    QModelIndex index = model2->index(row, 0);
                    result += model2->data(index).toString() + "\n";
                }

                ui->textBrowser->append(result);
                 ui->tableView->setModel(model);
           }


    }


}

void MainWindow::on_actionUpdateDB_triggered()
{
    bool ok;
    const QStringList param = QInputDialog::getText(this, QString("Parameters"), QString("Enter parameters:"), QLineEdit::Normal,
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
}

