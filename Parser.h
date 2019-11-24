//
// Created by abc on 24.11.2019.
//

#ifndef UNTITLED_PARSER_H
#define UNTITLED_PARSER_H
#include <fstream>
#include <queue>
#include "Table.h"

class Parser {
public:
    Parser(const std::string& input,const std::string& details,const std::string& result);
    void start();
private:
    void modifyTable(Table &table, const std::string& str);
    void write(std::list<Data> res);
    Data createData (const std::string& str);
    std::ifstream input;
    std::ofstream details;
    std::ofstream result;

    std::string
    createField(const std::string &str, std::priority_queue<std::tuple<size_t, Data::CompareField>> &priority,
                size_t &pos,
                size_t sizeCom, Data::FieldName name);
};


#endif //UNTITLED_PARSER_H
