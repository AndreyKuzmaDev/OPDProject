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
        //ui->statusBar->showMessage("db is open: " + db.databaseName());//������ � ������� ����������� � ���� ������

              // model = new QSqlTableModel(this,db);// ����� ���� ������ �� �����
              //  ui->tableView->setModel(model);
    }
    else
        ui->statusBar->showMessage("db have error: "+ db.lastError().databaseText());
}     //   ��������� ������� �������� � ���������� ����������� � ��
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
       //������� � ������ � ����� �������

         QString LowSearch = search.toLower();//��������� ������ � ������ ��������
         // ������ ����� ���������� ��������� ����� � ������))

          bool makeUpper = true;

              for (int i = 0; i < LowSearch.length(); i++) {
                  if (makeUpper && LowSearch[i].isLetter()) {
                      LowSearch[i] = LowSearch[i].toUpper();
                      makeUpper = false;
                  } else if (LowSearch[i] == ' ') {
                      makeUpper = true;
                  }
              }// ������ ����� ���������� ����������

              for(int i = 0; i < LowSearch.length(); i++)//������������ ������� � ������ ������
              {
                  if(LowSearch[i]==',')
                  {
                     LowSearch.insert(i,"'");
                     LowSearch.insert(i+3,"'");
                     i += 2;
                  }

              }//������� ����� ���� � �������

             //qDebug() << LowSearch;

        model = new QSqlQueryModel;//������� ������ ��� ����������� ��������

// /////////////////////////////////////////////////////////////////////////////////////////////
        const string separators{ " ,;:.\"!?'*\n" };
        vector <string> words; // ������ ��� �������� ����
        size_t start { LowSearch.toStdString().find_first_not_of(separators) };
        while (start != string::npos)
        {
            size_t end = LowSearch.toStdString().find_first_of(separators, start + 1);
            if (end == string::npos)
                end = search.toStdString().length();
            words.push_back(LowSearch.toStdString().substr(start, end - start));
            start = LowSearch.toStdString().find_first_not_of(separators, end + 1);
        }//������� ���� � ������
// ///////////////////////////////////////////////////////////////////////////////////////////////

        QSqlQuery qry;
        QString query = "SELECT r.name, r.Category "
                        "FROM Recipes r "
                        "JOIN Composition c ON r.id = c.id_recipe "
                         "JOIN Ingredients i ON c.id_ingredient = i.id "
                        "WHERE i.name IN (" + LowSearch + ")"
                        "GROUP BY r.name "
                        "HAVING COUNT(DISTINCT c.id_ingredient) = " + QString::number((int)words.size());

        if (qry.exec(query))//exec() ���������� ������ ��������, ������� ���������, ������� �� �������� ������.
        {
            model->setQuery(query);
            ui->tableView->setModel(model);

           model->setHeaderData(0,Qt::Horizontal,"Recipes", Qt::DisplayRole);//�������� �������� �������


          // ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//������������ �� ������ �������
           ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed); // ��������� ������ ��������� �������� �������
           int totalWidth = ui->tableView->width(); // ����� ������ TableView
           int firstColumnWidth = totalWidth * 0.55;
           int secondColumnWidth = totalWidth * 0.45;
           ui->tableView->setColumnWidth(0, firstColumnWidth); // ������ �������
           ui->tableView->setColumnWidth(1, secondColumnWidth); // ������ �������

          ui->tableView-> setSelectionBehavior(QAbstractItemView::SelectRows);//���������� ��� ������, � �� ���������� ������
          ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//��������� scroll �����/������
          // ui->tableView->setShowGrid(false); // c������� �����(�����? � � �� ����
        }
        else return;
    }


void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
           QString first = index.sibling(index.row(), 0).data().toString(); // ������ �������
           QString second = index.sibling(index.row(), 1).data().toString();

           first.insert(0,"'");
           first.append("'");

           QSqlQuery qr;
           QString quer = "SELECT i.Name "
                           "FROM Recipes r "
                           "JOIN Composition c ON r.id = c.id_recipe "
                           "JOIN Ingredients i ON c.id_ingredient = i.id "
                           "WHERE r.name IN (" + first + ")";


          QSqlQueryModel *model2 = new QSqlQueryModel;//������� ������ ��� ����������� ��������
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
