//
// Created by abc on 20.11.2019.
//

#include "Time.h"

Time::Time (int hour, int minute) : hour(hour) , minute(minute){}

Time::Time (const std::string& value)
{
    //h.mm or h.mm
    size_t p = value.find_first_of(':');
    hour = stoi(value.substr(0, p));
    minute = stoi (value.substr(p+1,value.size()));
}

bool Time::operator== (Time t){
    return hour == t.getHour() and minute == t.getMinute();
}

bool Time::operator!= (Time t){
    return !(*this == t);
}

bool Time::operator< (Time t) {
    if (hour < t.getHour()) return true;
    if (hour > t.getHour()) return false;
    return minute < t.getMinute();
}

bool Time::operator> (Time t) {
    return !(*this < t);
}

int Time::getHour(){return this->hour;}
int Time::getMinute(){return this->minute;}

std::string Time::show() {
    std::string result;
    result += std::to_string(hour) + ".";
    result += std::to_string(minute);
    return result;
}