//#include "methods.h"
//#include <iostream>
//#include <string>
//#include <vector>
//using namespace std;


//methods::methods()
//{
//  void methods1()
//  {
//      const string separators{ " ,;:.\"!?'*\n" }; // разделители слов
//      vector <string> words; // вектор для хранения слов
//      size_t start { search.toStdString().find_first_not_of(separators) }; // начальный индекс первого слова
//      while (start != string::npos) // проходим, пока в строке не окажется других символов, кроме separators
//      {
//          size_t end = search.toStdString().find_first_of(separators, start + 1); // находим, где кончается слово
//          if (end == string::npos) // если НЕ найден ни один из символов-разделителей
//              end = search.toStdString().length();        // устанавливаем переменную end на конец текста
//          words.push_back(search.toStdString().substr(start, end - start)); // добавляем в вектор слово
//          start = search.toStdString().find_first_not_of(separators, end + 1); // находим начальный индекс следующего слова и переустанавливаем start
//      }
//  }
//}
