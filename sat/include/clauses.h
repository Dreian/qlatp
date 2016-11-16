// clauses.h
// Definitions of all relevant clause and literal types

#include <set>
#include <utility>

// definitions of fundamental SAT objects
typedef unsigned int proposition_t;
typedef std::pair<proposition_t, bool> literal_t;
typedef std::set<literal_t> clause_t;
typedef std::set<clause_t> clause_set_t;
