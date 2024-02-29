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
        //ui->statusBar->showMessage("db is open: " + db.databaseName());//запись о удачном подключении к базе данных

              // model = new QSqlTableModel(this,db);// вывод базы данных на экран
              //  ui->tableView->setModel(model);
    }
    else
        ui->statusBar->showMessage("db have error: "+ db.lastError().databaseText());
}     //   ОБРАБОТКА СОБЫТИЙ УДАЧНОГО И НЕУДАЧНОГО ПОДКЛЮЧЕНИЯ К БД
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
       //КАВЫЧКИ В НАЧАЛИ И КОНЦЕ ЗАПРОСА

         QString LowSearch = search.toLower();//поисковая строка в нижнем регистре
         // теперь нужно проставить заглавные буквы в словах))

          bool makeUpper = true;

              for (int i = 0; i < LowSearch.length(); i++) {
                  if (makeUpper && LowSearch[i].isLetter()) {
                      LowSearch[i] = LowSearch[i].toUpper();
                      makeUpper = false;
                  } else if (LowSearch[i] == ' ') {
                      makeUpper = true;
                  }
              }// ПЕРВЫЕ БУКВЫ СТАНОВЯТСЯ ЗАГЛАВНЫМИ

              for(int i = 0; i < LowSearch.length(); i++)//Просставляем ковычки в нужных местах
              {
                  if(LowSearch[i]==',')
                  {
                     LowSearch.insert(i,"'");
                     LowSearch.insert(i+3,"'");
                     i += 2;
                  }

              }//КАВЫЧКИ МЕЖДУ СЛОВ В ЗАПРОСЕ

             //qDebug() << LowSearch;

        QSqlQueryModel *model = new QSqlQueryModel;//создали модель для отображения заспроса

// /////////////////////////////////////////////////////////////////////////////////////////////
        const string separators{ " ,;:.\"!?'*\n" };
        vector <string> words; // вектор для хранения слов
        size_t start { LowSearch.toStdString().find_first_not_of(separators) };
        while (start != string::npos)
        {
            size_t end = LowSearch.toStdString().find_first_of(separators, start + 1);
            if (end == string::npos)
                end = search.toStdString().length();
            words.push_back(LowSearch.toStdString().substr(start, end - start));
            start = LowSearch.toStdString().find_first_not_of(separators, end + 1);
        }//ПОДСЧЕТ СЛОВ В СТРОКЕ
// ///////////////////////////////////////////////////////////////////////////////////////////////

        QSqlQuery qry;
        QString query = "SELECT r.name, r.Category "
                        "FROM Recipes r "
                        "JOIN Composition c ON r.id = c.id_recipe "
                         "JOIN Ingredients i ON c.id_ingredient = i.id "
                        "WHERE i.name IN (" + LowSearch + ")"
                        "GROUP BY r.name "
                        "HAVING COUNT(DISTINCT c.id_ingredient) = " + QString::number((int)words.size());

        if (qry.exec(query))//exec() возвращает булево значение, которое указывает, успешно ли выполнен запрос.
        {
            model->setQuery(query);
            ui->tableView->setModel(model);

           model->setHeaderData(0,Qt::Horizontal,"Recipes", Qt::DisplayRole);//изменили название столбца

           ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//выравнивание по ширине виджета
          // ui->tableView->horizontalHeader()->setSectionResizeMode(0,);
          ui->tableView-> setSelectionBehavior(QAbstractItemView::SelectRows);//выделяется вся строка, а не конкретная ячейка

          // ui->tableView->setShowGrid(false); // cкрывает сетку(ЗАЧЕМ? а я не знаю)

          //в дальнейшем при нажатии на рецепт должно вылезать
           //новое окно с количеством необходимых продуктов и список продуктов которых надо приобрести
        }
        else return;
    }


void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
           QString first = index.sibling(index.row(), 0).data().toString(); // Первая колонка
           QString second = index.sibling(index.row(), 1).data().toString();

           qDebug() << first << " -> " << second;


    }
}
