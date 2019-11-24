//
// Created by abc on 19.11.2019.
//

#include "Date.h"

Date::Date (int year, int month, int day): day(day), month(month), year(year){}

//Установить дату по ее текстовому представлению
Date::Date (const std::string& value){
    //Положение первой точки - разделителя
    std::size_t p1 = value.find_first_of('.', 0);//Позиция первой точки
    std::size_t p2 = value.find_first_of('.', p1+1);
    int d = stoi(value.substr(0, p1));
    int m = stoi(value.substr(p1+1, p2));
    int y = stoi(value.substr(p2+1, value.size()));
    day = d;
    month = m;
    year = y;
}

bool Date::operator==(Date d) {
    return day == d.getDay() and month == d.getMonth() and d.year == d.getYear();
}

bool Date::operator!=(Date d) {
    return !(*this == d);
}

bool Date::operator< (Date d){
    if (year < d.getYear())  return true;
    if (year > d.getYear())  return false;
    if (month < d.getMonth())return true;
    if (month > d.getMonth())return false;
    return day < d.day;
}

bool Date::operator>(Date d) {
    return !(*this < d);
}

int Date::getDay  (){return this->day;}
int Date::getMonth(){return this->month;}
int Date::getYear (){return this->year;}

std::string Date::show() {
    std::string result;
    result += std::to_string(day) + ".";
    result += std::to_string(month) + ".";
    result += std::to_string(year);
    return result;
}
