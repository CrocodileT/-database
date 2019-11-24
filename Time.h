//
// Created by abc on 20.11.2019.
//

#ifndef UNTITLED_TIME_H
#define UNTITLED_TIME_H

#include <string>


class Time {
public:
    explicit Time (int hour = 0, int minute = 0);
    explicit Time (const std::string& value);

    bool operator== (Time t);
    bool operator!= (Time t);
    bool operator< (Time t);
    bool operator> (Time t);

    int getHour();
    int getMinute();

    std::string show();
private:
    int hour;
    int minute;
};


#endif //UNTITLED_TIME_H
