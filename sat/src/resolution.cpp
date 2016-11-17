// resolution.cpp
// Implementation of the given clause algorithm and the most important helper
// methods. Also present are implementations of basic heuristics.

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <utility>
#include "resolution.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#define debug_write(obj) \
    if (DEBUG) { \
        std::cout << obj; }

// A debugging method for pretty-printing a clause
void print_clause(clause_t clause)
{
    if (DEBUG) {
        debug_write("{ ");
        for (literal_t lit : clause) {
            if (!std::get<1>(lit)) {
                debug_write("-");
            }
            debug_write(std::get<0>(lit));
            debug_write(" ");
        }
        debug_write("} ");
    }
}

// Base constructor of every resolution algorithm
resolution_algorithm::resolution_algorithm(clause_set_t& clauses)
{
    unprocessed = clauses;
    debug_write("Created the algorithm instance\n");
}

// Base destructor of every resolution algorithm
resolution_algorithm::~resolution_algorithm() {}

// Proof procedure of every resolution algorithm. Implemented only in base
// class, it follows the given clause algorithm
bool resolution_algorithm::prove(void)
{
    bool proved = false;
    clause_t chosen_clause;
    // main loop
    while (!unprocessed.empty() && !proved && !should_reject()) {
        // more detailed debug information
        // not needed here
        /*if (DEBUG) {
            debug_write("Processed: ");
            for (clause_t cl : processed) {
                print_clause(cl);
            }
            debug_write("| Unprocessed: ");
            for (clause_t cl : unprocessed) {
                print_clause(cl);
            }
        }*/
        // choose clause (based on heuristic)
        chosen_clause = choose_clause();
        // did we find a contradiction?
        if (chosen_clause.empty()) {
            proved = true;
        } else {
            // perform all possible resolutions
            processed.insert(chosen_clause);
            generate(chosen_clause);
        }
        //debug_write("\n");
    }
    // result of proof attempt?
    return proved;
}

// Helper method for the theorem proving algorithm. Given two clause
// references and a reference to the appropriate literal, it produces the
// clause inferred with the resolution rule
clause_t resolution_algorithm::resolve(clause_t& clause_a, clause_t& clause_b,
                                       literal_t& lit_res)
{
    // first clause contains the appropriate literal?
    assert(clause_a.find(lit_res) != clause_a.end());
    clause_t new_clause;
    clause_t empty_clause;
    // copy literals from the two clauses, while avoiding duplicates
    // inefficient with current implementation, consider different way of
    // representing clauses
    for (literal_t lit : clause_a) {
        if (lit != lit_res) {
            literal_t opp_lit(std::get<0>(lit), !(std::get<1>(lit)));
            if (clause_b.find(opp_lit) != clause_b.end()) {
                return empty_clause;
            } else {
                new_clause.insert(lit);
            }
        }
    }
    for (literal_t lit : clause_b) {
        if (lit != lit_res) {
            literal_t opp_lit(std::get<0>(lit), !(std::get<1>(lit)));
            if (clause_a.find(opp_lit) == clause_a.end()) {
                new_clause.insert(lit);
            }
        }
    }
    return new_clause;
}

// Generation step in the given clause algorithm. Given a clause, resolution is
// performed with every claused in the processed clause set.
void resolution_algorithm::generate(clause_t& clause)
{
    // new clauses getting build
    clause_t clause_res;
    // iterate over all literals in clause
    for (literal_t lit : clause) {
        literal_t opp_lit(std::get<0>(lit), !(std::get<1>(lit)));
        // iterate over all processed clauses
        for (clause_t proc : processed) {
            // can this resolution be performed?
            if (proc.find(opp_lit) != proc.end()) {
                clause_res = resolve(clause, proc, lit);
                // is this a clause we have not seen before?
                if (processed.find(clause_res) == processed.end() &&
                    unprocessed.find(clause_res) == unprocessed.end()) {
                    unprocessed.insert(clause_res);
                }
            }
        }
    }
}

// H1 constructor
// adds nothing
res_h1::res_h1(clause_set_t& clauses) : resolution_algorithm(clauses)
{
    debug_write("H1 used\n");
}

// H1 method of choosing the clause
// always takes the first one
clause_t res_h1::choose_clause(void)
{
    clause_set_t::iterator it = (*get_unprocessed()).begin();
    clause_t chosen = *it;
    (*get_unprocessed()).erase(it);
    return chosen;
}

// H1 method of rejecting a set of clauses
// only if set of unprocessed clauses is empty
bool res_h1::should_reject(void)
{
    return (*get_unprocessed()).empty();
}

// H2 constructor
// maintains number of steps
res_h2::res_h2(clause_set_t& clauses, int steps) :
    resolution_algorithm(clauses)
{
    if (steps <= 0) {
        throw "Could not create resolution algorithm";
    }
    steps_limit = steps;
    steps_taken = 0;
    debug_write("H2 used\n");
}

// H2 method of choosing the clause
// pick a random one
clause_t res_h2::choose_clause(void)
{
    clause_set_t::iterator it = (*get_unprocessed()).begin();
    advance(it, rand() % (*get_unprocessed()).size());
    clause_t chosen = *it;
    (*get_unprocessed()).erase(it);
    steps_taken++;
    return chosen;
}

// H2 method of rejecting a set of clauses
// if set of unprocessed clauses is empty or too many steps
bool res_h2::should_reject(void)
{
    return (*get_unprocessed()).empty() || steps_taken == steps_limit;
}

// H3 constructor
// maintains number of steps
res_h3::res_h3(clause_set_t& clauses, int steps) :
    resolution_algorithm(clauses)
{
    if (steps <= 0) {
        throw "Could not create resolution algorithm";
    }
    steps_limit = steps;
    steps_taken = 0;
    debug_write("H3 used\n");
}

// H3 method of choosing the clause
// pick a random one out of all the shortest ones
clause_t res_h3::choose_clause(void)
{
    clause_set_t::iterator it = (*get_unprocessed()).begin();
    int min_size = (*it).size();
    int min_cnt = 1;
    it++;
    for (; it != (*get_unprocessed()).end(); it++) {
        if (min_size > (*it).size()) {
            min_size = (*it).size();
            min_cnt = 1;
        } else if (min_size == (*it).size()) {
            min_cnt++;
        }
    }
    clause_set_t::iterator chosen_it;
    clause_t chosen;
    int which_one = 1 + rand() % min_cnt;
    for (it = (*get_unprocessed()).begin();
         it != (*get_unprocessed()).end() && which_one > 0; it++) {
        if (min_size == (*it).size()) {
            which_one--;
            if (which_one == 0) {
                chosen_it = it;
            }
        }
    }
    chosen = *chosen_it;
    (*get_unprocessed()).erase(chosen_it);
    steps_taken++;
    return chosen;
}

// H3 method of rejecting a set of clauses
// if set of unprocessed clauses is empty or too many steps
bool res_h3::should_reject(void)
{
    return (*get_unprocessed()).empty() || steps_taken == steps_limit;
}

