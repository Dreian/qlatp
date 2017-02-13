// parser.cpp
// Implementation of a parser for SATLIB input files.

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <fstream>
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

// returns a set of clauses from a stream,
// expects SATLIB format
clause_set_t parse_stream(std::istream& in)
{
    std::string line;
    clause_set_t cls;
    do {
        std::getline(in, line);
        // debug_write(line << "\n");
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
    debug_write("Processed clause set" << std::endl);
    return cls;
}

double l = 1.0;

// parse and solve a concrete problem, input from
// a stream
bool solve_problem(std::istream& in)
{
    clause_set_t cs;
    try {
        cs = parse_stream(in); 
    } catch (const std::exception& ex) {
        debug_write("Parsing error detected!" << std::endl);
        debug_write(ex.what() << std::endl);
        debug_write("Finishing..." << std::endl);
        return false;
    }
    res_qlearn algo(cs, 100, l, 1000.0);
    //res_h3 algo(cs, 100);
    bool proved = algo.prove();
    debug_write((proved ? "SUCCESS" : "FAIL") << std::endl);
    return proved;
}

// accept a list of file names from an input stream, then
// solve all problems in the given files
void process_files(std::istream& in)
{
    std::string file_name;
    while (std::getline(in, file_name)) {
        std::fstream fs;
        debug_write("*******************************" << std::endl);
        debug_write(file_name << std::endl);
        debug_write("*******************************" << std::endl);
        fs.open(file_name, std::fstream::in);
        for (int i = 0; i < 5000; i++, l += 0.0001) {
            solve_problem(fs);
        }
        fs.close();
    }
}

// get file names from stdin
int main(int argc, char** argv)
{
    process_files(std::cin);
    return 0;
}

