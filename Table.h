//
// Created by abc on 20.11.2019.
//

#ifndef UNTITLED_TABLE_H
#define UNTITLED_TABLE_H

#include <list>
#include "Data.h"

class Table {
public:
    std::string show();
    std::list<Data>& getTable();

    void insert(const Data& d);
    std::list<Data> select(const Data& d);
    void del(Data d);
    void update(const Data& d);
    void sort();
private:
    std::list<Data> table;
};


#endif //UNTITLED_TABLE_H
