#include <iostream>
#include "Parser.h"
std::list<Data::CompareField> Data::fieldsPriority;
bool Data::paramSort = false;
int main() {
    std::string input("input.txt");
    std::string details("detail.txt");
    std::string result("result.txt");

    Parser parser(input,details,result);
    parser.start();

    return 0;
}