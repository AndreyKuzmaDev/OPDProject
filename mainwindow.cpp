#include <QProcess>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/prog/project/temp/delicious.db");

    if(db.open())
    {
        //ui->statusBar->showMessage("db is open: " + db.databaseName());//������ � ������� ����������� � ���� ������

              // model = new QSqlTableModel(this,db);// ����� ���� ������ �� �����
              //  ui->tableView->setModel(model);
    }
    else
        ui->statusBar->showMessage("db have error: "+ db.lastError().databaseText());
}

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

             // qDebug() << LowSearch;


              for(int i = 0; i < LowSearch.length(); i++)//������������ ������� � ������ ������
              {
                  if(LowSearch[i]==',')
                  {
                     LowSearch.insert(i,"'");
                     LowSearch.insert(i+3,"'");
                     i += 2;
                  }

              }//������� ��� Sqlite �������

             //qDebug() << LowSearch;



        QSqlQueryModel *model = new QSqlQueryModel;//������� ������ ��� ����������� ��������

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
        }//������� ���� � ������ �������
// ///////////////////////////////////////////////////////////////////////////////////////////////

        QSqlQuery qry;
        QString query = "SELECT r.name, r.Category, r.Link "
                        "FROM Recipes r "
                        "JOIN Composition c ON r.id = c.id_recipe "
                         "JOIN Ingredients i ON c.id_ingredient = i.id "
                        "WHERE i.name IN (" + LowSearch + ")"
                        "GROUP BY r.name "
                        "HAVING COUNT(DISTINCT c.id_ingredient) = " +  QString::number((int)words.size());

    // qDebug() << query;

        if (qry.exec(query))
        {
            model->setQuery(query);
            ui->tableView->setModel(model);
           model->setHeaderData(0,Qt::Horizontal,"Recipes", Qt::DisplayRole);//�������� �������� �������
           ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//������������ �� ������ �������
           //ui->tableView->setShowGrid(false); // c������� �����

          //� ���������� ��� ������� �� ������ ������ ��������
           //����� ���� � ����������� ����������� ��������� � ������ ��������� ������� ���� ����������
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
}
