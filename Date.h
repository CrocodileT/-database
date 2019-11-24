//
// Created by abc on 19.11.2019.
//

#ifndef UNTITLED_DATE_H
#define UNTITLED_DATE_H

#include <string>


class Date {
public:
    explicit Date (int year = 0, int month = 0, int day = 0);
    explicit Date (const std::string& value);

    int getDay ();
    int getMonth();
    int getYear();

    bool operator== (Date d);
    bool operator!= (Date d);
    bool operator< (Date d);
    bool operator> (Date d);

    std::string show();

private:
    int day;
    int month;
    int year;
};


#endif //UNTITLED_DATE_H
