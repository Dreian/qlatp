// parser.cpp
// Implementation of a parser for SATLIB input files.

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include "resolution.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#define debug_write(obj) \
    if (DEBUG) { \
        std::cout << obj; }

clause_set_t parse_stream(std::istream& in)
{
    std::string line;
    clause_set_t cls;
    do {
        std::getline(in, line);
        debug_write(line << "\n");
    } while (line[0] == 'c');
    line = line.substr(line.find(" ") + 1);
    line = line.substr(line.find(" ") + 1);
    line = line.substr(line.find(" ") + 1);
    int clause_cnt = std::stoi(line);
    debug_write("Number of clauses: " << clause_cnt << "\n");
    for (int i = 0; i < clause_cnt; i++) {
        clause_t cl;
        int lit = 0;
        do {
            in >> lit;
            if (lit > 0) {
                cl.insert(literal_t(lit, true));
            } else if (lit < 0) {
                cl.insert(literal_t(-lit, false));
            }
        } while (lit != 0);
        cls.insert(cl);
    }
    return cls;
}

// temporary, for testing purposes
int main(int argc, char** argv)
{
    srand(time(0));
    debug_write("Hello!\n");
    clause_set_t cs = parse_stream(std::cin);
    res_h3 algo(cs, 1000);
    std::cout << algo.prove() << std::endl;
    return 0;
}
