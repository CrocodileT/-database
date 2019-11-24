//
// Created by abc on 24.11.2019.
//

#include "Parser.h"

void Parser::write(std::list<Data> res){
    for(auto& element : res) {
        result << element.show();
    }
    result << "---------------------------------" << std::endl;
}

void Parser::modifyTable(Table &table, const std::string& str) {
    Data d = createData(str);
    std::list<Data> res;
    try {
        if (str[0] == '-' and str[1] == '-') return;
        else if (str.find("insert") != std::string::npos) table.insert(d);
        else if (str.find("select") != std::string::npos) write(table.select(d));
        else if (str.find("delete") != std::string::npos) table.del(d);
        else if (str.find("update") != std::string::npos) table.update(d);
        else if (str.find("sort") != std::string::npos) table.sort();
        else if (str.find("print") != std::string::npos) write(table.getTable());
        else{
            throw str;
        }
    }
    catch (const std::string& str){
        details << "Bad command: " << str;
    }
}


std::string Parser::createField(const std::string& str, std::priority_queue<std::tuple<size_t, Data::CompareField>>& priority,
        size_t &pos, size_t sizeCom, Data::FieldName name) {
    Data::CompareField cf;

    pos += sizeCom;

    if (str[pos] == '=') {
        cf = {std::make_tuple(name, Data::equal)};
        pos += 1;
    }
    else if (str[pos] == '<') {
        cf = {std::make_tuple(name, Data::less)};
        pos += 1;
    }
    else if (str[pos] == '>') {
        cf = {std::make_tuple(name, Data::greater)};
        pos += 1;
    }
    else if (str[pos + 1] == '=') {
        cf = {std::make_tuple(name, Data::notEqual)};
        pos += 2;
    }

    size_t lastPos = str.find_first_of(' ', pos);

    priority.push(std::make_tuple(pos, cf));

    return (str.substr(pos, (lastPos - pos)));
}

Data Parser::createData(const std::string& str) {
    int geoId = 0; //station ID
    int level = 0; //height above water level
    std::string geoPos; // The location of the station, address, coordinates
    Date meaDate; // Date of weather observations
    Time sunrise; // Sunrise time
    Time sundown; // Sundown time
    Data::Weather weather = Data::fair; // General weather conditions during sunrise
    std::list<Data::CompareField> fields;

    size_t sizeCom = 0;
    size_t pos = 0;
    Data::FieldName name;

    std::priority_queue<std::tuple<size_t, Data::CompareField>> priority;

    if (std::string::npos != (pos = str.find("geoId"))) {
        sizeCom = 5;
        name = Data::FgeoId;
        geoId = std::stoi(createField(str, priority, pos, sizeCom, name));
    }
    pos = 0;
    if (std::string::npos != (pos = str.find("level"))) {
        sizeCom = 5;
        name = Data::Flevel;
        level = std::stoi(createField(str, priority, pos, sizeCom, name));
    }
    pos = 0;
    if (std::string::npos != (pos = str.find("geoPos"))) {
        sizeCom = 6;
        name = Data::FgeoPos;
        geoPos = createField(str, priority, pos, sizeCom, name);
    }
    pos = 0;
    if (std::string::npos != (pos = str.find("meaDate"))) {
        sizeCom = 7;
        name = Data::FmeaDate;
        meaDate = Date(createField(str, priority, pos, sizeCom, name));
    }
    pos = 0;
    if (std::string::npos != (pos = str.find("sunrise"))) {
        sizeCom = 7;
        name = Data::Fsunrise;
        sunrise = Time(createField(str, priority, pos, sizeCom, name));
    }
    pos = 0;
    if (std::string::npos != (pos = str.find("sundown"))) {
        sizeCom = 7;
        name = Data::Fsundown;
        sundown = Time(createField(str, priority, pos, sizeCom, name));
    }
    pos = 0;
    if (std::string::npos != (pos = str.find("weather"))) {
        sizeCom = 7;
        name = Data::Fweather;
        weather = Data::initWeather(createField(str, priority, pos, sizeCom, name));
    }
    Data d(geoPos, meaDate, sunrise, sundown, weather ,geoId, level);

    while (!priority.empty()){
        fields.push_front(std::get<1>(priority.top()));
        priority.pop();
    }

    Data::initFieldsPriority(fields);
    return d;
}

Parser::Parser(const std::string& input,const std::string& details,const std::string& result) {
    this->input.open(input);
    this->details.open(details);
    this->result.open(result);
    if (!this->input.is_open()) this->details << input + " is not open\n";
    if (!this->details.is_open()) this->details << details + " is not open\n";
    if (!this->result.is_open()) this->details << result + " is not open\n";
}

void Parser::start() {
    Table t;
    std::string str;
    while(getline(input, str)){
        modifyTable(t,str);
    }
    input.close();
    details.close();
    result.close();
}

