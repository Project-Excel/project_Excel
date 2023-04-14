#include <iostream>
#include <windows.h>
#include "libxl.h"
#include <vector>
#include <string>
#include <sstream>

using namespace libxl;

const wchar_t* filename = L"testxl.xlsx";

void writeToFile(std::vector<std::vector<std::pair<std::string, std::pair<int, int>>>> sheetinfo) {
    Book* book = xlCreateXMLBook();
    for (int i = 0; i < sheetinfo.size(); i++) {

        const wchar_t shname[6]{ 115, 104, 101, 101, 116, i + 48 };
        Sheet* sh1 = book->addSheet(shname);
        for (int k = 0; k < sheetinfo[i].size(); k++) {
            std::wstring widestr = std::wstring(sheetinfo[i][k].first.begin(), sheetinfo[i][k].first.end());
            const wchar_t* widecstr = widestr.c_str();

            sh1->writeStr(sheetinfo[i][k].second.first, sheetinfo[i][k].second.second, widecstr);
        }
    }

}

std::string to_string(const double& d)
{
    std::ostringstream strm;
    strm << d;
    return strm.str();
}

void  addtotable(std::vector<std::vector<std::pair<std::string, std::pair<int, int>>>> sheetinfo) {
    //for (size_t i = 0; i < sheetinfo[i].size(); i++) {
        //writeTocell(sheetinfo[i].first, sheetinfo[i].second.first, sheetinfo[i].second.second);
    //}
    //функци€ дл€ добавлени€ информации из вектора в €чейку,writetocell(std::string, int, int)/ инт инт дл€ адреса €чейки
    //все данные из хслх файла считываютс€ в виде std::string, дл€ большей инфы пиши в лс
}

std::vector<std::vector<std::pair<std::string, std::pair<int, int>>>> readfromfile()
{
    //вектор(вектор(пара(инфо, координаты)))
    std::vector<std::vector<std::pair<std::string, std::pair<int, int>>>> sheetinfo;

    Book* book = xlCreateXMLBook();
    // дл€  считывани€ из файла
    if (book->load(filename)) {
        int i = 0;
        Sheet* sh1 = book->getSheet(i);
        while (sh1) {

            std::vector<std::pair<std::string, std::pair<int, int>>> sh1info;
            for (int row = sh1->firstRow(); row < sh1->lastRow(); row++) {
                for (int col = sh1->firstCol(); col < sh1->lastCol(); col++) {

                    CellType cell = sh1->cellType(row, col);

                    if (sh1->isFormula(row, col)) {

                        std::pair <std::string, std::pair<int, int>> buf1;
                        std::pair<int, int> buf2(row, col);
                        std::wstring ws = sh1->readFormula(row, col);
                        std::string temp(ws.begin(), ws.end());
                        buf1.first = temp;
                        buf1.second = buf2;
                        sh1info.push_back(buf1);
                    }

                    else {

                        if (cell == CELLTYPE_NUMBER) {
                            double temp = sh1->readNum(row, col);
                            std::pair<int, int> buf2(row, col);
                            std::pair <std::string, std::pair<int, int>> buf1(to_string(temp), buf2);
                            sh1info.push_back(buf1);
                        }
                        else if (cell == CELLTYPE_STRING) {
                            std::pair<int, int> buf2(row, col);
                            std::wstring ws(sh1->readStr(row, col));
                            std::string temp(ws.begin(), ws.end());
                            std::pair <std::string, std::pair<int, int>> buf1(temp, buf2);
                        }
                        else {
                            int year, month, day;
                            book->dateUnpack(sh1->readNum(row, col), &year, &month, &day);
                            std::pair<int, int> buf2(row, col);

                            std::string datetemp(to_string((double)year));
                            datetemp += '.';
                            datetemp += to_string((double)month);
                            datetemp += '.';
                            datetemp += to_string((double)day);

                            std::pair <std::string, std::pair<int, int>> buf1(datetemp, buf2);
                            sh1info.push_back(buf1);
                        }
                    }


                }
            }
            sheetinfo.push_back(sh1info);
            i++;
            sh1 = book->getSheet(i);
        }

    }
    book->release();

    return sheetinfo;
}
