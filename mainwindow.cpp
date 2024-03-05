
#include <QProcess>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/Users/acer/Desktop/SQLiteDatabaseBrowserPortable/delicious.db");

// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(db.open())
    {
        //ui->statusBar->showMessage("db is open: " + db.databaseName());//çàïèñü î óäà÷íîì ïîäêëþ÷åíèè ê áàçå äàííûõ

              // model = new QSqlTableModel(this,db);// âûâîä áàçû äàííûõ íà ýêðàí
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

      QString search;

        search = ui->lineEdit->text();

        search.insert(0,"'");
        search.append("'");
       //ÊÀÂÛ×ÊÈ Â ÍÀ×ÀËÈ È ÊÎÍÖÅ ÇÀÏÐÎÑÀ

         QString LowSearch = search.toLower();//ïîèñêîâàÿ ñòðîêà â íèæíåì ðåãèñòðå
         // òåïåðü íóæíî ïðîñòàâèòü çàãëàâíûå áóêâû â ñëîâàõ))

          bool makeUpper = true;

              for (int i = 0; i < LowSearch.length(); i++) {
                  if (makeUpper && LowSearch[i].isLetter()) {
                      LowSearch[i] = LowSearch[i].toUpper();
                      makeUpper = false;
                  } else if (LowSearch[i] == ' ') {
                      makeUpper = true;
                  }
              }// ÏÅÐÂÛÅ ÁÓÊÂÛ ÑÒÀÍÎÂßÒÑß ÇÀÃËÀÂÍÛÌÈ

              for(int i = 0; i < LowSearch.length(); i++)//Ïðîññòàâëÿåì êîâû÷êè â íóæíûõ ìåñòàõ
              {
                  if(LowSearch[i]==',')
                  {
                     LowSearch.insert(i,"'");
                     LowSearch.insert(i+3,"'");
                     i += 2;
                  }

              }//ÊÀÂÛ×ÊÈ ÌÅÆÄÓ ÑËÎÂ Â ÇÀÏÐÎÑÅ

             //qDebug() << LowSearch;

        model = new QSqlQueryModel;//ñîçäàëè ìîäåëü äëÿ îòîáðàæåíèÿ çàñïðîñà

// /////////////////////////////////////////////////////////////////////////////////////////////
        const string separators{ " ,;:.\"!?'*\n" };
        vector <string> words; // âåêòîð äëÿ õðàíåíèÿ ñëîâ
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

        if (qry.exec(query))//exec() âîçâðàùàåò áóëåâî çíà÷åíèå, êîòîðîå óêàçûâàåò, óñïåøíî ëè âûïîëíåí çàïðîñ.
        {
            model->setQuery(query);
            ui->tableView->setModel(model);

           model->setHeaderData(0,Qt::Horizontal,"Recipes", Qt::DisplayRole);//èçìåíèëè íàçâàíèå ñòîëáöà


          // ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//âûðàâíèâàíèå ïî øèðèíå âèäæåòà
           ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed); // Óñòàíîâêà ðåæèìà èçìåíåíèÿ ðàçìåðîâ âðó÷íóþ
           int totalWidth = ui->tableView->width(); // Îáùàÿ øèðèíà TableView
           int firstColumnWidth = totalWidth * 0.55;
           int secondColumnWidth = totalWidth * 0.45;
           ui->tableView->setColumnWidth(0, firstColumnWidth); // ïåðâûé ñòîëáåö
           ui->tableView->setColumnWidth(1, secondColumnWidth); // âòîðîé ñòîëáåö

          ui->tableView-> setSelectionBehavior(QAbstractItemView::SelectRows);//âûäåëÿåòñÿ âñÿ ñòðîêà, à íå êîíêðåòíàÿ ÿ÷åéêà
          ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//îòêëþ÷àåì scroll âëåâî/âïðàâî
          // ui->tableView->setShowGrid(false); // cêðûâàåò ñåòêó(ÇÀ×ÅÌ? à ÿ íå çíàþ
        }
        else return;
    }



void MainWindow::on_buttonRunPy_clicked()
{
    std::cout << "Started" << std::endl;

    QString program("C:\\prog\\project\\temp\\scripts\\data_transfer.exe");
    QStringList parameters;
    parameters << "3001" << "3010";
    std::cout << QProcess::execute(program, parameters);

    std::cout << "Finished" << std::endl;

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
           QString first = index.sibling(index.row(), 0).data().toString(); // Ïåðâàÿ êîëîíêà
           QString second = index.sibling(index.row(), 1).data().toString();

           first.insert(0,"'");
           first.append("'");

           QSqlQuery qr;
           QString quer = "SELECT i.Name "
                           "FROM Recipes r "
                           "JOIN Composition c ON r.id = c.id_recipe "
                           "JOIN Ingredients i ON c.id_ingredient = i.id "
                           "WHERE r.name IN (" + first + ")";


          QSqlQueryModel *model2 = new QSqlQueryModel;//ñîçäàëè ìîäåëü äëÿ îòîáðàæåíèÿ çàñïðîñà
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
