#include "db_manager.h"


DB_manager::DB_manager()
{
    db_ready = true;
}


void DB_manager::search(QString request, QString cathegory)
{

    model = new QSqlQueryModel;

    if (!db_ready)
    {
        qDebug() << "DB is currently updating, pls wait";
        emit search_done(model);
    }

    if (!db.open())
    {
        qDebug() << "DB isn't open, pls open something";
        emit search_done(model);
    }

    request.remove(QChar(' '), Qt::CaseInsensitive);//Voda,Voda

    request.insert(0,"'");
    request.append("'");//'Voda,Voda'

    request = request.toLower();
    request[1] = request[1].toUpper();

    for (int i = 0; i < request.length(); i++) {
        if (request[i] == ',') {
            request[i+1] = request[i+1].toUpper();
        }
    }

    for(int i = 0; i < request.length(); i++)
    {
        if(request[i]==',')
        {
            request.insert(i,"'");
            request.insert(i+2,"'");
            i += 1;
        }
    }
    //--------------edit search

    words.clear();//words is liststring for counting
// /////////////////////////////////////////////////////////////////////////////////////////////
    const QString separators = " ,;:.\"!?'*\n";
    int start = request.indexOf(QRegExp("[^" + separators + "]"));
    while (start != -1)
    {
        int end = request.indexOf(QRegExp("[" + separators + "]"), start + 1);
        if (end == -1)
            end = request.length();

        words.push_back(request.mid(start, end - start));

        start = request.indexOf(QRegExp("[^" + separators + "]"), end + 1);
    }
// ///////////////////////////////////////////////////////////////////////////////////////////////

    QSqlQuery qry;
    QString query;

    if(cathegory == CATHEGORY_EMPTY)//if not fillter or fillter = bez filltracii
    {
        query = "SELECT r.name, r.Category "
                    "FROM Recipes r "
                    "JOIN Composition c ON r.id = c.id_recipe "
                    "JOIN Ingredients i ON c.id_ingredient = i.id "
                    "WHERE i.name IN (" + request + ")"
                    "GROUP BY r.name "
                    "HAVING COUNT(DISTINCT c.id_ingredient) = " + QString::number((int)words.size());
    }

    else// else query with selected category
    {
        query = "SELECT r.name, r.Category "
                        "FROM Recipes r "
                        "JOIN Composition c ON r.id = c.id_recipe "
                        "JOIN Ingredients i ON c.id_ingredient = i.id "
                        "WHERE i.name IN (" + request + ")"
                        "AND  r.Category = '"+ cathegory +"' "
                        "GROUP BY r.name "
                        "HAVING COUNT(DISTINCT c.id_ingredient) = " + QString::number((int)words.size());
    }

    if (qry.exec(query))//exec() - bool funct : 1 if  query work,  else 0
    {
        model->setQuery(query);
        model->setHeaderData(0,Qt::Horizontal,"Recipes", Qt::DisplayRole);
    }
    emit search_done(model);
}


void DB_manager::open_DB(QString path)
{
    if (db.open())
        db.close();

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    QString message;
    if (db.open())
    {
        message = get_name();
        get_cathegories();
    }
    else
    {
        message = get_err();
    }
    emit DB_opened(db.open(), message, &cathegories);
}


void DB_manager::create_DB(QString path, QString name)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path + "/" + name);

    if(db.open())
    {
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
        model = new QSqlQueryModel;
        if (qr.exec(quer1) && qr.exec(quer2) && qr.exec(quer3))
        {
            model->setQuery(quer3);
            model->setQuery(quer2);
            model->setQuery(quer1);
            // ui->tableView->setModel(model);
            // qDebug() << "here";

        }
    }

    QString message;
    if (db.open())
    {
        message = get_name();
        get_cathegories();
    }
    else
    {
        message = get_err();
    }

    emit DB_created(db.open(), message, &cathegories);
}


void DB_manager::get_cathegories_slot()
{
    get_cathegories();
    emit got_cathegories(&cathegories);
}


QString DB_manager::get_name()
{
    return db.databaseName();
}


QString DB_manager::get_err()
{
    return db.lastError().databaseText();
}


void DB_manager::get_cathegories()
{
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
            if (cathegories.contains(tmp) == false)
                cathegories << tmp;
        }
    }
    delete modeltmp;
}

void DB_manager::get_recipe_details(QString recipe_name)
{
    QSqlQuery qr;
    QString quer = "SELECT i.Name,c.number,i.Unit "    // all ingredients for current recipe
                   "FROM Recipes r "
                   "JOIN Composition c ON r.id = c.id_recipe "
                   "JOIN Ingredients i ON c.id_ingredient = i.id "
                   "WHERE r.name IN ('" + recipe_name + "')";

    QSqlQueryModel *model2 = new QSqlQueryModel;// new model for a simultaneosly process

    QString result = "<div><div style='text-align: left;'><center>ingredients<\center></div><br/>";
    bool contains;
    if (qr.exec(quer))
    {
        model2->setQuery(quer);
        QString ingredient;
        QString amount = " ";
        int maxlength = 0;
        int curlength = 0;
        for (int row = 0; row < model2->rowCount(); ++row)
        {
            QModelIndex index = model2->index(row, 0);
            curlength = model2->data(index).toString().length();
            if(curlength > maxlength)
                maxlength = curlength;
        }

        for (int row = 0; row < model2->rowCount(); ++row)
        {
            amount.clear();

            QModelIndex index = model2->index(row, 0);
            ingredient = model2->data(index).toString();

            for(int column = 1; column < model2->columnCount(); ++column)
            {
                QModelIndex index = model2->index(row, column);
                amount+= model2->data(index).toString();
                amount += " ";
            }

            int tempLength = maxlength - ingredient.length();
            contains = false;
            for(QString i : words)
            {
                if(i == ingredient)
                {
                    contains = true;
                    break;
                }
            }
            if (contains)
                result  += "<b>" + ingredient +"</b>"; // teg <s> for crossing out
            else
                result += ingredient;

            result += QString(tempLength + 4, '.') + amount + "<br/>";
        }
    }

    quer = "SELECT r.Link "
           "FROM Recipes r "
           "WHERE r.name IN ('" + recipe_name + "')";

    if (qr.exec(quer))
    {
        model2->setQuery(quer);
        QModelIndex index = model2->index(0, 0);
        QUrl link(model2->data(index).toString());
        result  += "<br/><a href=\"" + link.toString() + "\" >this recipe</a>";
    }
    result += "<\div>";
    delete model2;
    emit got_recipe_details(result);
}





