//
// Created by abc on 20.11.2019.
//

#include "Data.h"

Data::Data (std::string geoPos, Date meaDate, Time sunrise, Time sundown, Data::Weather weather, int geoId, int level) {
    this->geoId = geoId;
    this->level = level;
    this->geoPos = std::move(geoPos);
    this->meaDate = meaDate;
    this->sunrise = sunrise;
    this->sundown = sundown;
    this->weather = weather;
}

Data::Weather Data::initWeather(const std::string & str){
    try {
        if (str == "fair") return fair;
        if (str == "rain") return rain;
        if (str == "cloudy") return cloudy;
        if (str == "snow") return snow;
        throw str;
    }
    catch (const std::string& bad){
        std::cout << "bad weather : " << bad << std::endl;
        return (fair);
    }
}

void Data::initFieldsPriority(std::list<CompareField> fields){
    fieldsPriority = std::move(fields);
}

int Data::getGeoId() {return this->geoId;}

int Data::getLevel() {return this->level;}

std::string Data::getGeoPos(){return this->geoPos;}

Date Data::getMeaDate(){return this->meaDate;}

Time Data::getSunrise(){return this->sunrise;}

Time Data::getSundown(){return this->sundown;}

Data::Weather Data::getWeather(){return this->weather;}

bool Data::operator==(Data d) {
    return this->geoId == d.getGeoId()     and
           this->level == d.getLevel()     and
           this->geoPos == d.getGeoPos()   and
           this->meaDate == d.getMeaDate() and
           this->sunrise == d.getSunrise() and
           this->sundown == d.getSundown() and
           this->weather == d.getWeather();
}

bool Data::operator!=(Data d) {
    return !(*this == std::move(d));
}

bool Data::operator<(Data d) {
    return compare(*this, std::move(d));
}

bool Data::operator>(Data d) {
    return !compare(*this, std::move(d));
}

bool Data::compare(Data d1, Data d2) {
    bool result = false;
    for(auto& element : fieldsPriority){
        FieldName name = std::get<0>(element);
        Cond cond = std::get<1>(element);

        if (name == FgeoId)   result = d1.compGeoId(d2.getGeoId(), cond);
        if (name == Flevel)   result = d1.compLevel(d2.getLevel(), cond);
        if (name == FgeoPos)  result = d1.compGeoPos(d2.getGeoPos(), cond);
        if (name == FmeaDate) result = d1.compMeaDate(d2.getMeaDate(), cond);
        if (name == Fsunrise) result = d1.compSunrise(d2.getSunrise(), cond);
        if (name == Fsundown) result = d1.compSundown(d2.getSundown(), cond);
        if (name == Fweather) result = d1.compWeather(d2.getWeather(), cond);

        if (paramSort and result) return result;
        else if (!result) return result;
    }
    return result;
}

bool Data::compGeoId(int val, Cond cond){
    if (cond == equal)    return this->geoId == val;
    if (cond == notEqual) return this->geoId != val;
    if (cond == greater)  return this->geoId > val;
    if (cond == less)     return this->geoId < val;
    return false;
}
bool Data::compLevel(int val, Cond cond){
    if (cond == equal)    return this->level == val;
    if (cond == notEqual) return this->level != val;
    if (cond == greater)  return this->level > val;
    if (cond == less)     return this->level < val;
    return false;
}
bool Data::compGeoPos(const std::string& val, Cond cond){
    if (cond == equal)    return this->geoPos == val;
    if (cond == notEqual) return this->geoPos != val;
    if (cond == greater)  return this->geoPos > val;
    if (cond == less)     return this->geoPos < val;
    return false;
}
bool Data::compMeaDate(Date val, Cond cond){
    if (cond == equal)    return this->meaDate == val;
    if (cond == notEqual) return this->meaDate != val;
    if (cond == greater)  return this->meaDate > val;
    if (cond == less)     return this->meaDate < val;
    return false;
}
bool Data::compSunrise(Time val, Cond cond){
    if (cond == equal)    return this->sunrise == val;
    if (cond == notEqual) return this->sunrise != val;
    if (cond == greater)  return this->sunrise > val;
    if (cond == less)     return this->sunrise < val;
    return false;
}
bool Data::compSundown(Time val, Cond cond){
    if (cond == equal)    return this->sundown == val;
    if (cond == notEqual) return this->sundown != val;
    if (cond == greater)  return this->sundown > val;
    if (cond == less)     return this->sundown < val;
    return false;
}
bool Data::compWeather(Weather val, Cond cond){
    if (cond == equal)    return this->weather == val;
    if (cond == notEqual) return this->weather != val;
    if (cond == greater)  return this->weather > val;
    if (cond == less)     return this->weather < val;
    return false;
}

std::string Data::weatherShow(Weather w){
    if (w == fair)   return "fair";
    else if (w == cloudy) return "cloudy";
    return w == rain ? "rain" : "show";
}

std::string Data::show(){
    std::string result = "| ";
    result += "geoId : " + std::to_string(geoId) + " | ";
    result += "level : " + std::to_string(level) + " | ";
    result += "geoPos : " + geoPos + " | ";
    result += "meaDate : " + meaDate.show() + " | ";
    result += "sunrise : " + sunrise.show() + " | ";
    result += "sundown : " + sundown.show() + " | ";
    result += "weather : " + weatherShow(weather) + " |\n";
    return result;
}

void Data::initSort() {
    paramSort = true;
}

void Data::disableSort() {
    paramSort = false;
}