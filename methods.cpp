//#include "methods.h"
//#include <iostream>
//#include <string>
//#include <vector>
//using namespace std;


//methods::methods()
//{
//  void methods1()
//  {
//      const string separators{ " ,;:.\"!?'*\n" }; // ����������� ����
//      vector <string> words; // ������ ��� �������� ����
//      size_t start { search.toStdString().find_first_not_of(separators) }; // ��������� ������ ������� �����
//      while (start != string::npos) // ��������, ���� � ������ �� �������� ������ ��������, ����� separators
//      {
//          size_t end = search.toStdString().find_first_of(separators, start + 1); // �������, ��� ��������� �����
//          if (end == string::npos) // ���� �� ������ �� ���� �� ��������-������������
//              end = search.toStdString().length();        // ������������� ���������� end �� ����� ������
//          words.push_back(search.toStdString().substr(start, end - start)); // ��������� � ������ �����
//          start = search.toStdString().find_first_not_of(separators, end + 1); // ������� ��������� ������ ���������� ����� � ����������������� start
//      }
//  }
//}
