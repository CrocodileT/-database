//
// Created by abc on 20.11.2019.
//

#ifndef UNTITLED_DATA_H
#define UNTITLED_DATA_H

#include <iostream>
#include <string>
#include <tuple>
#include <list>
#include <utility>
#include "Date.h"
#include "Time.h"

//Database type

class Data {
public:
    enum Cond {equal, notEqual, greater, less};
    enum FieldName {FgeoId, Flevel, FgeoPos, FmeaDate, Fsunrise, Fsundown, Fweather};
    typedef std::tuple <FieldName, Cond> CompareField;

    enum Weather {fair, rain, cloudy, snow };
    static Weather initWeather(const std::string & str);
    explicit Data (std::string geoPos, Date meaDate, Time sunrise, Time sundown, Weather weather = fair, int geoId = 0, int level = 0);

    static void initFieldsPriority(std::list<CompareField> fields);

    int getGeoId();
    int getLevel();
    std::string getGeoPos();
    Date getMeaDate();
    Time getSunrise();
    Time getSundown();
    Weather getWeather();

    static bool compare(Data d1, Data d2);

    bool operator== (Data d);
    bool operator!= (Data d);
    bool operator< (Data d);
    bool operator> (Data d);

    std::string show();

    static std::string weatherShow(Weather w);
    static void initSort();
    static void disableSort();

private:

    bool compGeoId(int val, Cond cond);
    bool compLevel(int val, Cond cond);
    bool compGeoPos(const std::string& val, Cond cond);
    bool compMeaDate(Date val, Cond cond);
    bool compSunrise(Time val, Cond cond);
    bool compSundown(Time val, Cond cond);
    bool compWeather(Weather val, Cond cond);

    static std::list<CompareField> fieldsPriority;
    static bool paramSort;

    int geoId; //station ID
    int level; //height above water level
    std::string geoPos; // The location of the station, address, coordinates
    Date meaDate; // Date of weather observations
    Time sunrise; // Sunrise time
    Time sundown; // Sundown time
    Weather weather; // General weather conditions during sunrise
};


#endif //UNTITLED_DATA_H
