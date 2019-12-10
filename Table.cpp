//
// Created by abc on 20.11.2019.
//

#include "Table.h"

std::list<Data>& Table::getTable(){
    return (this->table);
}

std::string Table::show() {
    std::string res;
    for(auto& element : table){
        res += element.show();
    }
    return res;
}

void Table::insert(const Data& d){table.push_back(d);}

std::list<Data> Table::select(const Data& d){
    std::list<Data> result;
    for(auto& element : table){
        if (Data::compare(element, d)) result.push_back(element);
    }
    return result;
}

void Table::del(Data d){
    std::list<Data> del;
    for(auto& element : table){
        if (Data::compare(element, d)) del.push_back(element);
    }
    for (auto& element : del) {
        table.remove(element);
    }
}

void Table::update(const Data& d) {
    for(auto& element : table){
        if (Data::compare(element, d)) element = d;
    }
}

void Table::sort() {
    Data::initSort();
    table.sort(Data::compare);
    Data::disableSort();
}